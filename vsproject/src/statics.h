#if !defined statics_h
#define statics_h
/************************************************************************************************************
** statics
**        __         __   __             
** .-----|  |_.---.-|  |_|__|----.-----.
** |__ --|   _|  _  |   _|  |  __|__ --|
** |_____|____|___._|____|__|____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <windows.h>
#include <string>
#include <vector>
using namespace std;

typedef char *        lpstr;
typedef LPTSTR        lptstr;
typedef unsigned char byte;
typedef wchar_t       wide;
typedef unsigned int  uint;
typedef TCHAR         tchar;

#define new new 

/***********************************************************************************************************/
#define golden_section          1.606

#define SERVERNAME              "instant-eyedropper.com"
#define INI_NAME                "eyedropper"

#define LINK_HOME               "http://instant-eyedropper.com?ref=ie" vsfilestr
#define LINK_DONATE             "https://store.payproglobal.com/checkout?products[1][id]=11283"

#define WM_MYICONNOTIFY         WM_USER + 100   // мессага для обработки сообщений из трея
#define WM_MOUSEOUT             WM_USER + 110   // кастомный lparam для WM_MYICONNOTIFY

#define ID_HOTKEY               3333            // id для обработки WM_HOTKEY контроллером. 0x0000-0xBFFF
#define ID_ESCAPE               4444            // id для обработки WM_HOTKEY фризом

#define ID_FILE_DONATE          5003
#define ID_FILE_ABOUT           5004
#define ID_FILE_OPTIONS         5005
#define ID_FILE_EXIT            5006

#define ID_MODE_DRAG            101             // 1-click (drag)
#define ID_MODE_CLICK           102             // 2-click (click)

#define ID_FORMAT_HTML          0
#define ID_FORMAT_HEX           1
#define ID_FORMAT_DELPHI        2
#define ID_FORMAT_VB            3
#define ID_FORMAT_RGB           4
#define ID_FORMAT_RGBFLOAT      5
#define ID_FORMAT_HSV           6
#define ID_FORMAT_HSL           7
#define ID_FORMAT_LONG          8

#define MODE_DRAG               1
#define MODE_CLICK              2

#define PICK_START              2000            // флаг для вызова MSG_PROCEED
#define PICK_STOP               2001            // флаг для вызова MSG_PROCEED
#define PICK_CANCEL             2002            // флаг для вызова MSG_PROCEED

const UINT MSG_RS_LIBRARY       = WM_USER        + 0x4000;  // reserved
const UINT MSG_BALLOON          = MSG_RS_LIBRARY + 100;     //
const UINT MSG_POPUPMENU        = MSG_RS_LIBRARY + 200;     // 
const UINT MSG_STARTEYEDROPPER  = MSG_RS_LIBRARY + 400;     // 
const UINT MSG_PROCEED          = MSG_RS_LIBRARY + 500;     // контроллер шлет стеклу при обработке хоткея
const UINT MSG_COLOR            = MSG_RS_LIBRARY + 600;     //
const UINT MSG_UPDATE           = MSG_RS_LIBRARY + 700;     //

/***********************************************************************************************************/
// связь идентификаторов и темплейтов
struct colorformats { int format; string caption; vector<string> templates; };
static colorformats formats[] =
{
    {ID_FORMAT_HTML,        "HTML",       {"#(R)(G)(B)", "(R)(G)(B)", "#(r)(g)(b)", "(r)(g)(b)"}},
    {ID_FORMAT_HEX,         "Hex",        {"0x(B)(G)(R)", "0x(R)(G)(B)", "0x(b)(g)(r)", "0x(r)(g)(b)"}},
    {ID_FORMAT_DELPHI,      "Delphi Hex", {"$00(B)(G)(R)", "$00(b)(g)(r)"}},
    {ID_FORMAT_VB,          "VB Hex",     {"&H00(B)(G)(R)&"}},
    {ID_FORMAT_RGB,         "RGB",        {"(r), (g), (b)", "(r)\\n(g)\\n(b)", "(r)\\t(g)\\t(b)"}},
    {ID_FORMAT_RGBFLOAT,    "RGB Float",  {"(r), (g), (b)", "(r)\\n(g)\\n(b)", "(r)\\t(g)\\t(b)"}},
    {ID_FORMAT_HSV,         "HSV",        {"(h), (s), (v)", "(h)\\n(s)\\n(v)", "(h)\\t(s)\\t(v)"}},
    {ID_FORMAT_HSL,         "HSL",        {"(h), (s), (l)", "(h)\\n(s)\\n(l)", "(h)\\t(s)\\t(l)"}},
    {ID_FORMAT_LONG,        "Long",       {"(long)"}}};

#define FORMATSCOUNT sizeof formats / sizeof formats[0]

// список желаемых шрифтов в порядке приоритета
static string fontlist[] = {"Consolas", 
                            "Courier New", 
                            "Lucida Console",
                            "System"};

/***********************************************************************************************************/
#endif















