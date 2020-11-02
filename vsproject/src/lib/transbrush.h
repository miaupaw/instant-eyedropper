#if !defined transbrush_h
#define transbrush_h
/************************************************************************************************************ 
** pseudo transparent brush (camouflage)
**  __                          __                     __    
** |  |_.----.-----.-----.-----|  |--.----.--.--.-----|  |--.
** |   _|   _|  _  |     |__ --|  _  |   _|  |  |__ --|     |
** |____|__| |___._|__|__|_____|_____|__| |_____|_____|__|__|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>

/***********************************************************************************************************/

class transbrush
{
public:
    transbrush() { _sample = NULL; _hbrush = NULL; }
   ~transbrush() { DeleteObject(_hbrush); }

    void make(HWND sample)
    {
        _sample = sample;

        // destroy old brush
        if (_hbrush) DeleteObject(_hbrush);

        _hbrush = NULL;

        // get tab control dimensions
        RECT rc; GetWindowRect(_sample, &rc);

        // get the tab control dc
        HDC hDC = GetDC(_sample);

        // create a compatible dc
        HDC hDCMem = CreateCompatibleDC(hDC);
        HBITMAP hBmp = CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
        HBITMAP hBmpOld = (HBITMAP)(SelectObject(hDCMem, hBmp));

        // tell the tab control to paint in our dc
        SendMessage(_sample, WM_PRINTCLIENT, (WPARAM)(hDCMem), (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));

        // create a pattern brush from the bitmap selected in our dc
        _hbrush = CreatePatternBrush(hBmp);

        // restore the bitmap
        SelectObject(hDCMem, hBmpOld);

        // cleanup
        DeleteObject(hBmp);
        DeleteDC(hDCMem);
        ReleaseDC(_sample, hDC);
    }

    void adjust(HDC hdc, HWND hwnd)
    {
        RECT rc;
        // get the controls window dimensions
        GetWindowRect(hwnd, &rc);

        // set the background mode to transparent
        SetBkMode(hdc, TRANSPARENT);

        // map the coordinates to coordinates with the upper left corner of dialog control as base
        MapWindowPoints(NULL, _sample, (LPPOINT)(&rc), 2);

        // adjust the position of the brush for this control (else we see the top left of the brush as background)
        SetBrushOrgEx(hdc, -rc.left, -rc.top, NULL);
    }

    HBRUSH brush() { return (HBRUSH)_hbrush; }

private:
    HWND    _sample;
    HBRUSH  _hbrush;
};

/***********************************************************************************************************/
#endif