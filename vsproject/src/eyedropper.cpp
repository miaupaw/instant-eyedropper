/************************************************************************************************************ 
**                       __                                    
** .-----.--.--.-----.--|  |----.-----.-----.-----.-----.----.
** |  -__|  |  |  -__|  _  |   _|  _  |  _  |  _  |  -__|   _|
** |_____|___  |_____|_____|__| |_____|   __|   __|_____|__|   controller
**       |_____|                      |__|  |__|                                      instant-eyedropper.com
************************************************************************************************************/

#include "eyedropper.h"

/***********************************************************************************************************/
LRESULT CALLBACK eyedropperwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
    // вытаскивает указатель из винды
    eyedroppercontroller * pctrl = getwindownlong<eyedroppercontroller*>(hwnd);

    switch (msg)
    {
    // при первом создании окна
    case WM_CREATE:
        pctrl = new eyedroppercontroller(hwnd, (CREATESTRUCT*)lparam);
        setwindownlong<eyedroppercontroller*>(hwnd, pctrl);
        return 0;

    case WM_DESTROY:          setwindownlong<eyedroppercontroller*>(hwnd, 0); delete pctrl; return 0; // дестрой 
    case WM_ERASEBKGND:       return 0; // чтобы небыло мерцания
    case WM_PAINT:            pctrl->paint(); return 0; // вызывается постоянно при перерисовке
    case WM_DISPLAYCHANGE:    pctrl->displaychanged(); return 0; // по идее приходит всем окнам если конфигурация изменилась
    case MSG_STARTEYEDROPPER: pctrl->start(); return 0; // передают установки
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

/***********************************************************************************************************/
/* сохраняет хэндлы окна и родителя внутри объекта 
** (инициализирует внутренние переменные реальными значениями) */
eyedroppercontroller::eyedroppercontroller(HWND hwnd, CREATESTRUCT *pcrt)
:   _eyedropper     ((eyedropper*)pcrt->lpCreateParams),
    _hwnd           (hwnd),                
    _hwndparent     (pcrt->hwndParent),
    _hinst          ((HINSTANCE)GetModuleHandle(NULL)),
    _restheme       (NULL),
    _color          ()
{}

/* деструктор */
eyedroppercontroller::~eyedroppercontroller() {}

/* перерисовка */
void eyedroppercontroller::paint()
{
    // начинаем рисовать
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(_hwnd, &ps);
    //SetBkMode(hdc, TRANSPARENT);
        
        // создать виртуальный дэвайс контекст
        HDC hdccompatible = CreateCompatibleDC(hdc);
        SetBkMode(hdccompatible, TRANSPARENT);

        // нарисовать бэкграунд битмап на окно
        HBITMAP hbmp = (HBITMAP)LoadImage(_hinst,
                                          _restheme,
                                          IMAGE_BITMAP, 
                                          EYEDROPPER_WIDTH,
                                          EYEDROPPER_HEIGHT,
                                          LR_CREATEDIBSECTION);
        HBITMAP oldbmp = (HBITMAP)SelectObject(hdccompatible, hbmp);
        
        // нарисовать квадратики
        if (_eyedropper->sshdc) { paintlens(hdccompatible, _eyedropper->sshdc); }

        // нарисовать крестик
        paintcross(hdccompatible);
        
        // нарисовать цифры
        string str = getcolorstring();
        if (!str.empty()) textout(hdccompatible, str.c_str());
        else              textout(hdccompatible, "......");

        // вывести все
        BitBlt(hdc, 0, 0, EYEDROPPER_WIDTH, EYEDROPPER_HEIGHT, hdccompatible, 0, 0, SRCCOPY);

        // вернуть объекты на место
        SelectObject(hdccompatible, oldbmp);
        DeleteDC(hdccompatible);
        DeleteObject(oldbmp);
        DeleteObject(hbmp);

    EndPaint(_hwnd, &ps);
}

/* изменилась конфигурация мониторов */
void eyedroppercontroller::displaychanged()
{
    _eyedropper->monitors.initialize();
}

/* кастомные обработчики ***********************************************************************************/
/* передали параметры для дроппера */
void eyedroppercontroller::start()
{
    // указать на правильный бэкграунд
    if (!*_eyedropper->base->theme()) _restheme = MAKEINTRESOURCE(IDB_EYEDROPPER);
    else                              _restheme = MAKEINTRESOURCE(IDB_EYEDROPPERDARK);

    _hwnd.show();
}

/* protected: **********************************************************************************************/
/* нарисовать квадратики */
bool eyedroppercontroller::paintlens(HDC hdctarget, HDC hdcsource)
{
    int width  = 3;
    int height = 3;
    int shift  = 1;

    int zoom = *_eyedropper->base->zoom();

    if (zoom == 0) { width = 3; height = 3; shift = 1; }
    if (zoom == 1) { width = 5; height = 5; shift = 2; }
    if (zoom == 2) { width = 7; height = 7; shift = 3; }
    if (zoom == 3) { width = 9; height = 9; shift = 4; }

    StretchBlt(hdctarget,               // handle to destination DC
               1,                       // x-coord of destination upper-left corner
               1,                       // y-coord of destination upper-left corner
               EYEDROPPER_HEIGHT - 2,   // width of destination rectangle
               EYEDROPPER_HEIGHT - 2,   // height of destination rectangle
               hdcsource,               // handle to source DC
               _eyedropper->x - shift,  // x-coord of source upper-left corner
               _eyedropper->y - shift,  // y-coord of source upper-left corner
               width,                   // width of source rectangle
               height,                  // height of source rectangle
               SRCCOPY);                // raster operation code

    return false;
}

// нарисовать крестик
bool eyedroppercontroller::paintcross(HDC hdc)
{
    SetROP2(hdc, R2_NOTXORPEN);
    SetPixel(hdc, 30, 30, RGB(0, 0, 0));
    SetPixel(hdc, 27, 30, RGB(0, 0, 0));
    SetPixel(hdc, 28, 30, RGB(0, 0, 0));
    SetPixel(hdc, 29, 30, RGB(0, 0, 0));
    SetPixel(hdc, 31, 30, RGB(0, 0, 0));
    SetPixel(hdc, 32, 30, RGB(0, 0, 0));
    SetPixel(hdc, 33, 30, RGB(0, 0, 0));
    SetPixel(hdc, 30, 27, RGB(0, 0, 0));
    SetPixel(hdc, 30, 28, RGB(0, 0, 0));
    SetPixel(hdc, 30, 29, RGB(0, 0, 0));
    SetPixel(hdc, 30, 31, RGB(0, 0, 0));
    SetPixel(hdc, 30, 32, RGB(0, 0, 0));
    SetPixel(hdc, 30, 33, RGB(0, 0, 0));

    return false;
}

// вычислить colorstring
string eyedroppercontroller::getcolorstring()
{
    _color.setcolor(_eyedropper->color);
    _color.settemplates(_eyedropper->base->getformats());

    switch (*_eyedropper->base->colorformat()) {
    case ID_FORMAT_HTML:     return _color.html(false);
    case ID_FORMAT_HEX:      return _color.hex(false);
    case ID_FORMAT_DELPHI:   return _color.delphi(false);
    case ID_FORMAT_VB:       return _color.vb(false);
    case ID_FORMAT_RGB:      return _color.rgb(false);
    case ID_FORMAT_RGBFLOAT: return _color.rgbfloat(false, *_eyedropper->base->floatprecision());
    case ID_FORMAT_HSV:      return _color.hsv(false);
    case ID_FORMAT_HSL:      return _color.hsl(false);
    case ID_FORMAT_LONG:     return _color.longvalue(false); }

    return 0;
}

// вывести текст
bool eyedroppercontroller::textout(HDC hdc, const char * str)
{
    // вычислить относительный прямоугольник в центре которого будет текст
    RECT rect;

    rect.top    = 2;                        // высота бордера
    rect.right  = EYEDROPPER_WIDTH  - 1;    // 1 - ширина бордера
    rect.bottom = EYEDROPPER_HEIGHT - 1;    // высота бордера
    rect.left   = EYEDROPPER_HEIGHT;        // ширина квадрата вместе с бордерами
    
    // устанавить высоту шрифта в зависимости от формата кода
    int colorformat = *_eyedropper->base->colorformat();
    int fontsize    =  _eyedropper->base->fontsize();

    if (colorformat == ID_FORMAT_HTML)     fontsize = fontsize;
    if (colorformat == ID_FORMAT_HEX)      fontsize = fontsize;
    if (colorformat == ID_FORMAT_DELPHI)   fontsize = fontsize - 5;
    if (colorformat == ID_FORMAT_VB)       fontsize = fontsize - 6;
    if (colorformat == ID_FORMAT_RGB)      fontsize = fontsize - 10;
    if (colorformat == ID_FORMAT_RGBFLOAT) fontsize = fontsize - 12;
    if (colorformat == ID_FORMAT_HSV)      fontsize = fontsize - 5;
    if (colorformat == ID_FORMAT_HSL)      fontsize = fontsize - 5;
    if (colorformat == ID_FORMAT_LONG)     fontsize = fontsize - 4;

    // установка шрифта. логическая единица = пиксель.
    SetMapMode(hdc, MM_TEXT);               

    // если шрифт курьер или режим rgb - установим жирноту
    int weight = FW_NORMAL;
    if (_eyedropper->base->fontweight())   weight = FW_BOLD;
    if (colorformat == ID_FORMAT_RGB)      weight = FW_BOLD;
    
    HFONT hfnt = selectfont(_eyedropper->base->fontface(), fontsize, weight);

    SetTextColor(hdc, _eyedropper->base->fontcolor());
    HFONT oldfnt = (HFONT)SelectObject(hdc, hfnt);
    
    // вычисление координат для вывода текста
    SIZE size;
    GetTextExtentPoint32(hdc, str, strlen(str), &size);
    int x = (rect.left + rect.right  - size.cx)/2;
    int y = (rect.top  + rect.bottom - size.cy)/2;

    // вывод текста
    if (colorformat == ID_FORMAT_RGBFLOAT)
    {
        rect.top  = 6;
        rect.left = rect.left + 5;
        DrawText(hdc, str, strlen(str), &rect, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
    }
    else TextOut(hdc, x, y, str, strlen(str));
    
    // почистить во избежание утечек
    SelectObject(hdc, oldfnt);
    DeleteObject(hfnt);
    DeleteObject(oldfnt);

    return false;
}

HFONT eyedroppercontroller::selectfont(string face, int fontsize, int weight)
{
    HFONT hfnt = CreateFont(fontsize, 
                            0, 0, 0,
                            weight, 
                            FALSE, FALSE, FALSE,
                            ANSI_CHARSET, 
                            OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, 
                            DEFAULT_QUALITY,
                            DEFAULT_PITCH, 
                            face.c_str());
    return hfnt;    
}

/***********************************************************************************************************/




