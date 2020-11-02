#if !defined winbase_h
#define winbase_h
/************************************************************************************************************
** winbase
** базовыЙ класс для создания окна
**           __       __                      
** .--.--.--|__|-----|  |--.---.-.-----.-----.
** |  |  |  |  |     |  _  |  _  |__ --|  -__|
** |________|__|__|__|_____|___._|_____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "handler.h"            // класс handler для манимулирования окном
#include "../winex.h"

/***********************************************************************************************************/

/* инкапсулирует WNDCLASSEX 
** устанавливает его переменные по умолчанию 
** регистрирует класс 
** инициализирует дефолные переменные для CreateWindowEx
** запускает окно */
class winbase: public WNDCLASSEX
{
public:
    /* конструктор (имя класса задается явно) */
    winbase(WNDPROC wndproc, char const * classname, HINSTANCE hinst)
    :   _hwnd         (0),                      // инициализируется хэндл
        _exstyle      (0),                      // extended window style
        _classname    (classname),              // pointer to registered class name
        _windowname   (0),                      // pointer to window name
        _style        (WS_OVERLAPPED),          // window style
        _x            (CW_USEDEFAULT),          // horizontal position of window
        _y            (0),                      // vertical position of window
        _width        (CW_USEDEFAULT),          // window width  
        _height       (0),                      // window height
        _hwndparent   (0),                      // handle to parent or owner window
        _hmenu        (0),                      // handle to menu, or child-window identifier
        _hinstance    (hinst),                  // handle to application instance
        _data         (0)                       // pointer to window-creation data
    {
	    setdefaults(wndproc, hinst);            // устанавливает значания структуры WNDCLASSEX по умолчанию
        lpszClassName = classname;              // устанавливает имя класса в структуре WNDCLASSEX
    }
    
    /* установка бэкграунда для WNDCLASSEX */
    void setbgsyscolor(int syscolor = COLOR_WINDOW) { hbrBackground = (HBRUSH)(syscolor + 1); }

    /* установка курсора для WNDCLASSEX */
    void setsyscursor(char const * id) { hCursor = LoadCursor(0, id); }

    /* регистрирует класс */
    void reg()
    {
        WNDCLASSEX wcex;
        if (!GetClassInfoEx(_hinstance, _classname, &wcex)) // если еще нет
        if (RegisterClassEx(this) == 0) throw "Internal error: RegisterClassEx failed.";
    }

    /* запускает окно (передает все установленные параметры в функцию запуска) */
    HWND create() { return create(_x, _y, _width, _height, _data); }

    /* перегруженный метод, сразу устанавливает габариты */
    HWND create(int x, int y, int width, int height, LPVOID data = 0)
    {
        _x      = x;
        _y      = y;
        _width  = width;
        _height = height;
        _data   = data;

        _hwnd = CreateWindowEx(_exstyle,
                               _classname,
                               _windowname,
                               _style,
                               _x,
                               _y,
                               _width,
                               _height,
                               _hwndparent,
                               _hmenu,
                               _hinstance,
                               _data);

        if (_hwnd == 0) throw winexception("Internal error: Window Creation Failed.");
        return _hwnd;
    }

    /* показывает окно (в методе используются методы модного хэндла из hwnd.h) */
    void show(int ncmdshow = SW_SHOWNORMAL)
    {
        _hwnd.show(ncmdshow);
        _hwnd.update();
    }

    /* спрятать окно */
    void hide() { _hwnd.hide(); }

    /* отдает хэндл */
    handler gethandle() { return _hwnd; }

    /* шлет мессагу себе */
    LRESULT sendmessage(UINT msg, WPARAM wparam, LPARAM lparam) { return _hwnd.sendmessage(msg, wparam, lparam); }

private:
    /* устанавливает значания структуры WNDCLASSEX по умолчанию */
    void setdefaults(WNDPROC wndproc, HINSTANCE hinst)
    {
        /* provide reasonable default values */
        cbSize          = sizeof(WNDCLASSEX);           // размер структуры
        style           = 0;                            // стиль класса
        lpfnWndProc     = wndproc;                      // имя калбэк функции
        cbClsExtra      = 0;                            // дополнительные байты в для класса
        cbWndExtra      = 0;                            // дополнительные байты в для приложения?
        hInstance       = hinst;                        // экземпляр программы
        hIcon           = 0;                            // иконка 
        hCursor         = LoadCursor(0, IDC_ARROW);     // курсор
        hbrBackground   = (HBRUSH)(COLOR_WINDOW);       // бэкграунд
        lpszMenuName    = 0;                            // указательна строку в ресурсах идентифицирующую меню
        lpszClassName   = 0;                            // имя класса (должно быть обязательно)(инициализирует конструктор)
        hIconSm         = 0;                            // маленькая иконка
    }

protected:
        handler         _hwnd;                          // модный хэндл для этого окна
        
        /* для createwindowex */
        DWORD           _exstyle;                       // extended window style
        char const *    _classname;                     // pointer to registered class name
        char const *    _windowname;                    // pointer to window name
        DWORD           _style;                         // window style
        int             _x;                             // horizontal position of window
        int             _y;                             // vertical position of window
        int             _width;                         // window width  
        int             _height;                        // window height
        handler         _hwndparent;                    // handle to parent or owner window
        HMENU           _hmenu;                         // handle to menu, or child-window identifier
        HINSTANCE       _hinstance;                     // handle to application instance
        void *          _data;                          // pointer to window-creation data
};

/***********************************************************************************************************/
#endif