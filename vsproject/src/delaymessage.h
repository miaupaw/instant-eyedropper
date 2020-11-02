#if !defined delaymessage_h
#define delaymessage_h
/************************************************************************************************************ 
** delay message
**     __       __                                                           
** .--|  |-----|  |-----.--.--.--------.-----.-----.-----.-----.-----.-----.
** |  _  |  -__|  |  _  |  |  |        |  -__|__ --|__ --|  _  |  _  |  -__|
** |_____|_____|__|___._|___  |__|__|__|_____|_____|_____|___._|___  |_____|
**                      |_____|                                |_____|      
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "statics.h"
#include "lib/dynamic/dynamic.h"

/***********************************************************************************************************/
class delaymessage: public dynamic
{
public:
    delaymessage(HWND hwnd) : _suspend(true), _hwnd(hwnd) {}
   ~delaymessage() { kill(); }

    void send(uint message, WPARAM wparam, LPARAM lparam, int delay)
    { 
        _msg    = message;
        _wparam = wparam;
        _lparam = lparam;
        _delay  = delay;

        _thread.resume();
        _suspend = false;
    }

    void suspend() { if (!_suspend) _thread.suspend(); _suspend = true; }
    bool working() { return !_suspend; }

private:
    void initthread() 
    {
        Sleep(_delay);
        SendMessage(_hwnd, _msg, _wparam, _lparam);
        suspend();
    }

    /* тело потока */
    void loop()        { for (;;) { Sleep(100); } } 
    void flushthread() {}

private:
    bool        _suspend;

    HWND        _hwnd;
    uint        _msg;
    WPARAM      _wparam; 
    LPARAM      _lparam;
    int         _delay;
};

/***********************************************************************************************************/
#endif