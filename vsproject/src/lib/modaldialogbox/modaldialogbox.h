#if !defined modaldialogbox_h
#define modaldialogbox_h
/************************************************************************************************************ 
** modal dialog box запускает всю шарманку
**                    __       __     __ __       __              __                
** .--------.-----.--|  |-----|  |.--|  |__|-----|  |-----.-----.|  |--.-----.--.--.
** |        |  _  |  _  |  _  |  ||  _  |  |  _  |  |  _  |  _  ||  _  |  _  |_   _|
** |__|__|__|_____|_____|___._|__||_____|__|___._|__|_____|___  ||_____|_____|__.__|
**                                                        |_____|              
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>
#include "controllerfactory.h"

#define resource MAKEINTRESOURCE

/***********************************************************************************************************/
class modaldialogbox
{
public:
    // creates modal dialog window
    modaldialogbox(HINSTANCE hinst, HWND hwnd, int dlgresource, ctrlfactory * cf)
    {
        _r = DialogBoxParam(hinst, resource(dlgresource), hwnd, (DLGPROC)modaldialogboxproc, (LPARAM)cf);
    }

    // ok or not depends on how EndDialog in controller calls
    bool isok() const { return (_r == -1) ? false : _r != 0; }

    // declaration of callback window procedure
    static BOOL CALLBACK modaldialogboxproc(HWND, UINT, WPARAM, LPARAM);

private:
    int _r;
};

/***********************************************************************************************************/
#endif
