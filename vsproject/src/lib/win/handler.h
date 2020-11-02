#if !defined handler_h
#define handler_h
/************************************************************************************************************
** класс "модный хэндл"
** типа ведет себя как обычный хендл, но имееет всякие модные методы 
** для манипулирования владельцем хэндла. инкапсуляция йопт.
**  __                   __ __             
** |  |--.---.-.-----.--|  |  |-----.----.
** |     |  _  |     |  _  |  |  -__|   _|
** |__|__|___._|__|__|_____|__|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>
#include <commctrl.h>
#include <string>
using namespace std;

/***********************************************************************************************************/

class handler { public: handler(HWND hwnd = 0) : _hwnd (hwnd) { }
    
    /* перезагрузка операторов присваивания типов */
    operator    HWND()   const { return _hwnd; } 
    operator    WPARAM() const { return (WPARAM)_hwnd; } 

    /* мессаги */
    LRESULT     sendmessage(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) const 
                                                    { return SendMessage(_hwnd, msg, wparam, lparam); }

    bool        postmessage(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) const 
                                                    { return PostMessage(_hwnd, msg, wparam, lparam) != FALSE; }

    /* возвращает instance */
    HINSTANCE   getinstance() const                 { return reinterpret_cast<HINSTANCE>(GetWindowLongPtr(_hwnd, GWL_HINSTANCE)); }
	
    /* работа с предком */
    HWND        getparent() const                   { return GetParent(_hwnd); }
    void        setparent(HWND hwndParent)	        { SetParent(_hwnd, hwndParent); }
    
    /* и что же оно делает? */
    void        setfocus()                          { SetFocus(_hwnd); }
    bool        hasfocus() const                    { return GetFocus() == _hwnd; }
    
    /* знать бы еще почему я указал статик */
    void        capturemouse()                      { SetCapture(_hwnd); }
    static void releasemouse()	                    { ReleaseCapture(); }
    bool        hascapture() const                  { return GetCapture() == _hwnd;	}
    
    /* для эдитконтролов комбобоксов тоже работает */
    void        settext(char const * text)          { SetWindowText(_hwnd, text); }
    int         gettext(char * buf, int len) const  { return GetWindowText(_hwnd, buf, len); }
    string      gettext(int maxlen)          const  { char * txt = new char[maxlen];
                                                      GetWindowText(_hwnd, txt, maxlen); string objecttext = txt;
                                                      delete[] txt; return objecttext; }
    /* парам-пам-пам */
    void        show(int cmdShow = SW_SHOW)         { ShowWindow(_hwnd, cmdShow); }
    BOOL        hide()                              { return ShowWindow(_hwnd, SW_HIDE); }
    BOOL        foreground()                        { return SetForegroundWindow(_hwnd); }

    /* йопт */
    void        enable()                            { EnableWindow(_hwnd, TRUE); }
    void        disable()                           { EnableWindow(_hwnd, FALSE); }

    /* посылает WM_PAINT (if the window's update region is not empty) */
    void        update()                            { UpdateWindow(_hwnd); }
    
    /* делает инвалидом, осторожно */
    void        invalidate()                        { InvalidateRect(_hwnd, 0, TRUE); }
    void        invalidate(RECT const & rect)       { InvalidateRect(_hwnd, &rect, TRUE); }
    void        forcerepaint()                      { invalidate(); update(); }

    /* в абсолютных координатах */
    void        getclientrect(RECT & rect)          { GetClientRect(_hwnd, &rect); }
    void        getwindowrect(RECT & rect)          { GetWindowRect(_hwnd, &rect); }

    /* двигай */
    void        move(int x, int y, int w, int h)           { MoveWindow(_hwnd, x, y, w, h, TRUE); }
    void        movedelaypaint(int x, int y, int w, int h) { MoveWindow(_hwnd, x, y, w, h, FALSE); }

    HDC         getdc()                             { return GetDC(_hwnd); }

    /* вот это тема, прикольные закорючки */
    void        addexstyle(long style)              { SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) |  style); }
    void        removeexstyle(long style)           { SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) & ~style); }

    /* состояние окна */
    BOOL        visible()                           { return IsWindowVisible(_hwnd); }
    BOOL        hidecaret()                         { return HideCaret(_hwnd); }

    /* для модальных окон */
    BOOL        enddialog(bool value)               { return EndDialog(_hwnd, value);}
    void        tabselect(int index)                { TabCtrl_SetCurSel(_hwnd, index); }

private:
    HWND    _hwnd;
};

/***********************************************************************************************************/
#endif
