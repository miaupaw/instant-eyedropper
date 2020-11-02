/************************************************************************************************************ 
** combobox control for hotkeys
**  __          __                       __          
** |  |--.-----|  |_.----.-----.--------|  |--.-----.
** |     |  _  |   _|  __|  _  |        |  _  |  _  |
** |__|__|_____|____|____|_____|__|__|__|_____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "hotcombo.h"

/***********************************************************************************************************/
static WNDPROC original_hotcombo_proc = NULL; // для указателя на оригинальный proc
static WNDPROC original_combedit_proc = NULL; // для указателя на оригинальный proc
static WNDPROC original_comblist_proc = NULL; // для указателя на оригинальный proc

/***********************************************************************************************************/
/* combo combo*/
LRESULT CALLBACK hotcomboproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    /* вытаскивает указатель из винды */
    hotcombocontroller * pctrl = getwindownlong<hotcombocontroller*>(hwnd);

    switch (message)
    {
    // ... также прислали указатель на структуру данных
    case MSG_HC_CREATE:
        original_hotcombo_proc = (WNDPROC)wparam;
        pctrl = new hotcombocontroller(hwnd, (LPHOTCOMBODATA)lparam);
        setwindownlong<hotcombocontroller*>(hwnd, pctrl);
        return 0;

    // дестрой!
    case WM_DESTROY:
        setwindownlong<hotcombocontroller*>(hwnd, 0);
        delete pctrl;
        break;

    // команда
    case WM_COMMAND:
        if (pctrl && pctrl->command(hwnd, LOWORD(wparam), HIWORD(wparam))) return 0;
        break;
    }
    return CallWindowProc(original_hotcombo_proc, hwnd, message, wparam, lparam);
}

/***********************************************************************************************************/
/* combo edit */
LRESULT CALLBACK combeditproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    /* вытаскивает указатель из винды */
    hotcombocontroller * pctrl = getwindownlong<hotcombocontroller*>(hwnd);

    switch (message)
    {
    // ... также прислали указатель на структуру данных
    case MSG_CE_CREATE:
        original_combedit_proc = (WNDPROC)wparam;
        pctrl = new hotcombocontroller(hwnd, (LPHOTCOMBODATA)lparam);
        setwindownlong<hotcombocontroller*>(hwnd, pctrl);
        return 0;

    // дестрой!
    case WM_DESTROY:
        setwindownlong<hotcombocontroller*>(hwnd, 0);
        delete pctrl;
        break;

    // предотвращает выделение текста
    //case EM_SETSEL:
    //    return 0;

    // предотвращает выделение текста по даблклику
    case WM_LBUTTONDBLCLK:
        return 0;

    // предотваращает вызов менюшки, она нам не нужна
    case WM_RBUTTONUP:
        return 0;

    // предотвращает ввод текста обычным способом
    // но обычные кнопки чот долго обрабатываются, при вводе - система шлет дофига этих мессаг
    case WM_GETDLGCODE:
        return 0;

    // гыгы, эврика ) предотвращает автокомплит в эдитбоксе из выпадающего списка
    case WM_GETTEXT:
        return 0;

    // предотвращает появление контекстного меню, например по VK_APPS
    case WM_CONTEXTMENU:
        return 0;

    // предотвращает обработку, чтобы каретка оставалась на месте
    // и перенаправляет клик комбобоксу, чтобы тот мог сфокусироваться
    case WM_LBUTTONDOWN:
        if (pctrl) pctrl->edlbuttondown();
        return 0;

    // команда
    case WM_COMMAND:
        if (pctrl) pctrl->edcommand(hwnd, LOWORD(wparam), HIWORD(wparam));
        return 0;

    // set cursor: установим стрелочку вместо палочки
    case WM_SETCURSOR:
        if (pctrl) pctrl->edsetcursor();
        // If an application processes this message, 
        // it should return TRUE to halt further processing or FALSE to continue. 
        return TRUE;

    // при нажатии F10 (в том числе) - приходит эта мессага
    case WM_SYSKEYDOWN:
        if (pctrl && pctrl->edsyskeydown(wparam, lparam)) return 0;
        break;

    // keydown
    case WM_KEYDOWN:
        if (pctrl) pctrl->edkeydown(wparam, lparam);
        return 0;

    }
    return CallWindowProc(original_combedit_proc, hwnd, message, wparam, lparam);
}

/***********************************************************************************************************/
/* combo list */
LRESULT CALLBACK comblistproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    /* вытаскивает указатель из винды */
    hotcombocontroller * pctrl = getwindownlong<hotcombocontroller*>(hwnd);

    switch (message)
    {
    // ... также прислали указатель на структуру данных
    case MSG_CL_CREATE:
        original_comblist_proc = (WNDPROC)wparam;
        pctrl = new hotcombocontroller(hwnd, (LPHOTCOMBODATA)lparam);
        setwindownlong<hotcombocontroller*>(hwnd, pctrl);
        return 0;

    // дестрой!
    case WM_DESTROY:
        setwindownlong<hotcombocontroller*>(hwnd, 0);
        delete pctrl;
        break;

    }
    return CallWindowProc(original_comblist_proc, hwnd, message, wparam, lparam);
}

/***********************************************************************************************************/
/* походу методы этого класса будут обрабатывать три различных экземпляра 
** которые создаются в виндовпроцедурах )) 
** (инициализирует внутренние переменные реальными значениями) */
hotcombocontroller::hotcombocontroller(HWND hwnd, LPHOTCOMBODATA data)
:   _hwnd           (hwnd),
    _hotcombodata   (data),
    _hwndparent     (GetParent(hwnd))

{}

/* деструктор */
hotcombocontroller::~hotcombocontroller() {}

/* combo control: команда */
bool hotcombocontroller::command(HWND hwnd, UINT ctrlid, UINT notifycode)
{
    int x = 0;
    if (_hotcombodata->id)
    {
        switch (notifycode)
        {
        // indicates the list in a drop-down combo box or drop-down list box is about to close. 
        //case CBN_CLOSEUP:
        //    x = 0;
        //    break;
        // indicates the user has double-clicked a list item in a simple combo box. 
        //case CBN_DBLCLK:
        //    x = 0;
        //    break;
        // indicates the list in a drop-down combo box or drop-down list box is about to open. 
        //case CBN_DROPDOWN:
        //    x = 0;
        //    break;
        // indicates the user has changed the text in the edit control of a simple or drop-down combo box. 
        // this notification message is sent after the altered text is displayed. 
        //case CBN_EDITCHANGE:
        //    x = 0;
        //    break;
        // indicates the user has changed the text in the edit control of a simple or drop-down combo box. 
        // this notification message is sent before the altered text is displayed. 
        //case CBN_EDITUPDATE:
        //    x = 0;
        //    break;
        // indicates the combo box cannot allocate enough memory to carry out a request, such as adding a list item. 
        //case CBN_ERRSPACE:
        //    x = 0;
        //    break;
        // indicates the combo box is about to lose the input focus.
        //case CBN_KILLFOCUS:
        //    x = 0;
        //    break;
        // indicates the current selection has changed.
        case CBN_SELCHANGE:
            // сохраняем нужный указатель
            _hotcombodata->vk = getaddvkeydata(_hwnd.sendmessage(CB_GETCURSEL, 0, 0));
            break;
        // indicates that the selection made in the drop down list, while it was dropped down, should be ignored.
        //case CBN_SELENDCANCEL:
        //    x = 0;
        //    break;
        // indicates that the selection made drop down list, while it was dropped down, should be accepted.
        //case CBN_SELENDOK:
        //    x = 0;
        //    break;
        // indicates the combo box has received the input focus. 
        //case CBN_SETFOCUS:
        //    x = 0;
        //    break;
        //case EN_SETFOCUS:
        //    SendMessage(_hotcombodata->hedit, EM_SETSEL, -1, 0);
        //    HideCaret(_hotcombodata->hedit);
        //
        //    Create a solid black caret. 
        //    CreateCaret(_hotcombodata->hedit, (HBITMAP) NULL, 1, 15); 
        //
        //    Adjust the caret position, in client coordinates. 
        //    SetCaretPos(50, 2); 
        //
        //    Display the caret. 
        //    ShowCaret(_hotcombodata->hedit); 
        //    return true;
        }
    }
    return false;
}

/* editcontrol: нажали левую кнопку */
void hotcombocontroller::edlbuttondown()
{
    // перенаправляем клик комбобоксу, чтобы тот мог сфокусироваться
    SendMessage(_hotcombodata->hcomb, WM_LBUTTONDOWN, 0, 0);
    //SendMessage(_hotcombodata->hedit, EM_SETSEL, 1, 0);
    //ShowCaret(_hotcombodata->hedit);
}

/* editcontrol: команда от(для) эдитконтрола */
bool hotcombocontroller::edcommand(HWND hwnd, UINT ctrlid, UINT notifycode)
{
    if (_hotcombodata->id)
    {
        //switch (notifycode)
        //{
        //case EN_SETFOCUS:
        //    SendMessage(_hotcombodata->hedit, EM_SETSEL, -1, 0);
        //    break;
        //case EN_CHANGE:
        //    break;
        //}
    }
    return false;
}

/* editecontrol: set cursor */
void hotcombocontroller::edsetcursor()
{
    // установим стрелочку вместо палочки
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

/* editcontrol: при нажатии F10 (в том числе) - приходит эта мессага */
bool hotcombocontroller::edsyskeydown(UINT virtualkey, UINT flags)
{
    // перенаправляем мессагу
    if (virtualkey == VK_F10) { edkeydown(VK_F10, 0); return true; }
    return false;
}

/* editecontrol: keydown */
void hotcombocontroller::edkeydown(UINT virtualkey, UINT flags)
{
    // принимаем только известный нам virtualkey
    _hotcombodata->vk = getvkeydata(virtualkey);

    // отображаем
    _hotcombodata->hedit.settext(_hotcombodata->vk->caption);
    _hotcombodata->hedit.hidecaret();
}

/* приватные методы ****************************************************************************************/
/* editecontrol: keydown */
vkey * hotcombocontroller::getvkeydata(UINT virtualkey)
{
    // сканируем матрицу ключей в обратном порядке
    // если нужный не был найден - возвращается нулевой индекс (дефолтное значение)
    int count = VKCOUNT;
    while (count) { count--; if (virtualkey == virtualkeys[count].code) break; }
    return &virtualkeys[count];
}

/* combocontrol: select changed */
vkey * hotcombocontroller::getaddvkeydata(UINT index)
{
    // если индекс в нужных пределах возвращаем указатель на нужный vkey
    if (index < ADDVKCOUNT) return &addvkeys[index];

    // иначе возвращаем указатель на дефолтный vkey
    return &addvkeys[0];
}

/***********************************************************************************************************/