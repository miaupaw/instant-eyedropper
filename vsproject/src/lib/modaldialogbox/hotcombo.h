#if !defined hotcombo_h
#define hotcombo_h
/************************************************************************************************************ 
** combobox control for hotkeys
**  __          __                       __          
** |  |--.-----|  |_.----.-----.--------|  |--.-----.
** |     |  _  |   _|  __|  _  |        |  _  |  _  |
** |__|__|_____|____|____|_____|__|__|__|_____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "controls.h"
#include "../win/gwl.h"

/***********************************************************************************************************/
/* матрица правильных виртуалкеев */
struct vkey { UINT code; char caption[10]; };

/* основной набор виртуалкеев */
#define VKCOUNT sizeof virtualkeys / sizeof virtualkeys[0]
static vkey virtualkeys[] =
    {
        // поиск идет с конца к началу, и если ниего не найдено: это и есть дефолтное значение
        {0x00, ""}, 

        // common
        {0x70, "F1"},  {0x71, "F2"},  {0x72, "F3"},  {0x73, "F4"},  {0x74, "F5"},
        {0x75, "F6"},  {0x76, "F7"},  {0x77, "F8"},  {0x78, "F9"},  {0x79, "F10"},
        {0x7A, "F11"}, /*  system  */ {0x7C, "F13"}, {0x7D, "F14"}, {0x7E, "F15"},
        {0x7F, "F16"}, {0x80, "F17"}, {0x81, "F18"}, {0x82, "F19"}, {0x83, "F20"},
        {0x84, "F21"}, {0x85, "F22"}, {0x86, "F23"}, {0x87, "F24"},

        {0x30, "0"}, {0x31, "1"}, {0x32, "2"}, {0x33, "3"}, {0x34, "4"}, 
        {0x35, "5"}, {0x36, "6"}, {0x37, "7"}, {0x38, "8"}, {0x39, "9"}, 

        {0x41, "A"}, {0x42, "B"}, {0x43, "C"}, {0x44, "D"}, {0x45, "E"},
        {0x46, "F"}, {0x47, "G"}, {0x48, "H"}, {0x49, "I"}, {0x4A, "J"},
        {0x4B, "K"}, {0x4C, "L"}, {0x4D, "M"}, {0x4E, "N"}, {0x4F, "O"},
        {0x50, "P"}, {0x51, "Q"}, {0x52, "R"}, {0x53, "S"}, {0x54, "T"},
        {0x55, "U"}, {0x56, "V"}, {0x57, "W"}, {0x58, "X"}, {0x59, "Y"}, 
        {0x5A, "Z"},

        {0xBA, ":"}, {0xBB, "="}, {0xBC, ","}, {0xBD, "-"}, {0xBE, "."},
        {0xBF, "/"}, {0xC0, "~"}, {0xDB, "["}, {0xDC, "|"}, {0xDD, "]"},
        {0xDE, "'"}, {0xDF, "?"},

        {0x13, "Pause"}, {0x20, "Space"}, {0x29, "Sel"}, 
        {0x2A, "Print"}, {0x2B, "Exec"},  {0x2F, "Help"},

        {0x2D, "Ins"}, {0x24, "Home"}, {0x21, "PgUp"}, 
        {0x2E, "Del"}, {0x23, "End"},  {0x22, "PgDn"},

        // numpad
        {0x90, "NumL"}, {0x91, "Scroll"},

        {0x60, "Num0"}, {0x61, "Num1"}, {0x62, "Num2"}, {0x63, "Num3"}, {0x64, "Num4"},
        {0x65, "Num5"}, {0x66, "Num6"}, {0x67, "Num7"}, {0x68, "Num8"}, {0x69, "Num9"}, 

        {0x6F, "Div"}, {0x6A, "Mult"}, {0x6D, "Sub"}, {0x6B, "Add"}, {0x6E, "Dec"},
        {0x0C, "Clear"},

        // media
        {0xA6, "Back"},  {0xA7, "Fwrd"}, {0xA8, "Refr"}, {0xA9, "Stop"}, {0xAA, "Search"}, 
        {0xAB, "Stars"}, {0xAC, "Home"},

        {0xAD, "Mute"},  {0xAE, "Vdwn"},  {0xAF, "Vup"},  {0xB0, "Next"}, {0xB1, "Prev"}, 
        {0xB2, "Stop"},  {0xB3, "Pause"}, {0xB4, "Mail"}, {0xB5, "Media"}, 
        
        {0xB6, "App1"}, {0xB7, "App2"}
    };

// дополнительный набор виртуалкеев
#define ADDVKCOUNT sizeof addvkeys / sizeof addvkeys[0]
static vkey addvkeys[] =
    {   {0x00, ""}, {0x09, "Tab"}, {0x0D, "Enter"}, {0x5D, "Menu"} };

/***********************************************************************************************************/
/* сабклассинг комбо */
typedef struct tagHOTCOMBODATA
{
    UINT        id;         // id
    handler     hcomb;      // combobox handler
    handler     hedit;      // editbox handler
    handler     hlist;      // list handler
    vkey *      vk;         // virtual key pair (code, caption)
    bool        enabled;    // flag

} HOTCOMBODATA, *LPHOTCOMBODATA;

// кастомные команды CREATE
const UINT MSG_RSCB_LIB  = WM_USER      + 0x6000; // Reserved
const UINT MSG_HC_CREATE = MSG_RSCB_LIB + 100;    // hot combo create
const UINT MSG_CE_CREATE = MSG_RSCB_LIB + 100;    // combo edit create
const UINT MSG_CL_CREATE = MSG_RSCB_LIB + 100;    // combo list create

// декларация прототипов виндовспроцедур
LRESULT CALLBACK hotcomboproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK combeditproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK comblistproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

/* тип данных hotcombo */
class hotcombo: public simplecontrol
{
public:
    hotcombo(HWND hwndparent, int id) : simplecontrol (hwndparent, id)
    {
        InitCommonControls();

        // combo box items
        PCOMBOBOXINFO cbi = new COMBOBOXINFO(); cbi->cbSize = sizeof(COMBOBOXINFO);
        GetComboBoxInfo(_hwnd, cbi);

        HWND hcomb = cbi->hwndCombo;
        HWND hedit = cbi->hwndItem;
        HWND hlist = cbi->hwndList;

        // данные которые будут доступны всем
        _hotcombodata = new HOTCOMBODATA[1];
        _hotcombodata->id       = id;
        _hotcombodata->hcomb    = hcomb;
        _hotcombodata->hedit    = hedit;
        _hotcombodata->hlist    = hlist;
        _hotcombodata->vk       = &virtualkeys[0];

        // add advanced virtual keys
        int count = ADDVKCOUNT;
        for (int i = 0; i < ADDVKCOUNT; i++) add(addvkeys[i].caption);

        // выбираем значение по умолчанию
        select(0);

        // сабклассит комбобокс и передает указатель на структуру данных
        if (IsWindow(hcomb) && IsWindow(hedit) && IsWindow(hlist)) 
        {
            // сам комбобокс
            WNDPROC phcorigproc = (WNDPROC)(DWORD_PTR)SetWindowLong(hcomb, GWL_WNDPROC, (LONG)(DWORD_PTR)hotcomboproc);
            SendMessage(hcomb, MSG_HC_CREATE, (WPARAM)phcorigproc, (LPARAM)_hotcombodata);

            // эдитконтрол в комбобоксе
            WNDPROC pceorigproc = (WNDPROC)(DWORD_PTR)SetWindowLong(hedit, GWL_WNDPROC, (LONG)(DWORD_PTR)combeditproc);
            SendMessage(hedit, MSG_CE_CREATE, (WPARAM)pceorigproc, (LPARAM)_hotcombodata);

            // лист в комбобоксе
            WNDPROC pclorigproc = (WNDPROC)(DWORD_PTR)SetWindowLong(hlist, GWL_WNDPROC, (LONG)(DWORD_PTR)comblistproc);
            SendMessage(hlist, MSG_CL_CREATE, (WPARAM)pclorigproc, (LPARAM)_hotcombodata);
        }

        delete[] cbi;
    }
   ~hotcombo() { delete[] _hotcombodata; }

    void    clear()             {        _hwnd.sendmessage(CB_RESETCONTENT, 0,             0);           }
    void    add(char * str)     {        _hwnd.sendmessage(CB_ADDSTRING,    0,             (LPARAM)str); }
    void    select(int index)   {        _hwnd.sendmessage(CB_SETCURSEL,    (WPARAM)index, 0);           }
    int     currentindex()      { return _hwnd.sendmessage(CB_GETCURSEL,    0,             0);           }
    vkey *  getvkey()           { return _hotcombodata->vk; }
    UINT    getvkeycode()       { return _hotcombodata->vk->code; }

    // установка значения комбобокса в зависимости от кода присланного виртуалкея
    void    setup(UINT vk) 
    {
        // ищем в основном списке
        int count = VKCOUNT;
        while (count) { count--; if (vk == virtualkeys[count].code) break; }
        _hotcombodata->vk = &virtualkeys[count];
        _hwnd.settext(virtualkeys[count].caption);
        
        // если не нашли в основном - ищем в дополнительном
        if (count == 0) 
        {
            count = ADDVKCOUNT;
            while (count) { count--; if (vk == addvkeys[count].code) break; }
            _hotcombodata->vk = &addvkeys[count];
            _hwnd.settext(addvkeys[count].caption);
        }

        // если, до сих пор не нашли, но значение в пределах 0x00-0xFE
        // то все равно установим его, отобразив как "???"
        if (count == 0 && 0x00 < vk && vk < 0xFF)
        {
            static vkey key = {vk, "???"};
            _hotcombodata->vk = &key;
            _hwnd.settext(key.caption);
        }
    }

    //bool hasfocus() { return _hwnd.hasfocus(); }
    //void disable() { _editboxdata->enabled = false; _hwnd.sendmessage(EM_SETREADONLY, TRUE, 0); }
private:
    LPHOTCOMBODATA  _hotcombodata;
};

/***********************************************************************************************************/
// обработчик сообщений edконтрола
class hotcombocontroller
{
public:
    hotcombocontroller(HWND, LPHOTCOMBODATA);
   ~hotcombocontroller();

    /* родные мессаги */
    bool                command(HWND, UINT, UINT);
    void                edlbuttondown();
    bool                edcommand(HWND, UINT, UINT);
    void                edsetcursor();
    bool                edsyskeydown(UINT, UINT);
    void                edkeydown(UINT, UINT);

private:
    vkey *              getvkeydata(UINT);
    vkey *              getaddvkeydata(UINT);

private:
    handler             _hwnd;              // личный хендлер
    handler             _hwndparent;        // хэндл родителя
    LPHOTCOMBODATA      _hotcombodata;      // общие данные для класса и контроллера
};

/***********************************************************************************************************/
#endif
