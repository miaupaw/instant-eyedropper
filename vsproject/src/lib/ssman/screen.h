#if !defined screen_h
#define screen_h
/************************************************************************************************************ 
** screenshot maker
** .-----.----.----.-----.-----.-----.
** |__ --|  __|   _|  -__|  -__|     |
** |_____|____|__| |_____|_____|__|__|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "../win/handler.h" // класс handler для манипулирования окном

/***********************************************************************************************************/
class screen
{
public:
    screen(void)
    {
        _currect     = { 0, 0, 0, 0 };

        _capturedc   = NULL;
        _capturehbmp = NULL;
        _originalobj = NULL;
    }

   ~screen() { release(); }

    void capture()
    {
        // удалить старое если есть
        release(); 

        // размеры
        int left = GetSystemMetrics(SM_XVIRTUALSCREEN);     // can be negative
        int top  = GetSystemMetrics(SM_YVIRTUALSCREEN);     // can be negative
        int hres = GetSystemMetrics(SM_CXVIRTUALSCREEN);    // total width
        int vres = GetSystemMetrics(SM_CYVIRTUALSCREEN);    // total height

        // текущие габариты
        _currect.left   = left;
        _currect.top    = top;
        _currect.right  = hres;
        _currect.bottom = vres;

        // захватить и сохранить
        HDC desktopdc = GetDC(NULL);

        _capturedc    = CreateCompatibleDC(desktopdc);
        _capturehbmp  = CreateCompatibleBitmap(desktopdc, hres, vres);
        _originalobj  = SelectObject(_capturedc, _capturehbmp);

        BitBlt(_capturedc, 0, 0, hres, vres, desktopdc, left, top, SRCCOPY | CAPTUREBLT);
        ReleaseDC(NULL, desktopdc);
    }

    // ослобонить все что надо
    void release()
    {
        if (_capturedc)
        {
            DeleteObject(_originalobj);
            DeleteObject(_capturehbmp);
            DeleteDC(_capturedc);
            _capturedc = NULL;
        }
    }

    // текущие данные для желающих
    void rect(RECT &rc)  { rc = _currect; }
    HDC  getdc()         { return _capturedc; }

private:
    RECT    _currect;

    HDC     _capturedc;
    HBITMAP _capturehbmp;
    HGDIOBJ _originalobj;
};

/***********************************************************************************************************/
#endif