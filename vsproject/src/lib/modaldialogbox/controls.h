#if !defined controls_h
#define controls_h
/************************************************************************************************************ 
**                   __              __        
** .----.-----.-----|  |_.----.-----|  |-----.
** |  __|  _  |     |   _|   _|  _  |  |__ --|
** |____|_____|__|__|____|__| |_____|__|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>
#include <commctrl.h>
//                                                    __   __
#include <vector>                           // .-----|  |_|  |
#include <string>                           // |__ --|   _|  |
using namespace std;                        // |_____|____|__|

#include "../win/handler.h"

/***********************************************************************************************************/
class simplecontrol
{
public:
    simplecontrol(HWND hwndparent, int id) : _hwnd (GetDlgItem(hwndparent, id)) {}

    handler hwnd() const                { return _hwnd; }
    static BOOL isclicked(int code)     { return code == BN_CLICKED; } // code is the HIWORD (wParam)
    void setfocus()                     { SetFocus(_hwnd); }
    void enable()                       { EnableWindow(_hwnd, TRUE); }
    void disable()                      { EnableWindow(_hwnd, FALSE); }
    void hide()                         { ShowWindow (_hwnd, SW_HIDE); };
    void show()                         { ShowWindow (_hwnd, SW_SHOW); };
    void setfont(HFONT font)            { SendMessage(hwnd(), WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0)); }

protected:
    handler _hwnd;
};

/* наследники **********************************************************************************************/
/* манипулятор TAB-контролом, наследник simplecontrol */
class tab: public simplecontrol
{
public:
    tab(HWND hwndparent, int id) : simplecontrol (hwndparent, id) { _tabcount = 0; }

    bool add(char * str)
    {
        TCITEM item;
        item.mask        = TCIF_TEXT;
        item.iImage      = 0;
        item.lParam      = NULL;
        item.pszText     = str;
        item.dwState     = 0;
        item.dwStateMask = 0;
         
        if (SendMessage(hwnd(), TCM_INSERTITEM, _tabcount, (LPARAM)&item) == -1)
        {
            // ошибка
            // i-я закладка не добавилась
            return false;
        }

        _tabcount++;

        HFONT fnthwnd = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hwnd(), WM_SETFONT, (WPARAM)fnthwnd, (LPARAM)true);

        return true;
    }

    int count() { return _tabcount; }

private:
    int     _tabcount;
};

/* манипулятор edit-контролом, наследник simplecontrol */
class edit: public simplecontrol
{
public:
    edit(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}

    void setstring(char const * buf)     { SendMessage(hwnd(), WM_SETTEXT, 0, (LPARAM) buf); }
    static BOOL ischanged(int code)      { return code == EN_CHANGE; } // code is the HIWORD (wParam)
    int  getlen()                        { return SendMessage(hwnd(), WM_GETTEXTLENGTH, 0, 0); }
    void getstring(char * buf, int len)  { SendMessage(hwnd(), WM_GETTEXT, (WPARAM)len, (LPARAM)buf); }
    void select()                        { SendMessage(hwnd(), EM_SETSEL, 0, -1); }
};

/* манипулятор чекбоксом, наследник simplecontrol */
class checkbox: public simplecontrol   
{
public:
    checkbox(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}

    bool check()            
    {
        if (SendMessage(hwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED) return true;
        else return false;
    };

    void check(bool check)  
    { 
        if (check) SendMessage(hwnd(), BM_SETCHECK, BST_CHECKED, 0);
        else       SendMessage(hwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
    };

    void setstring(char const * buf) { SendMessage(hwnd(), WM_SETTEXT, 0, (LPARAM) buf); }
};


/* манипулятор радиокнопкой, наследник simplecontrol */
class radio: public simplecontrol   
{
public:
    radio(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}

    bool check()            
    {
        if (SendMessage(hwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED) return true;
        else return false;
    };

    void check(bool check)  
    { 
        if (check) SendMessage(hwnd(), BM_SETCHECK, BST_CHECKED,   0);
        else       SendMessage(hwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
    };
};

/* наследник simplecontrol */
class staticc: public simplecontrol   
{
public:
    staticc(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}

    bool settext(char const * buf) { SetWindowText(hwnd(), buf); return 0; }

    void setpic(HINSTANCE hinst, UINT id)
    {
        SendMessage(hwnd(),
                    STM_SETIMAGE,
                    IMAGE_BITMAP,
                    (LPARAM)LoadImage(hinst, 
                                      MAKEINTRESOURCE(id), 
                                      IMAGE_BITMAP, 
                                      0, 0, 
                                      LR_DEFAULTCOLOR));
    }
};

/* наследник simplecontrol */
class combo: public simplecontrol   
{
public:
    combo(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}

    void set(char const * text)     { _hwnd.settext(text); }
    void add(char * str)            { SendMessage(hwnd(), CB_ADDSTRING, 0, (LPARAM)str); }
    void add(const char * str)      { SendMessage(hwnd(), CB_ADDSTRING, 0, (LPARAM)str); }
    void select(int index)          { SendMessage(hwnd(), CB_SETCURSEL, (WPARAM)index, 0); }
    int  currentindex()             { return SendMessage(hwnd(), CB_GETCURSEL, 0, 0); }
    //_hwnd.sendmessage(CB_INSERTSTRING, 0, (LPARAM)str.c_str());

    void add(vector<string> values) { for (auto v : values) 
    {
        SendMessage(hwnd(), CB_ADDSTRING, 0, (LPARAM)v.c_str()); 
    }}
};

/* наследник simplecontrol */
class button: public simplecontrol   
{
public:
    button(HWND hwndparent, int id) : simplecontrol (hwndparent, id) {}
};

/***********************************************************************************************************/
#endif
