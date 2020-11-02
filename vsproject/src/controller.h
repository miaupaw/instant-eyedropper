#if !defined controller_h
#define controller_h
/************************************************************************************************************ 
** controller
** инкапсуляция коллбэк процедуры обработки сообщений
**                   __              __ __            
** .----.-----.-----|  |_.----.-----|  |  |-----.----.
** |  __|  _  |     |   _|   _|  _  |  |  |  -__|   _|
** |____|_____|__|__|____|__| |_____|__|__|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "lib/updater/updater.h"        // sockets, need to include before including "windows.h"
#include "lib/win/handler.h"            // класс handler для манипулирования окном
#include "lib/win/gwl.h"                // Getting and Setting WindowLong: default is GWL_USERDATA
#include "lib/winex.h"                  // исключения
#include "lib/color.h"                  // манипулятор цветом
#include "resource.h"                   // resource
#include "base.h"                       // M of MVC for store and manage data
#include "glasscatcher.h"               // система ниппель
#include "eyedropper.h"                 // окошечко
#include "options.h"                    // редактор опций
#include "delaymessage.h"               // динамическое сообщение на основе activeobject

/***********************************************************************************************************/
/* обработчик сообщений основного окна, рулит всем */
class controller
{
public:
    controller(HWND hwnd, CREATESTRUCT * pcrt);
   ~controller();
    
    /* системные обработчики */
    bool            close(void);
    void            paint(void);
    void            parentnotify(uint, uint, handler);
    void            hotkey(int, uint);
    bool            command(uint, uint, handler);

    /* кастомные обработчики */
    void            popupmenu(void);
    void            itiscolor(COLORREF);
    void            balloon(uint);
    void            registerhotkey(uint, uint);
    void            update(handler);

private:
    /* внутренние методы */
    bool            settray(bool);
    void            options(HWND, int);
    BOOL            CopyTextToClipboard(LPCSTR);

private:
    handler         _hwnd;              // хэндлер главного окна
    HINSTANCE       _hinst;             // экземпляр

    base            _base;              // store and manage data
    eyedropper      _eyedropper;        // само окошко
    glasscatcher    _glass;             // прозрачный экран и по совместильству drag catcher

    handler         _options;           // handle/flag окна настроек

    NOTIFYICONDATA  _nid;               // структура для иконки в трее
    BOOL            _systrayicon;       // наличие или отсутсвие иконки в трее
    BOOL            _balloonflag;       // если балон показывался
    delaymessage    _message;           // посылает сообщение из треда

    updater         _updater;           // система апдейтинга
};

/***********************************************************************************************************/
#endif
