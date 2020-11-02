#if !defined gwl_h
#define gwl_h
/************************************************************************************************************ 
** getting and setting windowlong: default is GWL_USERDATA
**                 __ 
** .-----.--.--.--|  |
** |  _  |  |  |  |  |
** |___  |________|__|
** |_____|           
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>

/***********************************************************************************************************/

template<typename type> inline type getwindownlong(HWND hwnd, int which = GWL_USERDATA)
{
    return reinterpret_cast<type>(GetWindowLongPtr(hwnd, which));
}

template<typename type> inline void setwindownlong(HWND hwnd, type value, int which = GWL_USERDATA)
{
    SetWindowLongPtr(hwnd, which, reinterpret_cast<long>(value)); //!
}

/***********************************************************************************************************/
#endif