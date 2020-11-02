#if !defined dynamicobject_h
#define dynamicobject_h
/************************************************************************************************************ 
** dynamic object thread incapsulated
**     __                            __       
** .--|  |--.--.-----.-----.--------|__|-----.
** |  _  |  |  |     |  _  |        |  |   __|
** |_____|___  |__|__|___._|__|__|__|__|_____|
**       |_____|                                                                      instant-eyedropper.com
************************************************************************************************************/

#include "thread.h"
#include <windows.h>

/***********************************************************************************************************/
class dynamic
{
public:
    dynamic()
    :   _isdying    (0),
#pragma warning(disable: 4355)
        _thread     (threadentry, this) // 'this' used before initialized
#pragma warning(default: 4355)
    {}

    virtual ~dynamic() {}
    void        kill() { _isdying++; flushthread(); _thread.waitfordeath(); } // let's make sure it's gone

protected:
    virtual void initthread()  = 0;
    virtual void loop()        = 0;
    virtual void flushthread() = 0;

    static DWORD WINAPI threadentry(void * parg)
    {
        dynamic * pdynamic = (dynamic*)parg;

        pdynamic->initthread();
        pdynamic->loop();

        return 0;
    }

    int         _isdying;
    thread      _thread;
};

/***********************************************************************************************************/
#endif