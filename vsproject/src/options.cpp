/************************************************************************************************************ 
** options + about
**              __   __                    
** .-----.-----|  |_|__|-----.-----.-----.
** |  _  |  _  |   _|  |  _  |     |__ --|
** |_____|   __|____|__|_____|__|__|_____| child controller
**       |__|                                                                         instant-eyedropper.com
************************************************************************************************************/

#include "options.h"

/***********************************************************************************************************/
void optionscontroller::initialization()
{
    /* маякнем родителю, передадим хэндл, он установит флаг */
    _hwndparent.sendmessage(WM_PARENTNOTIFY, MAKEWPARAM(WM_CREATE, IDD_DIALOG), (LPARAM)(HWND)_hwnd);

    /* установим окно посередине и правильного размера */
    RECT rc; _hwnd.getwindowrect(rc);
    int winw = rc.right  - rc.left;
    int winh = rc.bottom - rc.top;

    int x_pos = (GetSystemMetrics(SM_CXSCREEN) - winw)/2;
    int y_pos = (GetSystemMetrics(SM_CYSCREEN) - winh)/2;
    _hwnd.move(x_pos, y_pos, winw, winh);

    /* делаем табы и открываем нужный */
    _tab.add("Options"); _tab.add("About");

    _tabindex = _data->tabindex();              // сохраняем значение переданного
    _tab.hwnd().tabselect(_tabindex);           // включаем апишный таб
    switchtab(_tabindex);                       // выводим свое как надо

    /**********************************************************************************************/
    /* general: чекаем соответствующие контролы */
    _cbox_startup.check(*_data->base->autoloadstate()); // autoload
    _cbox_dktheme.check(*_data->base->theme());         // dark theme
    _cbox_cptocpb.check(*_data->base->clipboard());     // copy to clipboard

    /**********************************************************************************************/
    /* mode: */
    if (*_data->base->clickmode() == MODE_DRAG)  _radio_drag.check(true);
    if (*_data->base->clickmode() == MODE_CLICK) _radio_click.check(true);

    /**********************************************************************************************/
    /* resolution: */
    _combo_zoom.add({"3x3", "5x5", "7x7", "9x9"});
    _combo_zoom.select(*_data->base->zoom());

    /**********************************************************************************************/
    /* colorformat: добавляем все цветовые режимы и выберем нужный */
    vector<colorformats> * formats = _data->base->getformats();
    int index = 0; for (auto f : *formats) 
    {
        _combo_colorformat.add(f.caption.c_str());
        if (f.format == *_data->base->colorformat()) _combo_colorformat.select(index);
        index++;
    }

    /* выравнивание калечного позиционирования 
    ** для "Uppercase" и "Precision" (относительно комбобокса) */
    RECT rcombo; _combo_colorformat.hwnd().getwindowrect(rcombo);
    RECT rlabel;  _static_precision.hwnd().getwindowrect(rlabel);
    int diff = (rcombo.bottom - rcombo.top) - (rlabel.bottom - rlabel.top);

    POINT pt; pt.x = rlabel.left; pt.y = rcombo.top;
    ScreenToClient(_hwnd, &pt);

    _static_precision.hwnd().move(pt.x, pt.y + diff/2, rlabel.right  - rlabel.left, 
                                                       rlabel.bottom - rlabel.top);

    /* float precision */
    _combo_precision.add({"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"});
    _combo_precision.select(*_data->base->floatprecision() - 1);

    /* templates */
    _cbox_template.init(_data->base->getformats());
    _cbox_template.setup(*_data->base->colorformat());

    /**********************************************************************************************/
    /* hotkey: set up */
    _modcombos.setup(*_data->base->hotkeymods());
    _vkeycombo.setup(*_data->base->hotkeyvk());

    /**********************************************************************************************/
    /* подготовим камуфляжные кисти для контролов и апдейтим состояние темы */
    _tabbrush.make(_tab.hwnd());
    _bmpbrush.make(_about.hwnd());
    _th.update();

    /* впендюрим номер версии */
    _static_vers.settext(vsfilestr);
}

/***********************************************************************************************************/
BOOL optionscontroller::paint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(_hwnd, &ps);
    EndPaint(_hwnd, &ps);

    // Typically, the dialog box procedure should return TRUE if it processed the message, 
    // and FALSE if it did not. If the dialog box procedure returns FALSE, 
    // the dialog manager performs the default dialog operation in response to the message.
    return TRUE; 
}

/***********************************************************************************************************/
/* приходит тогда, кода нужно перерисовать овнердравы */
BOOL optionscontroller::drawitem(LPDRAWITEMSTRUCT pis)
{
    if (pis->hwndItem == _about.hwnd())
    {
        RECT rc; _about.hwnd().getwindowrect(rc);

        int w = rc.right  - rc.left;
        int h = rc.bottom - rc.top;

        switch (pis->itemAction)
        {
        case ODA_DRAWENTIRE:

            HDC hdccompatible = CreateCompatibleDC(pis->hDC);

            // draw picture
            HBITMAP hbmp = (HBITMAP)LoadImage(_data->hinst(), resource(IDB_ABOUT), IMAGE_BITMAP, w, h, LR_SHARED);
            HBITMAP oldbmp = (HBITMAP)SelectObject(hdccompatible, hbmp);

            // мутки с альфаканалом
            TransparentBlt(pis->hDC, 0, 0, w, h, hdccompatible, 0, 0, w, h, RGB(0,255,0));

            SelectObject(hdccompatible, oldbmp);
            DeleteDC(hdccompatible);

            DeleteObject(oldbmp);
            DeleteObject(hbmp);
        }
    }
    return TRUE;
}

/***********************************************************************************************************/
/* themechanged */
BOOL optionscontroller::themechanged()
{
    // апдейтим кисть от табконтрола и состояние флага темы
    // на случай если ситуация изменилась
    _tabbrush.make(_tab.hwnd()); 
    _th.update();
    return TRUE;
}

/***********************************************************************************************************/
/* перерисовка контролов. hwnd текущего контрола */
LRESULT optionscontroller::ctlcolorstatic(HDC hdc, HWND hwnd)
{
    // этим контролам как раз катит прозрачная кисточка (которая не пашет для других контролов)
    if (std::find(_aboutstatics.begin(), _aboutstatics.end(), hwnd) != _aboutstatics.end())
    {
        ::SetTextColor(hdc, RGB(200, 200, 190));                                    // сначала для всех 
        if (hwnd == _static_vers.hwnd()) ::SetTextColor(hdc, RGB(242, 242, 235));   // version number
        if (hwnd == _static_iurl.hwnd()) ::SetTextColor(hdc, RGB(117, 214, 255));   // url

        ::SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)(::GetStockObject(HOLLOW_BRUSH));
    }

    // если включена xp(и не только) тема или винда 8 и старше 
    if (_th.yep() || win8plus()) { _tabbrush.adjust(hdc, hwnd); return (LRESULT)_tabbrush.brush(); }
    // для всех остальных
    else { ::SetBkMode(hdc, TRANSPARENT); return (LRESULT)(::GetSysColorBrush(COLOR_3DFACE)); }
}

/***********************************************************************************************************/
/* делаем кисточку для кнопки. hwnd текущего контрола */
LRESULT optionscontroller::ctlcolorbutton(HDC hdc, HWND hwnd)
{
    // кнопка апдейт
    if (_updbutton.hwnd() == hwnd) { _bmpbrush.adjust(hdc, hwnd); return (LRESULT)_bmpbrush.brush(); }
    // для всех остальных
    else { ::SetBkMode(hdc, TRANSPARENT); return (LRESULT)(::GetStockObject(HOLLOW_BRUSH)); }
}

/***********************************************************************************************************/
bool optionscontroller::mousemove(int x, int y)
{
    /* если мыша наехала на определенную область */
    POINT pt; GetCursorPos(&pt);
    if ((_tabindex == 1) && pointinrect(_static_iurl.hwnd(), pt)) { SetCursor(LoadCursor(0, IDC_HAND)); }
    return true;
}
/***********************************************************************************************************/
bool optionscontroller::lbuttondn(int x, int y)
{
    /* если кликнули на определенную область */
    POINT pt; GetCursorPos(&pt);
    if ((_tabindex == 1) && pointinrect(_static_iurl.hwnd(), pt)) { SetCursor(LoadCursor(0, IDC_HAND)); }
    return true;
}

/***********************************************************************************************************/
bool optionscontroller::lbuttonup(int x, int y)
{
    /* если мыша отжата на определенной области */
    POINT pt; GetCursorPos(&pt);
    if ((_tabindex == 1) && pointinrect(_static_iurl.hwnd(), pt))
    {
        SetCursor(LoadCursor(0, IDC_HAND));
        ShellExecute(_hwnd, "open", LINK_HOME, 0, 0, SW_SHOWDEFAULT);
    }
    return true;
}

/***********************************************************************************************************/
bool optionscontroller::oncommand(int ctrlid, int notifycode)
{
    switch (ctrlid)
    {
    // это кастомная мессага от котроллера 
    case ID_CONTROLLER:
        // выбираем нужный пункт
        if (*_data->base->clickmode() == MODE_DRAG)  { _radio_drag.check(true);  _radio_click.check(false); }
        if (*_data->base->clickmode() == MODE_CLICK) { _radio_drag.check(false); _radio_click.check(true);  }

        // выбираем нужный пункт
        _combo_colorformat.select(*_data->base->colorformat());

        // симулируем сообщение что комбобокс изменился, но по сути это так и есть
        _hwnd.sendmessage(WM_COMMAND, MAKEWPARAM(IDCB_FORMAT, CBN_SELCHANGE), 0);

        return true;

    case IDCB_FORMAT:
        // если выбрали другой пункт из комбобокса
        if (notifycode == CBN_SELCHANGE)
        {
            // устанавливаем переменную colorformat в соответсвующее значение 
            _data->_colorformat = _combo_colorformat.currentindex();

            // теперь, сначала, выключим все дополнительные контролы 
            // а потом энаблим если нужно
            for (auto cls : _controls_tab0_float) cls.disable();
            if (_data->colorformat() == ID_FORMAT_RGBFLOAT) for (auto cls : _controls_tab0_float) cls.enable();

            // заполняем темплейт комбо в соответствии с текущим форматом
            _cbox_template.setup(_data->colorformat());
        }
        return true;

    case IDCB_TEMPLATE:
        // изменилось содержание поля эдит в комбомоксе
        if (notifycode == CBN_EDITCHANGE) { _cbox_template.echange(_data->colorformat()); }

        // был выбран другой пункт из списка
        if (notifycode == CBN_SELCHANGE)  { _cbox_template.schange(_data->colorformat()); }
        return true;

    case IDB_UPDATE:
        // update button action
        _hwndparent.sendmessage(MSG_UPDATE, 0, (LPARAM)_updbutton.hwnd());
        return true;

    case IDOK:
        // general:
		_data->_autoload  = _cbox_startup.check();
        _data->_theme     = _cbox_dktheme.check();
        _data->_clipboard = _cbox_cptocpb.check();

        // mode:
        if (_radio_drag.check())  _data->_clickmode = MODE_DRAG;
        if (_radio_click.check()) _data->_clickmode = MODE_CLICK;

        // zoom:
        _data->_zoom = _combo_zoom.currentindex();

        // float precision:
        _data->_floatprecision = _combo_precision.currentindex() + 1;

        // hotkey:
        _data->_hotkeymods = _modcombos.mods();
        _data->_hotkeyvk   = _vkeycombo.getvkeycode();

        // сохраняет изменения уже на базе
        _cbox_template.storedata();

        // end ok
        _hwnd.enddialog(true);
        return true;

    case IDCANCEL:
        // end cancel
        _hwnd.enddialog(false);
        return true;
    }
    return false;
}

/***********************************************************************************************************/
bool optionscontroller::onnotify(int ctrlid, LPNMHDR lpnmhdr)
{
    switch (ctrlid)
    {
    case IDC_TAB:
        /* переключалка табов */
        if (lpnmhdr->code == TCN_SELCHANGE)
        {
            switch (TabCtrl_GetCurSel(lpnmhdr->hwndFrom))
            {
            case 0: switchtab(0); _tabindex = 0; break;
            case 1: switchtab(1); _tabindex = 1; break;
            case 2: switchtab(2); _tabindex = 2; break;
            }
        }
        return true;
    }
    return false;
}

/************************************************************************************************************
**              __             __         
** .-----.-----|__|--.--.-----|  |_.-----.
** |  _  |   __|  |  |  |  _  |   _|  -__|
** |   __|__|  |__|\___/|___._|____|_____| :
** |__|                                     
************************************************************************************************************/
void optionscontroller::switchtab(int tab)
{
    if (tab == 0)
    {
        for (auto cls : _controls_tab1)          cls.hide();
        for (auto cls : _controls_tab0)          cls.show();
        for (auto cls : _controls_tab0_float)  { cls.show(); cls.disable(); }

        if (_data->colorformat() == ID_FORMAT_RGBFLOAT) 
            {   for (auto cls : _controls_tab0_float) cls.enable();  }   // float

        _cbox_startup.setfocus();
    }
    else if (tab == 1)
    {
        for (auto cls : _controls_tab0)          cls.hide();
        for (auto cls : _controls_tab0_float)    cls.hide();
        for (auto cls : _controls_tab1)          cls.show();
    }
    else if (tab == 2)
    {
        for (auto cls : _controls_tab0)          cls.hide();
        for (auto cls : _controls_tab0_float)    cls.hide();
        for (auto cls : _controls_tab1)          cls.hide();
    }
}

/***********************************************************************************************************/
bool optionscontroller::pointinrect(handler hwnd, POINT pt)
{
    RECT rc; hwnd.getwindowrect(rc);
    if ( (pt.x>rc.left) && (pt.x<rc.right) && (pt.y>rc.top) && (pt.y<rc.bottom) ) return true;
    return false;
}