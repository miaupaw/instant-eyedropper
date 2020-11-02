/************************************************************************************************************ 
** modal dialog box window procedure incapsulated into modaldialogboxcontroller
**                    __       __     __ __       __              __                
** .--------.-----.--|  |-----|  |.--|  |__|-----|  |-----.-----.|  |--.-----.--.--.
** |        |  _  |  _  |  _  |  ||  _  |  |  _  |  |  _  |  _  ||  _  |  _  |_   _|
** |__|__|__|_____|_____|___._|__||_____|__|___._|__|_____|___  ||_____|_____|__.__|
**                                                        |_____|              
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "modaldialogbox.h"
#include "../win/gwl.h"

/***********************************************************************************************************/

// modal dialog box procedure. process messages from dialog box
// by passing them to client-defined controller object
BOOL CALLBACK modaldialogbox::modaldialogboxproc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
{
    // при первом вызове здесь будет мусор
    // а так, восстанавливаем указатель на контроллер при каждом вызове процедуры
    modaldialogboxcontroller * ctrl = getwindownlong<modaldialogboxcontroller*>(hwnd);

    switch (message)
    {
    case WM_INITDIALOG:
        {
            // добывает указатель на уже созданную фабрику,
            // который передается сюда во время создания окна
            ctrlfactory * ctrlfctry = reinterpret_cast<ctrlfactory*>(lp);

            // создает контроллер по шаблону
            ctrl = ctrlfctry->makectrl(hwnd);   

            // сохраняем указатель на созданный контроллер в винде
            setwindownlong<modaldialogboxcontroller*>(hwnd, ctrl);

            // можем вызывать методы
            ctrl->initialization();
        }
        return TRUE;
    
    case WM_PAINT:          if (ctrl) return ctrl->paint(); break;
    case WM_DRAWITEM:       if (ctrl) return ctrl->drawitem(reinterpret_cast<LPDRAWITEMSTRUCT>(lp)); break; // redraws ownerdraws
    case WM_THEMECHANGED:   if (ctrl) return ctrl->themechanged(); break;

    case WM_CTLCOLORSTATIC: if (ctrl) return ctrl->ctlcolorstatic((HDC)wp, (HWND)lp); break;
    case WM_CTLCOLORBTN:    if (ctrl) return ctrl->ctlcolorbutton((HDC)wp, (HWND)lp); break;

    case WM_MOUSEMOVE:      if (ctrl && ctrl->mousemove((int)LOWORD(lp), (int)HIWORD(lp))) return TRUE; break;
    case WM_LBUTTONDOWN:    if (ctrl && ctrl->lbuttondn((int)LOWORD(lp), (int)HIWORD(lp))) return TRUE; break;
    case WM_LBUTTONUP:      if (ctrl && ctrl->lbuttonup((int)LOWORD(lp), (int)HIWORD(lp))) return TRUE; break;

    case WM_COMMAND:        if (ctrl && ctrl->oncommand(LOWORD(wp), HIWORD(wp))) return TRUE; break;
    case WM_NOTIFY:         if (ctrl && ctrl->onnotify(wp, reinterpret_cast<NMHDR*>(lp))) return TRUE; break;

    case WM_DESTROY:        delete ctrl; setwindownlong<modaldialogboxcontroller*>(hwnd, 0); break;
    }

    // Typically, the dialog box procedure should return TRUE if it processed the message, 
    // and FALSE if it did not. If the dialog box procedure returns FALSE, 
    // the dialog manager performs the default dialog operation in response to the message.
    //
    // The following messages are exceptions to the general rules stated above. 
    // Consult the documentation for the specific message for details on the semantics of the return value.
    //
    // WM_CHARTOITEM
    // WM_COMPAREITEM
    // WM_CTLCOLORBTN
    // WM_CTLCOLORDLG
    // WM_CTLCOLOREDIT
    // WM_CTLCOLORLISTBOX
    // WM_CTLCOLORSCROLLBAR
    // WM_CTLCOLORSTATIC
    // WM_INITDIALOG
    // WM_QUERYDRAGICON
    // WM_VKEYTOITEM
    return FALSE;
}

/***********************************************************************************************************/