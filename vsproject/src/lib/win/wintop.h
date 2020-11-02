#if !defined (WINTOP_H)
#define WINTOP_H
/************************************************************************************************************
** wintop
** класс для создания главного окна
**           __       __               
** .--.--.--|__|-----|  |_.-----.-----.
** |  |  |  |  |     |   _|  _  |  _  |
** |________|__|__|__|____|_____|   __|
**                              |__|                                                  instant-eyedropper.com
************************************************************************************************************/

#include "winbase.h"                            // базовыЙ класс для создания окна

/***********************************************************************************************************/

class wintop: public winbase
{
public:
    wintop(WNDPROC      wndproc,                // имя процедуцры обработки сообщений
           char const * classname,              // имя класса
           HINSTANCE    hinst,                  // место программы в памяти
           char const * caption,                // заголовок окна
           int          res_id)                 // идентификатор иконки в ресурсах
    :   winbase(wndproc, classname, hinst)
    {
        /* прикручивает иконку из ресурсов */
        setresicons(res_id);

        _windowname   = caption;
        _exstyle      = WS_EX_LTRREADING |      // left to right 
                        WS_EX_STATICEDGE;       // three-dimensional border style 
        _style        = WS_OVERLAPPED |         // has a caption and a border
                        WS_BORDER |             // has a border 
                        WS_CAPTION |            // has a caption 
                        WS_SYSMENU |            // has a control-menu box in its title bar
                        //WS_MINIMIZEBOX | 
                        WS_CLIPCHILDREN;        // не обновляет области занятые дочерними окнами при wm_paint
    }

    /* установка иконок для WNDCLASSEX обычная и маленькая */
	void setresicons(int res_id)
    {
        /* устанавливаются соответсвуюжие переменные структуры WNDCLASSEX */
        hIcon = LoadIcon(_hinstance, MAKEINTRESOURCE(res_id));
        hIconSm = (HICON)LoadImage(_hinstance,
                                   MAKEINTRESOURCE(res_id), 
                                   IMAGE_ICON, 
                                   GetSystemMetrics(SM_CXSMICON),
                                   GetSystemMetrics(SM_CYSMICON),
                                   LR_SHARED);
    }
};

/***********************************************************************************************************/
#endif