#if !defined mousemon_h
#define mousemon_h
/************************************************************************************************************ 
** mouse monitor
** .--------.-----.--.--.-----.-----.--------.-----.-----.
** |        |  _  |  |  |__ --|  -__|        |  _  |     |
** |__|__|__|_____|_____|_____|_____|__|__|__|_____|__|__|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "statics.h"
#include "lib/dynamic/dynamic.h"

/***********************************************************************************************************/

class mousemon: public dynamic
{
public:
    mousemon(HWND hwnd) : _suspend (true), _hwnd (hwnd), _ex (0), _ey (0), _x (0), _y (0) { }
   ~mousemon() { kill(); }

    void go()                 { Sleep(5); _thread.resume(); _suspend = false; }
    void suspend()            { if (!_suspend) _thread.suspend(); _suspend = true; }
    bool working()            { return !_suspend; }
    void etalon(int x, int y) { _ex = x; _ey = y; }

private:
    void initthread() { }

    /* тело потока */
    void loop()        
    {
        for (;;)
        {
            POINT pt; GetCursorPos(&pt); _x = pt.x; _y = pt.y;

            if ((abs(_x - _ex) > 2) || (abs(_y - _ey) > 2))
            {
                SendMessage(_hwnd, WM_MYICONNOTIFY, 0, (LPARAM)WM_MOUSEOUT);
            }
            Sleep(10);
        }
    } 

    void flushthread() {}

private:
    bool        _suspend;
    HWND        _hwnd;

    int         _ex, _ey;    // координаты мыши при движении над систреем
    int         _x, _y;
};

/***********************************************************************************************************/
#endif