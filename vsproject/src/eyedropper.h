#if !defined eyedropper_h
#define eyedropper_h
/************************************************************************************************************ 
**                       __                                    
** .-----.--.--.-----.--|  |----.-----.-----.-----.-----.----.
** |  -__|  |  |  -__|  _  |   _|  _  |  _  |  _  |  -__|   _|
** |_____|___  |_____|_____|__| |_____|   __|   __|_____|__|  
**       |_____|                      |__|  |__|                                      instant-eyedropper.com
************************************************************************************************************/

#include <tchar.h>

#include "statics.h"
#include "resource.h"
#include "base.h"

#include "lib/win/winbase.h"                // базовыЙ класс для создания окна
#include "lib/win/gwl.h"                    // Getting and Setting WindowLong: default is GWL_USERDATA
#include "lib/cursor.h"                     // специфические курсоры
#include "lib/color.h"                      // конвертор цвета
#include "lib/monitors/monitors.h"          // класс про мониторы

#define EYEDROPPER_WIDTH    177
#define EYEDROPPER_HEIGHT   61              // должен быть нечетным чтобы был пиксел посередине

/***********************************************************************************************************/
/* winproc основного окна айдроппера */
LRESULT CALLBACK eyedropperwndproc(HWND, UINT, WPARAM, LPARAM);

/***********************************************************************************************************/
/* регистратор класса и создатель окна */
class eyedropper: public winbase
{
public:
    eyedropper(HINSTANCE hinst, HWND hwndparent, base * bp)
    :   winbase         (eyedropperwndproc, "eyedropper", hinst),
        base            (bp),
        monitors        (),
        cursor          ((HINSTANCE)GetModuleHandle(NULL))
    {
        hbrBackground   = 0;                // чтобы не делетило бэкграунд без спросу
        
        _hinstance      = hinst; 
        _hwnd           = 0;
        _hwndparent     = hwndparent;       // хэндл родителя (нужен для дочерних или owned окон)
        _windowname     = "";               // имя окна
        _exstyle        = WS_EX_TOPMOST |   // всегда наверху
                          WS_EX_NOACTIVATE; // не забирает фокус у других приложений 
        _style          = WS_POPUP |        // без лишних приколов, нельзя юзать с ws_child
                          WS_CLIPCHILDREN;  // не обновляет области занятые дочерними окнами при wm_paint

        // создадим окно
        this->reg(); _hwnd = this->create(10, 10, EYEDROPPER_WIDTH, EYEDROPPER_HEIGHT, this); _hwnd.hide();
    }

   ~eyedropper() {}    

/***********************************************************************************************************/
/* public access members */
public:
    base *      base;       // link to data
    HDC         sshdc;      // hdc of screenshot
    monitors    monitors;   // мониторы кароч
    cursor      cursor;     // специфические курсоры
    uint        x;          // координата x
    uint        y;          // координата y
    COLORREF    color;      // текущий цвет

    // init virtual monitors
    void monitorsinitialize() { monitors.initialize(); }

    // началось айдропание
    void start(HDC hdc)
    {
        sshdc = hdc; // store screenshot dc
        _hwnd.sendmessage(MSG_STARTEYEDROPPER, 0, 0); // send message to eyedroppercontroller
        SetCursor(cursor.mode(DROPPER));
    }

    // событие что мышка двигается
    void move()
    {
        // взять текущие координаты мыши
        POINT pt, correct_pt; GetCursorPos(&pt);

        // to virtual
        point vpt = pt; monitors.screentovirtual(vpt);

        // установить координаты в структуру данных для айдроппера
        x = vpt.x;  y = vpt.y;

        // вычислить цвет
        color = GetPixel(sshdc, vpt.x, vpt.y);
        
        // правим координаты для окошка чтобы оно не вылазило
        if (monitors.ok()) correct_pt = correction(pt);

        _hwnd.move(correct_pt.x, correct_pt.y, EYEDROPPER_WIDTH, EYEDROPPER_HEIGHT);
        _hwnd.invalidate();
    }
    
    // айдропание закончилось
    void stop(bool copy)
    {
        _hwnd.hide();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        if (copy) _hwndparent.sendmessage(MSG_COLOR, 0, (LPARAM)color);
    }

/***********************************************************************************************************/
private:
    /* правим координаты для окошка чтобы оно не вылазило */
    /* значение приходит в обычных поинтах 
    /* возвращаем в обычных */
    POINT correction(POINT pt)
    {
        // correct and tmp points
        point сpt = pt, tpt = pt;

        // константы: смещения относительно курсора
        int mediana = (EYEDROPPER_HEIGHT - 1) / 2;      // половина высоты
        int fxedge = mediana;                           // передний(левый) край
        int bxedge = mediana + EYEDROPPER_WIDTH - 1;    // задний(правый) край

        // вычисляем лимиты через временную точку
        // ok = точка в принципе находится внутри какого-нить экрана
        tpt = pt; bool ok = monitors.edge(tpt, R);  int rlimit = abs(pt.x - tpt.x);

        // если не помещается по горизонтали
        if (ok && (bxedge > rlimit)) { сpt.x -= bxedge;  }
        else                         { сpt.x += mediana; }

        // координаты левого и правого края окна
        point lx = сpt;
        point rx = сpt; rx.x += (bxedge - mediana);    // плюс почти ширина айдроппера вобщем

        // вычисляем лимиты через временную точку
        tpt = lx; monitors.edge(tpt, T); int lxtlimit = abs(lx.y - tpt.y);
        tpt = rx; monitors.edge(tpt, T); int rxtlimit = abs(rx.y - tpt.y);
        tpt = lx; monitors.edge(tpt, B); int lxblimit = abs(lx.y - tpt.y);
        tpt = rx; monitors.edge(tpt, B); int rxblimit = abs(rx.y - tpt.y);

        // присваиваем наименьшее значение из двух
        int tlimit = lxtlimit; if (rxtlimit < lxtlimit) tlimit = rxtlimit;
        int blimit = lxblimit; if (rxblimit < lxblimit) blimit = rxblimit;

        // обычное смещение окошка для "y"
        сpt.y -= mediana;

        // если не помещается по вертикали
        if (ok && tlimit < mediana) { сpt.y = pt.y - tlimit; }
        if (ok && blimit < mediana) { сpt.y = pt.y + blimit - mediana*2; }

        return сpt;
    }
};

/***********************************************************************************************************/
class eyedroppercontroller
{
public:
    eyedroppercontroller(HWND hwnd, CREATESTRUCT *pcreat);
   ~eyedroppercontroller();

    /* системные обработчики */
    void            paint();

    /* кастомные обработчики */
    void            start();
    void            displaychanged();

protected:
    bool            paintlens(HDC, HDC);
    bool            paintcross(HDC);
    string          getcolorstring();
    bool            textout(HDC, const char*);
    HFONT           selectfont(string, int, int);

private:
    eyedropper *    _eyedropper;        // pointer to "родительский" объект
    handler         _hwnd;              // хендлер мона
    handler         _hwndparent;        // хэндл родителя
    HINSTANCE       _hinst;             // экземпляр

    LPCTSTR         _restheme;          // указатель на бэкграунд (IDB_EYEDROPPER/IDB_EYEDROPPERDARK)
    color           _color;             // название само за себя говорит, правда? )
};

/***********************************************************************************************************/
#endif