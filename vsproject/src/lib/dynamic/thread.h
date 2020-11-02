#if !defined thread_h
#define thread_h
/************************************************************************************************************ 
**  __   __                        __ 
** |  |_|  |--.----.-----.---.-.--|  |
** |   _|     |   _|  -__|  _  |  _  |
** |____|__|__|__| |_____|___._|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>

/***********************************************************************************************************/
class thread
{
public:
    thread(DWORD (WINAPI * pfun)(void * arg), void * parg)
    {
        // create (security attributes, stack size...)
        _handle = CreateThread(0, 0, pfun, parg, CREATE_SUSPENDED, &_tid);
    }

   ~thread()            { CloseHandle (_handle); }
    
    void resume()       { ResumeThread(_handle); }
    void suspend()      { SuspendThread(_handle); }
    void waitfordeath() { WaitForSingleObject(_handle, 200); } // be 2000 !!!

private:
    HANDLE  _handle;
    DWORD   _tid;       // thread id
};

/***********************************************************************************************************/
class mutex
{
    friend class lock;

public:
    mutex()             { InitializeCriticalSection(&_critsection); }
   ~mutex()             { DeleteCriticalSection(&_critsection); }

private:
    void acquire()      { EnterCriticalSection(&_critsection); }
    void release()      { LeaveCriticalSection(&_critsection); }

    CRITICAL_SECTION _critsection;
};

/***********************************************************************************************************/
class lock 
{
public:
	
	lock(mutex &mx) : _mutex (mx) { _mutex.acquire(); } // acquire the state of the semaphore
   ~lock()                        { _mutex.release(); } // release the state of the semaphore

private:
	mutex   &_mutex;
};

/***********************************************************************************************************/
class event
{
public:
    // start in non-signaled state (red light)
    event()             { _handle = CreateEvent(0, TRUE, FALSE, 0); }
   ~event()             { CloseHandle(_handle); }

    void greenlight()   { SetEvent(_handle); }                      // put into signaled state
    void redlight()     { ResetEvent (_handle); }                   // put into non-signaled state
    void wait()         { WaitForSingleObject(_handle, INFINITE); } // wait until event is in signaled (green) state

private:
    HANDLE _handle;
};

/***********************************************************************************************************/
class atomiccounter
{
public:
    atomiccounter() : _counter(0) {}

    long inc() { return InterlockedIncrement(&_counter); } // return the sign (or zero) of the new value
    long dec() { return InterlockedDecrement(&_counter); } // return the sign (or zero) of the new value

    BOOL isnonzeroreset() { return InterlockedExchange(&_counter, 0) != 0; }
  
private:
    long _counter;
};

/***********************************************************************************************************/
#endif