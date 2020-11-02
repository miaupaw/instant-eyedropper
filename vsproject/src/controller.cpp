/************************************************************************************************************
** controller
** процедура обработки сообщений от винды, используется класс "контроллер"
**                   __              __ __            
** .----.-----.-----|  |_.----.-----|  |  |-----.----.
** |  __|  _  |     |   _|   _|  _  |  |  |  -__|   _|
** |____|_____|__|__|____|__| |_____|__|__|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "controller.h"

/***********************************************************************************************************/
LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
{
    // восстановление указателя на экземпляр класса из недр винды
    // при первом вызове здесь будет мусор, объект создается в wm_create
    controller * pctrl = getwindownlong<controller*>(hwnd);

    // обрабатываем мессаги
    switch (message) {

    // вызывается один раз при старте программы
    case WM_CREATE:
        try
        {
            pctrl = new controller(hwnd, (CREATESTRUCT*)lp);
            setwindownlong<controller*>(hwnd, pctrl);
        }
        catch (winexception e)
        {
            MessageBox(hwnd, e.getmessage(),  "Initialization", MB_ICONEXCLAMATION | MB_OK);
            return -1;
        }
        catch (...)
        {
            MessageBox(hwnd, "Unknown Error", "Initialization", MB_ICONEXCLAMATION | MB_OK);
            return -1;
        }
        return 0;

    /* ************************* **
    ** **  system processors  ** **
    ** ************************* */
    // просьба закрыться (в нашем случае - еще смотря от кого)
    // the end 
    // вызывается постоянно при перерисовке
    // приходит от дочернего окна
    // хоткей
    // обработка команд (от главного меню в том числе)
    case WM_CLOSE:        return pctrl->close();
    case WM_DESTROY:      setwindownlong<controller*>(hwnd, 0); delete pctrl; return 0;
    case WM_PAINT:        pctrl->paint(); return 0;
    case WM_PARENTNOTIFY: pctrl->parentnotify(LOWORD(wp), HIWORD(wp), (HWND)lp); return 0;
    case WM_HOTKEY:       pctrl->hotkey((int)wp, (uint)lp); return 0;
    case WM_COMMAND:      pctrl->command(LOWORD(wp), HIWORD(wp), (HWND)lp); return TRUE;

    /* ************************* **
    ** **  custom processors  ** **
    ** ************************* */
    // нужно показать попупменю
    // пришел цвет!
    // кто-то просит показать балон
    // вызов апдейта
    case MSG_POPUPMENU:   pctrl->popupmenu(); break;
    case MSG_COLOR:       pctrl->itiscolor((COLORREF)lp); return 0;
    case MSG_BALLOON:     pctrl->balloon((UINT)lp); return 0;
    case MSG_UPDATE:      pctrl->update((HWND)lp); return 0;

    }
    return DefWindowProc(hwnd, message, wp, lp);
}

/* получает хэндл и и структуру CREATESTRUCT в этой структуре лежит *****************************************
** всякое фуфло: типа ширины, высоты окна, названия класса окна, заголовок и т.п. 
** инициализирует внтренние объекты, создает, заполнет начальной информацией, 
** отображает, устанавливает начальные значения ************************************************************/
controller::controller(HWND hwnd, CREATESTRUCT * pcrt)
:   _hwnd           (hwnd),                 // ложим хэндл в хэндл (кладем))
    _hinst          (pcrt->hInstance),      // 0x400...
    _base           (_hinst, _hwnd),
    _eyedropper     (_hinst, _hwnd, &_base),
    _glass          (_hinst, _hwnd, &_base, &_eyedropper),
    _options        (NULL),                 // flag/handler 
    _systrayicon    (FALSE),                // flag: tray icon
    _balloonflag    (FALSE),                // flag: balloon shown
    _message        (_hwnd),                // threded delay message
    _updater        (_hwnd, vsfilestr)      // init with current version number (from resources)
{
    /* пытаемся впендюрить иконку пока не получится 
    ** потому как эксплорер может запуститься позже при старте винды */
    while (!settray(true)) Sleep(500);

    /* делаем это как можно позже, чтобы системная конфигурация "устоялась" */
    _eyedropper.monitorsinitialize();

    /* если надо показать балон - показываем через 2 сек */
    if (_base.showballoon()) balloon(2000); 
    _base.showballoon(false); // off

    /* регаем хоткей */
    registerhotkey(*_base.hotkeymods(), *_base.hotkeyvk());
}

/************************************************************************************************************
**                    __                  
** .-----.--.--.-----|  |_.-----.--------.
** |__ --|  |  |__ --|   _|  -__|        |
** |_____|___  |_____|____|_____|__|__|__| processors
**       |_____|                           
************************************************************************************************************/
/* обработчик WM_CLOSE */
bool controller::close()
{
    DestroyWindow(_hwnd);       // и посылает wm_destroy
    return 0;                   // если взялись обрабатывать wm_close - то нужно возвращать 0
}

/* деструктор */
controller::~controller()
{
    settray(false);             // убираем иконку
    PostQuitMessage(0);         // постит wm_quit который уже никогда не ловиться winproc-ом
}

/* типа перерисовка шоб глюков небыло, а то бывает например 
** мэссэджбоксы вылазят под окном, иными словами - всегда 
** необходимо обрабатывать это событие */
void controller::paint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(_hwnd, &ps);
    EndPaint(_hwnd, &ps);
}

/* сообщения от дочернего окна
** на самом деле IDD_DIALOG диалог не шлет это сообщение, так как сделан из ресурсов
** а для ресурсных окон автоматически устанавливается стиль WS_EX_NOPARENTNOTIFY
** но мы в конструкторе диалога отправляем это сообщение сделанное по всем правилам,
** поэтому этот обработчик универсальный, и его можно использовать и для настоящих сообщений :) */
void controller::parentnotify(uint event, uint id, handler hwnd)
{
    // сохраняем хэндл окна диалога, при закрытии окна мы его потом обнуляем
    // поэтому ненулевой хендл свидетельствует что окно настроек открыто
    if ((event == WM_CREATE) && (id == IDD_DIALOG)) _options = hwnd;
}

/* hotkey comes */
void controller::hotkey(int id, uint mod)
{
    if (id = ID_HOTKEY) // тот что мы зарегали
    {
        if (!_eyedropper.gethandle().visible()) { _glass.sendmessage(MSG_PROCEED, PICK_START, 0); }
        else                                    { _glass.sendmessage(MSG_PROCEED, PICK_STOP,  0); }
    }
}

/* обработка команд от главного меню */
bool controller::command(uint locmd, uint hicmd, handler hwnd)
{
    /* сообщение от меню */
    if (hicmd == 0)
    {
        // устанавливаем формат в зависимости от команды
        for (auto f : *_base.getformats()) { if (f.format == locmd) _base.colorformat(f.format); }

        // обрабатываем остальные команды
        switch (locmd) {
        /*************************************************/
        case ID_MODE_DRAG:      _base.clickmode(MODE_DRAG);  break;
        case ID_MODE_CLICK:     _base.clickmode(MODE_CLICK); break;
        /*************************************************/
        case ID_FILE_DONATE:    ShellExecute(_hwnd, "open", LINK_DONATE, 0, 0, SW_SHOWDEFAULT); break;
        case ID_FILE_ABOUT:     options(_hwnd, 1); break;
        case ID_FILE_OPTIONS:   options(_hwnd, 0); break;
        /*************************************************/
        case ID_FILE_EXIT:      SendMessage(_hwnd, WM_CLOSE, 0, 1); break; 
        }

        // обновить параметры в окне options
        if (_options) _options.sendmessage(WM_COMMAND, MAKEWPARAM(ID_CONTROLLER, 0), 0);
        return 0;
    }
    return false;
}

/************************************************************************************************************
**                   __  
** .----.--.--.-----|  |_.-----.--------.
** |  __|  |  |__ --|   _|  _  |        |
** |____|_____|_____|____|_____|__|__|__| processors
**
************************************************************************************************************/
/* создаем и показываем попуп меню */
void controller::popupmenu()
{
    HMENU submenu = CreatePopupMenu();
    HMENU hmenu   = CreatePopupMenu();

    // добавляем и чекаем соответсвтующий пункт формата
    for (auto f : *_base.getformats())
    {
        if (f.format == *_base.colorformat())
            AppendMenu(hmenu, MF_STRING | MF_CHECKED, f.format, f.caption.c_str());
        else
            AppendMenu(hmenu, MF_STRING | 0,          f.format, f.caption.c_str());
    }
    
    // чекаем соответсвтующий пункт режима
    UINT style_d = 0; if (*_base.clickmode() == MODE_DRAG)  style_d = MF_CHECKED;
    UINT style_c = 0; if (*_base.clickmode() == MODE_CLICK) style_c = MF_CHECKED;

    // добавляем остальное
    AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hmenu, MF_STRING | style_d, ID_MODE_DRAG,      "&Drag mode");
    AppendMenu(hmenu, MF_STRING | style_c, ID_MODE_CLICK,     "&Click mode");
    AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hmenu, MF_STRING,           ID_FILE_DONATE,    "Do&nate");
    AppendMenu(hmenu, MF_STRING,           ID_FILE_ABOUT,     "&About");
    AppendMenu(hmenu, MF_STRING,           ID_FILE_OPTIONS,   "&Options...");
    AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hmenu, MF_STRING,           ID_FILE_EXIT,      "E&xit");

    POINT pt; GetCursorPos(&pt);

    TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, _hwnd, NULL);
    PostMessage(_hwnd, WM_NULL, 0, 0); 
    //DestroyMenu(hmenu);
}

/* прислали цвет! засунем в клипбоард цвет в соответствии с выбранным форматом */
void controller::itiscolor(COLORREF iscolor)
{
    string str = ""; color clr(iscolor, _base.getformats());

    switch (*_base.colorformat()) {
    case ID_FORMAT_HTML:     str = clr.html(); break;
    case ID_FORMAT_HEX:      str = clr.hex(); break;
    case ID_FORMAT_DELPHI:   str = clr.delphi(); break;
    case ID_FORMAT_VB:       str = clr.vb(); break;
    case ID_FORMAT_RGB:      str = clr.rgb(); break;
    case ID_FORMAT_RGBFLOAT: str = clr.rgbfloat(true, *_base.floatprecision()); break;
    case ID_FORMAT_HSV:      str = clr.hsv(); break;
    case ID_FORMAT_HSL:      str = clr.hsl(); break;
    case ID_FORMAT_LONG:     str = clr.longvalue(); break; }

    if (_base.clipboard()) CopyTextToClipboard(str.c_str());
}

/* received message to show balloon */
void controller::balloon(uint delay)
{
    // говорим треду прислать позже
    if (delay) { _message.send(MSG_BALLOON, 0, 0, delay); return; }

    // сообщение в зависимости от настроек
    LPCTSTR msg;

    if (*_base.clickmode() == MODE_DRAG) { msg = "Press and hold the mouse button to proceed"; }
    else                                 { msg = "Click on the icon to proceed"; }

    // только если иконка уже есть и балон не выводился и надо щас без промедления
    if (_systrayicon && !_balloonflag && !delay) 
    {
                 _nid.uFlags       |= NIF_INFO;
                 _nid.dwInfoFlags   = NIIF_INFO;
                 _nid.uTimeout      = 5000;
        lstrcpyn(_nid.szInfo,         msg, sizeof(_nid.szInfo));
        lstrcpyn(_nid.szInfoTitle,   "Instant Eyedropper", sizeof(_nid.szInfoTitle));

        _balloonflag = Shell_NotifyIcon(NIM_MODIFY, &_nid);
    }
}

/* register hotkey */
void controller::registerhotkey(uint mods, uint vk)
{
    UnregisterHotKey(_hwnd, ID_HOTKEY);
    BOOL registered = RegisterHotKey(_hwnd, ID_HOTKEY, mods, vk);
    if (vk && !registered) { _base.hotkeymods(0); _base.hotkeyvk(0); }
}

/* update */
void controller::update(handler buttonhwnd) 
{
    _updater.start(buttonhwnd);
}

/************************************************************************************************************
**  __       __                          __ 
** |__|----.|  |_.-----.----.-----.-----|  |
** |  |     |   _|  -__|   _|     |  _  |  |
** |__|__|__|____|_____|__| |__|__|___._|__| processors
**
************************************************************************************************************/
/* установить/убрать иконку в систрэй */
bool controller::settray(bool set) 
{
    // запоняет блок нулями
    ZeroMemory(&_nid, sizeof(_nid));

    // заполняем структуру дэфолтными значениями
             _nid.cbSize = NOTIFYICONDATA_V2_SIZE;   // 
             _nid.hWnd   = _glass.hwnd();            // мессаги будет получать _glass
             _nid.uID    = 0;                        // 
             _nid.uFlags = NIF_ICON | NIF_TIP;       // 
    lstrcpyn(_nid.szTip, "", sizeof(_nid.szTip)/sizeof(_nid.szTip[0]));

    // если нужно установить иконку
    if (set)
    {
        // теперь пашет даже на high-dpi
        _nid.hIcon           = (HICON)LoadImage(_hinst,
                                                MAKEINTRESOURCE(IDI_ICON1), 
                                                IMAGE_ICON,
                                                GetSystemMetrics(SM_CXSMICON),
                                                GetSystemMetrics(SM_CXSMICON),
                                                LR_SHARED);
        _nid.uFlags          |= NIF_MESSAGE;
        _nid.uCallbackMessage = WM_MYICONNOTIFY;

        _systrayicon = Shell_NotifyIcon(NIM_ADD, &_nid);
    }
    else
    {
        Shell_NotifyIcon(NIM_DELETE, &_nid);
    }

    if (_systrayicon) return true;
    else              return false;
}

/* вызывают окно настроек */
void controller::options(HWND hwnd, int tabindex)
{
    /* если окно настроек уже открыто */
    if (_options) { _options.setfocus(); }

    /* если еще не открыто */
    else
    {
        /* создает фабрику контроллеров по шаблону 
        ** и передает туда объект optionsdata в качесте параметра */
        optionsdata data(_hinst, tabindex, &_base);
        controllerfactory<optionscontroller, optionsdata> factory(&data);

        /* го */
        modaldialogbox box(_hinst, hwnd, IDD_DIALOG, &factory);

        /* переходит на эту стадию когда окно закончилось
        ** результат вызова EndDialog */
        if (box.isok())
        {
            /* меняем реестр только если значение поменялось на противоположное */
            if     (!*_base.autoloadstate() &&  data.autoload()) _base.autoloadstate(true);
            else if (*_base.autoloadstate() && !data.autoload()) _base.autoloadstate(false);

            /* вызываем регистрацию хоткея раньше чем сохраним значение в базе 
            ** для того чтобы проверить и анрегистррнуть старый хоткей если надо */
            registerhotkey(data.hotkeymods(), data.hotkeyvk());

            /* сохраняем новые данные */
            _base.theme(data.theme());
            _base.clipboard(data.clipboard());
            _base.clickmode(data.clickmode());
            _base.zoom(data.zoom());
            _base.colorformat(data.colorformat());
            _base.floatprecision(data.floatprecision());
            _base.hotkeymods(data.hotkeymods());
            _base.hotkeyvk(data.hotkeyvk());

            _base.save();
        }
        _options = NULL;
    }
}

// копирует строку в клипбоард
BOOL controller::CopyTextToClipboard(LPCSTR lpszText)
{
    if (!OpenClipboard(0)) return FALSE;
    if (!EmptyClipboard()) return FALSE;

    long strLen = strlen(lpszText);
    HGLOBAL hnd = GlobalAlloc(GMEM_FIXED, strLen + 1); 
    if (hnd == NULL) 
    { 
        CloseClipboard(); 
        return FALSE; 
    }
    LPSTR lptstrCopy = (LPSTR)GlobalLock(hnd); 
    
    //strcpy(lptstrCopy, lpszText);
    memcpy(lptstrCopy, lpszText, strLen + 1);

    GlobalUnlock(hnd); 
    SetClipboardData(CF_TEXT, lptstrCopy);

    long wlen = MultiByteToWideChar(CP_ACP, 0, lptstrCopy, -1, 0, 0);

    HGLOBAL hnd2 = GlobalAlloc(GMEM_FIXED, wlen * 2); 
    LPWSTR wbuf = (LPWSTR)GlobalLock(hnd2); 
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lptstrCopy, -1, wbuf, wlen);
    SetClipboardData(CF_UNICODETEXT, wbuf);
    GlobalUnlock(hnd2);

    HGLOBAL hnd3 = GlobalAlloc(GMEM_FIXED, strLen + 1); 
    LPSTR lptstrCopy3 = (LPSTR)GlobalLock(hnd3); 
    CharToOem(lptstrCopy, lptstrCopy3);
    SetClipboardData(CF_OEMTEXT, lptstrCopy3);
    GlobalUnlock(hnd3);

    CloseClipboard(); 
    return TRUE;
}
/***********************************************************************************************************/

