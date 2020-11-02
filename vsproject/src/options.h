#if !defined options_h
#define options_h
/************************************************************************************************************ 
** options + about
**              __   __                    
** .-----.-----|  |_|__|-----.-----.-----.
** |  _  |  _  |   _|  |  _  |     |__ --|
** |_____|   __|____|__|_____|__|__|_____|
**       |__|                                                                         instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>

// для TransparentBlt
#pragma comment(lib, "Msimg32.lib")

#include <vector>
#include <string>
using namespace std;

#include "resource.h"
#include "statics.h"
#include "base.h"
#include "lib/transbrush.h"
#include "lib/themed.h"
#include "lib/win/handler.h"
#include "lib/modaldialogbox/modaldialogbox.h"
#include "lib/modaldialogbox/controls.h"
#include "lib/modaldialogbox/modcombos.h"
#include "lib/modaldialogbox/hotcombo.h"
#include "lib/modaldialogbox/combotemplate.h"

/***********************************************************************************************************/
// data storage for the options dialogbox
class optionsdata
{
public:
    optionsdata(HINSTANCE hinst, int tabindex, base * baseptr)
    {
        _hinst                  = hinst;
        _tabindex               = tabindex;
         base                   = baseptr;

        /* сохраняем копии текущих данных (разыменовывем указатели) */
        _autoload               = *base->autoloadstate();
        _theme                  = *base->theme();
        _clipboard              = *base->clipboard();
        _clickmode              = *base->clickmode();
        _zoom                   = *base->zoom();
        _colorformat            = *base->colorformat();
        _floatprecision         = *base->floatprecision();
        _hotkeymods             = *base->hotkeymods();
        _hotkeyvk               = *base->hotkeyvk();
    }
    
    HINSTANCE hinst()           { return _hinst; }
    int tabindex()              { return _tabindex;}
	
    /* методы для возврата новых данных */
    bool    autoload()          { return _autoload; }
    bool    theme()             { return _theme; }
    bool    clipboard()         { return _clipboard; }
    int     clickmode()         { return _clickmode; }
    int     zoom()              { return _zoom; }
    int     colorformat()       { return _colorformat; }
    int     floatprecision()    { return _floatprecision; }
    uint    hotkeymods()        { return _hotkeymods; }
    uint    hotkeyvk()          { return _hotkeyvk; }

public:
    /* указатель на реальный бейс, доступен в контроллере диалогбокса */
    base *      base;

    /* base data */
    bool        _autoload;
    bool        _theme;
    bool        _clipboard;
    int         _zoom;
    int         _colorformat;
    int         _clickmode;
    int         _floatprecision;
    uint        _hotkeymods;
    uint        _hotkeyvk;

private:
    /* local */
    HINSTANCE   _hinst;
    int         _tabindex;
};

/***********************************************************************************************************/
class optionscontroller: public modaldialogboxcontroller
{
public:
    optionscontroller(HWND hwnddlg, void * data)
    :   modaldialogboxcontroller    (data),                         // загружаем указатель на данные в контроллер
        _hwnd                       (hwnddlg),                      // dialogbox handler
        _hwndparent                 (GetParent(_hwnd)),             // сразу вычисляем хэндл родителя
        _th                         (),                             // темы

        /* controls */
        _tab                        (hwnddlg, IDC_TAB),             // tabcontrol

        /* general: */
        _group_general              (hwnddlg, IDCG_GENERAL),        // general:
        _cbox_startup               (hwnddlg, IDCK_STARTUP),        // load on startup
        _cbox_dktheme               (hwnddlg, IDCK_THEME),          // theme
        _cbox_cptocpb               (hwnddlg, IDCK_CPTOCPB),        // copy to clipboard

        /* color format: */
        _group_colorformat          (hwnddlg, IDCG_FORMAT),         // color format:
        _combo_colorformat          (hwnddlg, IDCB_FORMAT),         // color format
        _static_precision           (hwnddlg, IDCT_PRCN),           // precision:
        _combo_precision            (hwnddlg, IDCB_PRCN),           // combo precision
        _static_template            (hwnddlg, IDCT_TEMPLATE),       // template:
        _cbox_template              (hwnddlg, IDCB_TEMPLATE),       // combo templates

        /* mode: */
        _group_clickmode            (hwnddlg, IDCG_MODE),           // mode:
        _radio_drag                 (hwnddlg, IDCR_DRAG),           // drag mode
        _radio_click                (hwnddlg, IDCR_CLICK),          // click mode

        /* zoom */
        _group_zoom                 (hwnddlg, IDCG_ZOOM),           // zoom:
        _combo_zoom                 (hwnddlg, IDCB_ZOOM),           // zoom

        /* hotkey */
        _group_hotkey               (hwnddlg, IDCG_HOTKEY),         // hotkey:
        _modcombos                  (hwnddlg, IDCB_MOD0, IDCB_MOD1, IDCB_MOD2), // mod0, mod1, mod2
        _vkeycombo                  (hwnddlg, IDCB_VKEY),           // vkey

        /* about */
        _about                      (hwnddlg, IDCO_ABOUT),          // картинка
        _static_vers                (hwnddlg, IDCT_VERSION),        // version
        _static_home                (hwnddlg, IDCT_HOME),           // Home:
        _static_copy                (hwnddlg, IDCT_COPYRIGHT),      // © copyright date
        _static_iurl                (hwnddlg, IDCT_IEYEDROPPERURL), // instant-eyedropper.com
        _static_mnky                (hwnddlg, IDCT_AUTHOR),         // my name ky
        _updbutton                  (hwnddlg, IDB_UPDATE),          // update button
        
        /* buttons */
        _okbutton                   (hwnddlg, IDOK),                // common button
        _cancelbutton               (hwnddlg, IDCANCEL)             // common button
    {
        _data = (optionsdata*)getarglist();

        // соберем хэндлы контролов для каждой вкладки в одном месте
        _controls_tab0.push_back(_group_general.hwnd());
        _controls_tab0.push_back(_cbox_startup.hwnd());
        _controls_tab0.push_back(_cbox_dktheme.hwnd());
        _controls_tab0.push_back(_cbox_cptocpb.hwnd());
        _controls_tab0.push_back(_group_clickmode.hwnd());
        _controls_tab0.push_back(_radio_drag.hwnd());
        _controls_tab0.push_back(_radio_click.hwnd());
        _controls_tab0.push_back(_group_zoom.hwnd());
        _controls_tab0.push_back(_combo_zoom.hwnd());
        _controls_tab0.push_back(_group_colorformat.hwnd());
        _controls_tab0.push_back(_combo_colorformat.hwnd());
        _controls_tab0.push_back(_static_template.hwnd());
        _controls_tab0.push_back(_cbox_template.hwnd());
        _controls_tab0.push_back(_group_hotkey.hwnd());
        _controls_tab0.push_back(_modcombos.handler0());
        _controls_tab0.push_back(_modcombos.handler1());
        _controls_tab0.push_back(_modcombos.handler2());
        _controls_tab0.push_back(_vkeycombo.hwnd());

        _controls_tab0_float.push_back(_static_precision.hwnd());
        _controls_tab0_float.push_back(_combo_precision.hwnd());

        _controls_tab1.push_back(_about.hwnd());
        _controls_tab1.push_back(_static_vers.hwnd());
        _controls_tab1.push_back(_static_home.hwnd());
        _controls_tab1.push_back(_static_copy.hwnd());
        _controls_tab1.push_back(_static_iurl.hwnd());
        _controls_tab1.push_back(_static_mnky.hwnd());
        _controls_tab1.push_back(_updbutton.hwnd());

        /* эти контролы нужно отрисовывать с ньюансом */
        _aboutstatics.push_back(_static_vers.hwnd());
        _aboutstatics.push_back(_static_home.hwnd());
        _aboutstatics.push_back(_static_copy.hwnd());
        _aboutstatics.push_back(_static_iurl.hwnd());
        _aboutstatics.push_back(_static_mnky.hwnd());
    }

   ~optionscontroller() { }
   
    void    initialization();                   // yep

    BOOL    paint();                            // wm_paint
    BOOL    drawitem(LPDRAWITEMSTRUCT);         // redraws ownerdraws
    BOOL    themechanged();                     // windows theme changed event

    bool    mousemove(int, int);
    bool    lbuttondn(int, int);
    bool    lbuttonup(int, int);

    LRESULT ctlcolorstatic(HDC, HWND);          // redraws controls
    LRESULT ctlcolorbutton(HDC, HWND);          // redraws buttons
    bool    oncommand(int, int);
    bool    onnotify(int, LPNMHDR);

private:
    void    switchtab(int);                     // tab manager
    bool    pointinrect(handler, POINT);        // geometry calculation

private:
    optionsdata *       _data;                  // данные
    handler             _hwnd;                  // хэндл этого диалогового окна
    handler             _hwndparent;            // хэндл родителя
    int                 _tabindex;

    themed              _th;                    // детектор тем и хранитель кисти
    transbrush          _tabbrush;              // камуфляжная кисть
    transbrush          _bmpbrush;              // камуфляжная кисть

    vector<handler>     _controls_tab0;         // массив хендлов контролов вкладки options
    vector<handler>     _controls_tab1;         // массив хендлов контролов вкладки about
    vector<handler>     _controls_tab0_float;   // подмассив подконтролов rgbfloat

    vector<handler>     _aboutstatics;          // статики эбаутконтрола (требуют своей кисти)

    tab                 _tab;                   // tabcontrol

    // general
    simplecontrol       _group_general;         // general:
    checkbox            _cbox_startup;          // load on startup
    checkbox            _cbox_dktheme;          // theme
    checkbox            _cbox_cptocpb;          // copy to clipboard

    // color format area
    simplecontrol       _group_colorformat;     // color format:
    combo               _combo_colorformat;     // color format

    // float customize
    staticc             _static_precision;      // precision:
    combo               _combo_precision;       // combo precision

    // template
    staticc             _static_template;       // spacer:
    combotemplate       _cbox_template;         // combo spacer
    
    // zoom
    simplecontrol       _group_zoom;            // zoom:
    combo               _combo_zoom;            // zoom

    // mode
    simplecontrol       _group_clickmode;       // mode:
    radio               _radio_drag;            // drag mode
    radio               _radio_click;           // click mode
    
    // hotkey
    simplecontrol       _group_hotkey;          // hotkey:
    modcombos           _modcombos;             // modifier 0, 1, 2
    hotcombo            _vkeycombo;             // virtualkey

    // about
    staticc             _about;                 // картинка
    staticc             _static_vers;           // version
    staticc             _static_home;           // Home:
    staticc             _static_copy;           // © copyright date
    staticc             _static_iurl;           // instant-eyedropper.com
    staticc             _static_mnky;           // my name ky
    button              _updbutton;

    // main buttons
    button              _okbutton;              // OK
    button              _cancelbutton;          // Cancel
};

/***********************************************************************************************************/
#endif
