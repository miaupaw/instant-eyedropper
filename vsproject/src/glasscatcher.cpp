/************************************************************************************************************ 
**        __                               __         __               
** .-----|  |-----.-----.-----.-----.-----|  |_.-----|  |--.-----.----.
** |  _  |  |  _  |__ --|__ --|  ___|  _  |   _|  ___|     |  -__|   _|
** |___  |__|___._|_____|_____|_____|___._|____|_____|__|__|_____|__|  
** |_____|                                                                            instant-eyedropper.com
************************************************************************************************************/

#include "glasscatcher.h"

/***********************************************************************************************************/
LRESULT CALLBACK glasscatcherwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
    // вытаскивает указатель из винды
    glasscontroller * pctrl = getwindownlong<glasscontroller*>(hwnd);

    switch (msg)
    {
    case WM_CREATE:
        pctrl = new glasscontroller(hwnd, (CREATESTRUCT*)lparam);
        setwindownlong<glasscontroller*>(hwnd, pctrl);
        return 0;

    case WM_DESTROY:      setwindownlong<glasscontroller*>(hwnd, 0); delete pctrl; return 0; // дестрой 
    case WM_ERASEBKGND:   return 0; // чтобы небыло мерцания
    case WM_PAINT:        pctrl->paint(); return 0; // вызывается постоянно при перерисовке
    case WM_SHOWWINDOW:   pctrl->showwindow((BOOL)wparam); return 0; // если окно было спрятано или появилось
    case WM_KEYDOWN:      pctrl->keydown(wparam); return 0; // нажата кнопка
    case WM_KEYUP:        pctrl->keyup(wparam); return 0; // нажата кнопка
    case WM_RBUTTONUP:    pctrl->rbuttonup(); return 0; // откликнута правая кнопка
    case WM_LBUTTONDOWN:  pctrl->lbuttondown(); return 0; // left button down
    case WM_LBUTTONUP:    pctrl->lbuttonup(); return 0; // left button up
    case WM_MOUSEMOVE:    pctrl->mousemove(); return 0; // mousemove
    case WM_MYICONNOTIFY: pctrl->iconnotify(lparam); return 0; // сообщение от иконки трея
    case WM_HOTKEY:       pctrl->hotkey((int)wparam, (UINT)lparam); return 0; // local hotkeys
    case MSG_PROCEED:     pctrl->proceed(wparam, lparam); return 0; // from controller global hotkeys
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

/***********************************************************************************************************/
/* сохраняет хэндлы окна и родителя внутри объекта 
** (инициализирует внутренние переменные реальными значениями) */
glasscontroller::glasscontroller(HWND hwnd, CREATESTRUCT * pcrt)
:   _glass          ((glasscatcher*)pcrt->lpCreateParams),
    _hwnd           (hwnd),                
    _hwndparent     (pcrt->hwndParent),
    _hinst          ((HINSTANCE)GetModuleHandle(NULL)),
    _mousemon       (_hwnd),
    _screen         (),
    _mousespeed     (0),
    _clickmode      (_glass->clickmode()),  // pointer to base variable
    _capturemode    (false),
    _esc            (NULL)                  // хоткей escape
{
    // get and store an global mouse speed
    BOOL res = SystemParametersInfo(SPI_GETMOUSESPEED, 0, &_mousespeed, 0);
    if (!res) _mousespeed = 0;
}

/* деструктор */
glasscontroller::~glasscontroller()
{
    // восстановим на всякий случай
    if (_mousespeed) SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)_mousespeed, 0);
}

/************************************************************************************************************
**                    __                  
** .-----.--.--.-----|  |_.-----.--------.
** |__ --|  |  |__ --|   _|  -__|        |
** |_____|___  |_____|____|_____|__|__|__| processors
**       |_____|                           
************************************************************************************************************/
/* перерисовка */
void glasscontroller::paint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(_hwnd, &ps);
        //HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        //FillRect(hdc, &ps.rcPaint, brush);   
     EndPaint(_hwnd, &ps);
}

void glasscontroller::showwindow(BOOL show)  {} /* если окно было спрятано или появилось */
void glasscontroller::keydown(WPARAM wparam) {} /* нажали кнопку */
void glasscontroller::keyup(WPARAM wparam)   {} /* отжали кнопку */

/* в зависимости от текущего режима, или сбрасывает айдроппание без сохраниения или вызывает менюшку */
void glasscontroller::rbuttonup()
{
    if (_capturemode) proceed(PICK_CANCEL, 0);  // сбросить
    else              popup();                  // вызвать попуп окошко
}

/* left button down */
void glasscontroller::lbuttondown()
{
    // начать айдроппить
    if (!_capturemode && *_clickmode == MODE_DRAG) proceed(PICK_START, *_clickmode);
}

/* оно живое! :) */
void glasscontroller::mousemove()
{
    // процессинг
    if (_capturemode)
    {
        // сообщаем айдропперу что мышка двигается
        // раньше сообщали, сейчас просто двигаем )
        _glass->ed->move();

        // обрабатываем precise mode
        if (GetAsyncKeyState(VK_CONTROL))
        {   if (_mousespeed) SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)1, 0); }
        else
        {   if (_mousespeed) SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)_mousespeed, 0); }
    }
    else
    {   
        // возвращаем окно в нормальный "неактивный" режим чтобы не стилить фокус
        _hwnd.addexstyle(WS_EX_NOACTIVATE);
    }
}

/* left button up */
void glasscontroller::lbuttonup()
{
    // закончить айдроппить
    if (_capturemode) proceed(PICK_STOP, 0);
}

/************************************************************************************************************
**                   __  
** .----.--.--.-----|  |_.-----.--------.
** |  __|  |  |__ --|   _|  _  |        |
** |____|_____|_____|____|_____|__|__|__| processors
**
************************************************************************************************************/
/* сообщение от систрэя */
void glasscontroller::iconnotify(LPARAM lparam)
{
    switch (lparam)
    {
    case WM_MOUSEMOVE:

        // подлазить
        if (*_clickmode == MODE_DRAG)
        {
            // взять текущие координаты мыши
            POINT pt;
            GetCursorPos(&pt);

            // сдвинуть окно под маус-поинтер
            SetWindowPos(_hwnd, HWND_TOPMOST, pt.x - 3, pt.y - 3, 7, 7, SWP_SHOWWINDOW | SWP_NOACTIVATE);

            // передать координаты окна маусмонитору
            _mousemon.etalon(pt.x, pt.y);
            if (!_mousemon.working()) _mousemon.go();
        }
        break;

    case WM_LBUTTONUP:

        // начать айдроппить
        if (*_clickmode == MODE_CLICK) proceed(PICK_START, *_clickmode);
        break;

    case WM_RBUTTONUP:

        // вызвать попуп окошко
        // теперь независимо от режима драг или клик, чтобы работало всегда
        if (!_capturemode) popup();
        break;

    // шлет в основном наш тред
    case WM_MOUSEOUT:

        // reset
        _hwnd.move(0, 0, 0, 0);
        _mousemon.suspend();
        break;
    }
}

/* обработка хоткея */
void glasscontroller::hotkey(int id, UINT mod)
{
    if (id = ID_ESCAPE) proceed(PICK_CANCEL, 0);
}

/************************************************************************************************************
** вызывается локально от iconnotify или lbuttonup или сразу по сообщению MSG_PROCEED от контроллера
**                                       __ 
** .-----.----.-----.----.-----.-----.--|  |
** |  _  |   _|  _  |  __|  -__|  -__|  _  |
** |   __|__| |_____|____|_____|_____|_____|
** |__|
************************************************************************************************************/
void glasscontroller::proceed(WPARAM event, LPARAM mode)
{
    // START
    if (event == PICK_START)
    {
        // зарегаем обработку ESC
        _esc = RegisterHotKey(_hwnd, ID_ESCAPE, 0, VK_ESCAPE);

        // инициализация
        RECT rc = {0, 0, 0, 0};                     // обязятельно инициализируем нулями - пригодицца
        _screen.capture();                          // снять экран

        // запуск немного отличается в зависимости от выбранного режима
        // и от того от кого происходит вызов метода, локально или по сообщению
        if (mode == 1) { _mousemon.suspend(); }  
        else           { _screen.rect(rc); _hwnd.addexstyle(WS_EX_NOACTIVATE); }
        
        // все остальные действия
        _hwnd.capturemouse();                       // захватить мышку
        _hwnd.move(rc.left, rc.top, (rc.right-rc.left), (rc.bottom-rc.top));
        _glass->ed->start(_screen.getdc());         // передаем HDC в eyedropper (provoke MSG_STARTEYEDROPPER)
        _capturemode = true;                        // устанавливаем флаг
    }

    // STOP
    if (event == PICK_STOP || event == PICK_CANCEL)
    {
        // анрегистер хоткея
        if (UnregisterHotKey(_hwnd, ID_ESCAPE)) _esc = NULL;

        // подготовка нужной команды
                             bool command = true;
        if (event == PICK_CANCEL) command = false;

        _screen.release();                          // очищаем объекты hdc
        _hwnd.addexstyle(WS_EX_NOACTIVATE);         // без этого при переключении режимов начинает мигать таскбар
        _hwnd.move(0, 0, 0, 0);                     // заныкать окно 
        _hwnd.releasemouse();                       // освободить мышку
        _glass->ed->stop(command);                  // управляем непосредственно
        _capturemode = false;                       // сбрасываем флаг

        if (_mousespeed) SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)_mousespeed, 0);
    }
}

/* вызов менюшки */
void glasscontroller::popup()
{
    _hwnd.removeexstyle(WS_EX_NOACTIVATE);          // удаляет этот стиль
    _hwnd.foreground();                             // и активирует окно чтобы попуп окошко могло нормально закрываться
    _hwndparent.sendmessage(MSG_POPUPMENU, 0, 0);   // вызываем попуп окошко
}

/***********************************************************************************************************/