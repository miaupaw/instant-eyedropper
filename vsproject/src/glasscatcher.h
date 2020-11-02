#if !defined glasscatcher_h
#define glasscatcher_h
/************************************************************************************************************ 
** glasscatcher - суперинтеллектуальная, сверхтехнологическая система, широкого спектра действия
**        __                               __         __               
** .-----|  |-----.-----.-----.-----.-----|  |_.-----|  |--.-----.----.
** |  _  |  |  _  |__ --|__ --|  ___|  _  |   _|  ___|     |  -__|   _|
** |___  |__|___._|_____|_____|_____|___._|____|_____|__|__|_____|__|  
** |_____|                                                                            instant-eyedropper.com
************************************************************************************************************/

#include "statics.h"                        // железные значения
#include "base.h"                           // base
#include "eyedropper.h"                     // окошечко
#include "lib/win/winbase.h"                // базовыЙ класс для создания окна
#include "lib/win/gwl.h"                    // Getting and Setting WindowLong: default is GWL_USERDATA
#include "lib/ssman/screen.h"               // скриншот
#include "mousemon.h"                       // поток наблюдающий за мышой

/***********************************************************************************************************/
LRESULT CALLBACK glasscatcherwndproc(HWND, UINT, WPARAM, LPARAM);

/***********************************************************************************************************/
/* регистратор класса и создатель окна */
class glasscatcher: public winbase
{
public:
    glasscatcher(HINSTANCE hinst, handler hwndparent, base * bp, eyedropper * e)
    :   winbase         (glasscatcherwndproc, "glasscatcher", hinst),
        _base           (bp),
        _clickmode      (bp->clickmode()),
        ed              (e)
    {
        hbrBackground   = 0;                    // чтобы не делетило бэкграунд без спросу

        _hinstance      = hinst; 
        _hwnd           = 0;
        _hwndparent     = hwndparent;           // хэндл родителя (нужен для дочерних или owned окон)
        _windowname     = "";                   // имя окна
        _exstyle        = WS_EX_TOPMOST |       // всегда наверху
                          WS_EX_NOACTIVATE |    // не забирает фокус у других приложений 
                          WS_EX_TRANSPARENT;    // прозрачное
        _style          = WS_POPUP |            // без лишних приколов, нельзя юзать с ws_child
                          WS_CLIPCHILDREN;      // не обновляет области занятые дочерними окнами при wm_paint

        this->reg(); _hwnd = this->create(-1, -1, 1, 1, this);

        // чтобы к первому использованию квадратик был готов, 
        // чтобы на xp при первом наведении не появлялись часики рядом с курсором
        // наверное потому что при появлении окна - стартует тред
        _hwnd.show();
    }
    
public:
    eyedropper * ed;            

    const int * clickmode() { return _clickmode; }
    handler     hwnd()      { return gethandle(); }

private:
    base *      _base;          // указатель на базу
    const int * _clickmode;     // указатель на переменную в base
};

/***********************************************************************************************************/
class glasscontroller
{
public:
    glasscontroller(HWND hwnd, CREATESTRUCT * pcreat);
   ~glasscontroller();
    
    /* системные обработчики */
    void            paint();
    void            showwindow(BOOL);
    void            keydown(WPARAM);
    void            keyup(WPARAM);
    void            rbuttonup();
    void            lbuttondown();
    void            lbuttonup();
    void            mousemove();

    /* кастомные обработчики и методы */
    void            iconnotify(LPARAM);
    void            hotkey(int, UINT);
    void            proceed(WPARAM, LPARAM);
    void            popup();

private:
    glasscatcher *  _glass;                 // объект
    handler         _hwnd;                  // хендлер мона
    handler         _hwndparent;            // хэндл родителя
    HINSTANCE       _hinst;                 // экземпляр
    mousemon        _mousemon;              // следилка за мышей, генерирует WM_MOUSEOUT
    screen          _screen;                // скриншотер
    
    int             _mousespeed;            // скорость мышки
    const int *     _clickmode;             // указатель на переменную в модели
    bool            _capturemode;           // режим работы
    BOOL            _esc;                   // хоткей зареган или нет
};

/***********************************************************************************************************/
#endif