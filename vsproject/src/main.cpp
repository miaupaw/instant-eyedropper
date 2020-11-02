/************************************************************************************************************
** Name: ♥ Instant Eyedropper                                                          instant-eyedropper.com
** Type:   Color detection tool
** Author: Konstantin Yagola
**  __             __               __    ______                __                                    
** |  |-----.-----|  |_.-----.-----|  |_ |   ___|--.--.-----.--|  |----.-----.-----.-----.-----.----.
** |  |     |__ --|   _|  _  |     |   _||   ___|  |  |  -__|  _  |   _|  _  |  _  |  _  |  -__|   _|
** |__|__|__|_____|____|___._|__|__|____||______|___  |_____|_____|__| |_____|   __|   __|_____|__|  
**                                              |_____|                      |__|  |__|
************************************************************************************************************/

#define WIN32_LEAN_AND_MEAN                 // reduce exe size
//                                                       __        
#include <windows.h>                        // .--.--.--|__|-----.
#include <commctrl.h>                       // |  |  |  |  |     |
#pragma  comment(lib, "comctl32.lib")       // |________|__|__|__|
//                                                    __   __ 
#include <sstream>                          // .-----|  |_|  |
#include <string>                           // |__ --|   _|  |
using namespace std;                        // |_____|____|__|

#include "statics.h"                        // static data
#include "resource.h"                       // list of resources
#include "lib/win/handler.h"                // класс handler для манипулирования окном / incapsulates HWND
#include "lib/win/wintop.h"                 // класс главного окна / main window

#define WINCLASS "ed_winlass"               // идентификатор кастомного класса окна / window id

/***********************************************************************************************************/
LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);

/***********************************************************************************************************/
int WINAPI WinMain(HINSTANCE hinst,         // место программы в памяти, Обычно: 0x00400000
                   HINSTANCE hprevinst,     // предыдущий instance, всегда NULL
                   char *    cmdparam,      // содержимое командной строки ( без fullpath )
                   int       cmdshow)       // состояние окна. значение которое отправляется в ShowWindow()
{
    InitCommonControls();

    // если найдена запущенная копия - сообщаем и шлем сообщение чтобы закрылось
    bool relaunched = false; handler hwnd = FindWindow(WINCLASS, NULL);
    if (hwnd) { hwnd.sendmessage(WM_CLOSE, 0, 0); relaunched = true; }

    // создаем прототип нашего главного окна
    wintop win(wndproc, WINCLASS, hinst, "", IDI_ICON1);

    // пытаемся зарегать и создать главное окно
    bool err = false;
    try
    {
        /* обратите внимание, что в тот момент, когда вы вызываете create, 
        ** наша оконная процедура вызывается с сообщением WM_CREATE */     
        win.reg();                          // регистрация класса окна
        win.create(-1, -1, 1, 1);           // запускает  окно
        win.hide();

        // если прога была перезапущена - маякнем из трея
        // if relaunched - show the balloon after 1 sec
        if (relaunched) win.sendmessage(MSG_BALLOON, 0, 1000);
    }
    catch (winexception e)
    {
        stringstream sstr; sstr << e.getmessage() << " : " << e.geterror();
        MessageBox(0, sstr.str().c_str(), "Exception", MB_ICONEXCLAMATION | MB_OK);
        err = true;
    }
    catch (...)
    {
        MessageBox(0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
        err = true;
    }
    if (err) return 0;

    // главный цикл по приему сообщений
    MSG msg; BOOL ret;
    while (ret = GetMessage(&msg, 0, 0, 0))
    {
        if (ret == -1) { return 0; } else if ( !IsDialogMessage(win.gethandle(), &msg) )
        {
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

/***********************************************************************************************************/





