#if !defined monitors_h
#define monitors_h
/************************************************************************************************************ 
**                       __ __                    
** .--------.-----.-----|__|  |_.-----.----.-----.
** |        |  _  |     |  |   _|  _  |   _|__ --|
** |__|__|__|_____|__|__|__|____|_____|__| |_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <vector>
using namespace std;

#include <windows.h>
#include "atltypes.h"   // для CRect и CPoint

typedef CRect  frame;
typedef CPoint point;
typedef enum {L, T, R, B} direction;

/***********************************************************************************************************/
class monitors 
{
public:
    monitors();

    // proc and proc caller
    static BOOL CALLBACK monproc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data);
    bool monproc(HMONITOR monitor);

    // init
    bool initialize();
    bool ok();

    // main methods
    bool screentovirtual(frame & f);
    bool screentovirtual(point & p);

    bool virtualtoscreen(frame & f);
    bool virtualtoscreen(point & p);

    bool edge(point & p, direction where);

private:
    vector<MONITORINFOEX>   _monitors;
    vector<frame>           _frames;
    bool                    _ok;
};

/***********************************************************************************************************/
#endif