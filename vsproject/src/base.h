#if !defined base_h
#define base_h
/************************************************************************************************************
** base is like model
**  __                      
** |  |--.-----.-----.-----.
** |  _  |  _  |__ --|  -__|
** |_____|___._|_____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "statics.h"                    // statics
#include "lib/color.h"                  // цвет
#include "lib/autoload.h"               // автозагрузка из реестра
#include "lib/ini/ini.h"                // работа с ини-файлами
#include "lib/registry/registry.h"      // реестр
#include "lib/win/handler.h"            // классный класс handler

/***********************************************************************************************************/
class base
{
public:
    base(HINSTANCE hinst, handler mainhwnd)
        : _hinst            (hinst),                // 0x400...
          _mainhwnd         (mainhwnd),             // хэндл главного окна
          _currentini       (INI_NAME),
          _registry         (HKEY_CURRENT_USER, "Software\\Instant Eyedropper\\"),
          
          // default properties
          _balloon          (true),                 // general: baloon
          _autoload         (false),                // general: autoload
          _theme            (0),                    // general: theme
          _clipboard        (true),                 // general: copy to clipboard
          _clickmode        (MODE_CLICK),           // clickmode:
          _zoom             (2),                    // zoom:

          _colorformat      (ID_FORMAT_HTML),       // colorformat: format
          _floatprecision   (11),                   // float precision
          _formats          (),                     // pointer to all color formats data
          _template_html    (formats[0].templates[0]),  
          _template_hex     (formats[1].templates[0]),
          _template_delphi  (formats[2].templates[0]),
          _template_vb      (formats[3].templates[0]),
          _template_rgb     (formats[4].templates[0]),
          _template_float   (formats[5].templates[0]),
          _template_hsv     (formats[6].templates[0]),
          _template_hsl     (formats[7].templates[0]),
          _template_long    (formats[8].templates[0]),

          _hotkeymods       (1),                    // hotkey: modifiers (alt)
          _hotkeyvk         (192),                  // hotkey: virtualkey (~)

          _fontface         (""),                   // font: face
          _fontweight       (false),                // font: weight
          _fontsize         (28),                   // font: size
          _fontcolor        ("#000000")             // font: color
    {
        /* читает все данные из ини или реестра */
        read();

        /* проверяет наличие автозагрузочного ключа, еcли да - устнавливает state в true */
        _autoload.check(); 

        /* попытаемся установить шрифт из ини файла
        ** если не удалось, то назначим первый существующий шрифт из списка */
        if (fontface(_fontface) == false) { for (auto font : fontlist) if (fontface(font)) break; }

        /* если шрифт курьер - сделаем его жирным */
        if (_fontface.compare("Courier New") == 0) fontweight(true);
        else                                       fontweight(false);

        /* сохраняем капитально */
        save(); 
    }

/***********************************************************************************************************/
    HINSTANCE    gethinst()             { return _hinst; }
    HWND         getmainhandle()        { return _mainhwnd; }

    /* показывать или нет балон при запуске */
    const bool   showballoon()          { return _balloon; }
    void         showballoon(bool is)   { _balloon = is; save(); }

    /* флаги насчет автозагрузки при стартапе */
    const bool * autoloadstate()        { return _autoload.state(); }
    void         autoloadstate(bool is) { _autoload.set(is); }

    /* темы */
    const bool * theme()                { return &_theme; }
    void         theme(bool is)         { _theme = is; if (is) fontcolor("#D9DED0"); else fontcolor("#000000"); }

    /* копировать или нет в клипбоард? */
    const bool * clipboard()            { return &_clipboard; }
    void         clipboard(bool is)     { _clipboard = is; }

    /* click mode */
    const int *  clickmode() const      { return &_clickmode; }
    void         clickmode(int mode)    { _clickmode = mode; save(); }

    /* разрешение окошка */
    const int *  zoom()                 { return &_zoom; }
    void         zoom(int f)            { _zoom = f; }

    /* HTML, RGB... */
    const int *  colorformat() const    { return &_colorformat; }
    void         colorformat(int f)     { _colorformat = f; save(); }

    /* float precision */
    const int *  floatprecision()       { return &_floatprecision; }
    void         floatprecision(int v)  { _floatprecision = v; }

    /* return pointer to templates */
    vector<colorformats> * getformats() { return &_formats; }

    /* hotkey modifiers */
    const uint *  hotkeymods()          { return &_hotkeymods; }
    void          hotkeymods(uint is)   { _hotkeymods = is; }

    /* hotkeyvirtualkey */
    const uint *  hotkeyvk()            { return &_hotkeyvk; }
    void          hotkeyvk(uint is)     { _hotkeyvk = is; }

    /* жир */
    bool          fontweight()          { return _fontweight; }
    void          fontweight(bool is)   { _fontweight = is; }

    /* размер */
    int           fontsize()            { return _fontsize; }
    void          fontsize(int f)       { _fontsize = f; }

    /* цвет */
    COLORREF      fontcolor()           { color clr; return clr.html(_fontcolor); }
    void          fontcolor(string c)   { _fontcolor = c; }

    /* имя */
    string        fontface()            { return _fontface; }

/***********************************************************************************************************/
    /* установить желаемый шрифт */
    bool fontface(string face)
    {
        // если строка пустая
        if (face.empty()) return false; 

        // проверить доступность данного шрифта
        handler hwnd = GetDesktopWindow();
        LOGFONT lf;
        HDC     hdc = hwnd.getdc();
        
            strcpy_s(lf.lfFaceName, face.c_str()); 
            lf.lfCharSet = DEFAULT_CHARSET;
            int result = EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)enumfontfamexproc, (LPARAM)face.c_str(), (DWORD)0);
	    
        ReleaseDC(hwnd, hdc);

        // если такой шрифт есть в системе - сохраняем
        if (result == 0) { _fontface = face; return true; }
        else             { return false; }
    }

    /* колбэк функция для вычисления списка шрифтов */
    static int CALLBACK enumfontfamexproc(ENUMLOGFONTEX *   lpelfe,     // logical attributes of the font
                                          NEWTEXTMETRICEX * lpntme,     // physical attributes of a font
                                          DWORD             FontType,   // The type of the fon
                                          LPARAM            lParam)     // The application-defined data passed
    {
        // сравним название шрифтов: который нужен и тот который найден
        // если такой есть - возвращем ноль, что одновременно прекращает дальнейшие вызовы колбэк функции
        //if (_tcscmp(lpelfe->elfLogFont.lfFaceName, (LPTSTR)lParam) == 0) return 0;
        //return true;

        return 0; // если функция была вызвана системой то по идее шрифт был найден
    }

/***********************************************************************************************************/
    void read()
    {
        /* проверить доступность записи в ini */
        if (_currentini.access())
        {
            /* чтение настроек из ини файла */
            _balloon            = _currentini.readvalue("common", "balloon",        _balloon);
            _theme              = _currentini.readvalue("common", "theme",          _theme);
            _clipboard          = _currentini.readvalue("common", "clipboard",      _clipboard);
            _zoom               = _currentini.readvalue("common", "zoom",           _zoom);
            _clickmode          = _currentini.readvalue("common", "clickmode",      _clickmode);
            _colorformat        = _currentini.readvalue("common", "colorformat",    _colorformat);
            _floatprecision     = _currentini.readvalue("common", "floatprecision", _floatprecision);
            _template_html      = _currentini.readvalue("common", "tmplt_html",     _template_html);
            _template_hex       = _currentini.readvalue("common", "tmplt_hex",      _template_hex);
            _template_delphi    = _currentini.readvalue("common", "tmplt_delphi",   _template_delphi);
            _template_vb        = _currentini.readvalue("common", "tmplt_vb",       _template_vb);
            _template_rgb       = _currentini.readvalue("common", "tmplt_rgb",      _template_rgb);
            _template_float     = _currentini.readvalue("common", "tmplt_float",    _template_float);
            _template_hsv       = _currentini.readvalue("common", "tmplt_hsv",      _template_hsv);
            _template_hsl       = _currentini.readvalue("common", "tmplt_hsl",      _template_hsl);
            _template_long      = _currentini.readvalue("common", "tmplt_long",     _template_long);
            _hotkeymods         = _currentini.readvalue("common", "hotkeymods",     _hotkeymods);
            _hotkeyvk           = _currentini.readvalue("common", "hotkeyvk",       _hotkeyvk);
            _fontface           = _currentini.readvalue("common", "fontface",       _fontface);
            _fontweight         = _currentini.readvalue("common", "fontweight",     _fontweight);
            _fontsize           = _currentini.readvalue("common", "fontsize",       _fontsize);
            _fontcolor          = _currentini.readvalue("common", "fontcolor",      _fontcolor);
        }
        /* иначе будем работать с реестром */
        else
        {
            _balloon            = _registry.queryvalue("balloon",                   _balloon);
            _theme              = _registry.queryvalue("theme",                     _theme);
            _clipboard          = _registry.queryvalue("clipboard",                 _clipboard);
            _zoom               = _registry.queryvalue("zoom",                      _zoom);
            _colorformat        = _registry.queryvalue("colorformat",               _colorformat);
            _clickmode          = _registry.queryvalue("clickmode",                 _clickmode);
            _floatprecision     = _registry.queryvalue("floatprecision",            _floatprecision);
            _template_html      = _registry.queryvalue("tmplt_html",                _template_html);
            _template_hex       = _registry.queryvalue("tmplt_hex",                 _template_hex);
            _template_delphi    = _registry.queryvalue("tmplt_delphi",              _template_delphi);
            _template_vb        = _registry.queryvalue("tmplt_vb",                  _template_vb);
            _template_rgb       = _registry.queryvalue("tmplt_rgb",                 _template_rgb);
            _template_float     = _registry.queryvalue("tmplt_float",               _template_float);
            _template_hsv       = _registry.queryvalue("tmplt_hsv",                 _template_hsv);
            _template_hsl       = _registry.queryvalue("tmplt_hsl",                 _template_hsl);
            _template_long      = _registry.queryvalue("tmplt_long",                _template_long);
            _hotkeymods         = _registry.queryvalue("hotkeymods",                _hotkeymods);
            _hotkeyvk           = _registry.queryvalue("hotkeyvk",                  _hotkeyvk);
            _fontface           = _registry.queryvalue("fontface",                  _fontface);
            _fontweight         = _registry.queryvalue("fontweight",                _fontweight);
            _fontsize           = _registry.queryvalue("fontsize",                  _fontsize);
            _fontcolor          = _registry.queryvalue("fontcolor",                 _fontcolor);
        }

        /* для совместимости со старым ini, так как поменялись номера дефайнов */
        if (_colorformat > 8) _colorformat = 0;

        /* соберем все в кучу: 
        /* динамический вектор структур "colortemplates" с модифицированными данными (dynamic + default) */
        _formats.push_back(formats[0]); _formats[0].templates.insert (_formats[0].templates.begin(), _template_html);
        _formats.push_back(formats[1]); _formats[1].templates.insert (_formats[1].templates.begin(), _template_hex);
        _formats.push_back(formats[2]); _formats[2].templates.insert (_formats[2].templates.begin(), _template_delphi);
        _formats.push_back(formats[3]); _formats[3].templates.insert (_formats[3].templates.begin(), _template_vb);
        _formats.push_back(formats[4]); _formats[4].templates.insert (_formats[4].templates.begin(), _template_rgb);
        _formats.push_back(formats[5]); _formats[5].templates.insert (_formats[5].templates.begin(), _template_float);
        _formats.push_back(formats[6]); _formats[6].templates.insert (_formats[6].templates.begin(), _template_hsv);
        _formats.push_back(formats[7]); _formats[7].templates.insert (_formats[7].templates.begin(), _template_hsl);
        _formats.push_back(formats[8]); _formats[8].templates.insert (_formats[8].templates.begin(), _template_long);  
    }

    void save()
    {
        // распихаем свежие данные по переменным
        _template_html   = _formats[0].templates[0];
        _template_hex    = _formats[1].templates[0];
        _template_delphi = _formats[2].templates[0];
        _template_vb     = _formats[3].templates[0];
        _template_rgb    = _formats[4].templates[0];
        _template_float  = _formats[5].templates[0];
        _template_hsv    = _formats[6].templates[0];
        _template_hsl    = _formats[7].templates[0];
        _template_long   = _formats[8].templates[0];

        // запишем
        if (_currentini.access())
        {
            _currentini.writevalue("common", "balloon",         _balloon);
            _currentini.writevalue("common", "theme",           _theme);
            _currentini.writevalue("common", "clipboard",       _clipboard);
            _currentini.writevalue("common", "hotkeymods",      _hotkeymods);
            _currentini.writevalue("common", "hotkeyvk",        _hotkeyvk);
            _currentini.writevalue("common", "zoom",            _zoom);
            _currentini.writevalue("common", "clickmode",       _clickmode);
            _currentini.writevalue("common", "colorformat",     _colorformat);
            _currentini.writevalue("common", "floatprecision",  _floatprecision);
            _currentini.writevalue("common", "tmplt_html",      _template_html);
            _currentini.writevalue("common", "tmplt_hex",       _template_hex);
            _currentini.writevalue("common", "tmplt_delphi",    _template_delphi);
            _currentini.writevalue("common", "tmplt_vb",        _template_vb);
            _currentini.writevalue("common", "tmplt_rgb",       _template_rgb);
            _currentini.writevalue("common", "tmplt_float",     _template_float);
            _currentini.writevalue("common", "tmplt_hsv",       _template_hsv);
            _currentini.writevalue("common", "tmplt_hsl",       _template_hsl);
            _currentini.writevalue("common", "tmplt_long",      _template_long);
            _currentini.writevalue("common", "fontface",        _fontface);
            _currentini.writevalue("common", "fontweight",      _fontweight);
            _currentini.writevalue("common", "fontsize",        _fontsize);
            _currentini.writevalue("common", "fontcolor",       _fontcolor);
        }
        else
        {
            _registry.createvalue("balloon",                    _balloon);
            _registry.createvalue("theme",                      _theme);
            _registry.createvalue("clipboard",                  _clipboard);
            _registry.createvalue("hotkeymods",                 _hotkeymods);
            _registry.createvalue("hotkeyvk",                   _hotkeyvk);
            _registry.createvalue("zoom",                       _zoom);
            _registry.createvalue("clickmode",                  _clickmode);
            _registry.createvalue("colorformat",                _colorformat);
            _registry.createvalue("floatprecision",             _floatprecision);
            _registry.createvalue("tmplt_html",                 _template_html);
            _registry.createvalue("tmplt_hex",                  _template_hex);
            _registry.createvalue("tmplt_delphi",               _template_delphi);
            _registry.createvalue("tmplt_vb",                   _template_vb);
            _registry.createvalue("tmplt_rgb",                  _template_rgb);
            _registry.createvalue("tmplt_float",                _template_float);
            _registry.createvalue("tmplt_hsv",                  _template_hsv);
            _registry.createvalue("tmplt_hsl",                  _template_hsl);
            _registry.createvalue("tmplt_long",                 _template_long);
            _registry.createvalue("fontface",                   _fontface);
            _registry.createvalue("fontweight",                 _fontweight);
            _registry.createvalue("fontsize",                   _fontsize);
            _registry.createvalue("fontcolor",                  _fontcolor);
        }
    }

/***********************************************************************************************************/
private:
    HINSTANCE               _hinst;             // 0x400... и тп :)
    handler                 _mainhwnd;          // хэндл главного окна
    ini                     _currentini;        // текущий ини файл
    registry                _registry;          // registry

    bool                    _balloon;           // показывать или нет балон при запуске
    autoload                _autoload;          // класс оперирующий автозагрузкой из реестра
    bool                    _theme;             // тема
    bool                    _clipboard;         // копировать или нет в клипбоард?
    int                     _zoom;              // разрешение окошка
    int                     _clickmode;         // режим кликов 1-click, 2-click

    int                     _colorformat;       // HTML, RGB...
    int                     _floatprecision;    // float precision

    vector<colorformats>    _formats;           // вся пачка
    string                  _template_html;     // html template
    string                  _template_hex;      // hex template
    string                  _template_delphi;   // delphi template
    string                  _template_vb;       // vb template
    string                  _template_rgb;      // rgb template
    string                  _template_float;    // float template
    string                  _template_hsv;      // hsv template
    string                  _template_hsl;      // hsl template
    string                  _template_long;     // long template

    uint                    _hotkeymods;        // hotkey modifiers
    uint                    _hotkeyvk;          // hotkey virtualkey

    string                  _fontface;          // имя шрифта
    bool                    _fontweight;        // bold or regular
    int                     _fontsize;          // размер шрифта
    string                  _fontcolor;         // цвет шрифта
};

/***********************************************************************************************************/
#endif