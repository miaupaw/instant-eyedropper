/************************************************************************************************************ 
**                       __ __                    
** .--------.-----.-----|__|  |_.-----.----.-----.
** |        |  _  |     |  |   _|  _  |   _|__ --|
** |__|__|__|_____|__|__|__|____|_____|__| |_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "monitors.h"

/***********************************************************************************************************/
/* setup */
monitors::monitors() : _ok (false) {}

/***********************************************************************************************************/
/* callback procedure: handle, NULL, NULL, (LPARAM)(monitors *) */
BOOL CALLBACK monitors::monproc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data)
{
    monitors * self = (monitors *)data;
    return self->monproc(monitor);
}

/***********************************************************************************************************/
/* gather monitors data */
bool monitors::monproc(HMONITOR monitor)
{
    MONITORINFOEX info;
    info.cbSize = sizeof(info);
    
    // if failed, clear, reset flag and stop
    if (!GetMonitorInfo(monitor, (LPMONITORINFO)&info)) { _monitors.clear(); _ok = false; return false; }
    
    // store to vector
    _monitors.push_back(info);

    // continue enumeration
    return true;
}

/***********************************************************************************************************/
/* collect monitors, also should be re-initialized on WM_DISPLAYCHANGE system event */
bool monitors::initialize()
{
    // типа все ок до первой ошибки
    _ok = true; 

    // clear all storages
    _monitors.clear(); _frames.clear();

    // enumerate monitors
    if (!EnumDisplayMonitors(NULL, NULL, monproc, (LPARAM)this)) { _ok = false; return false; }

    // gather all frames
    for (auto m : _monitors) _frames.push_back(m.rcMonitor);

    // fine
    return _ok;
}

/***********************************************************************************************************/
/* ready or not */
bool monitors::ok() { if (_ok && !_monitors.empty()) return true; else return false; }

/***********************************************************************************************************/
/* converts frame coordinates */
bool monitors::screentovirtual(frame & f)
{
    if (!screentovirtual((point &)f.left))  return false;
    if (!screentovirtual((point &)f.right)) return false;

    return true;
}

/***********************************************************************************************************/
/* converts point coordinates */
bool monitors::screentovirtual(point & p)
{
    // if fail
    if (!ok()) return false;

    // already in virtual coordinates. only one monitor
    if (_monitors.size() == 1) return true;

    // convert x
    int vpx = 0; for (auto & f : _frames) if (f.left <= vpx) vpx = f.left;
    p.x = -vpx + p.x;

    // convert y
    int vpy = 0; for (auto & f : _frames) if (f.top  <= vpy) vpy = f.top;
    p.y = -vpy + p.y;

    return true;
}

/***********************************************************************************************************/
/* converts frame coordinates */
bool monitors::virtualtoscreen(frame & f)
{
    if (!virtualtoscreen((point &)f.left))  return false;
    if (!virtualtoscreen((point &)f.right)) return false;

    return true;
}

/***********************************************************************************************************/
/* converts point coordinates */
bool monitors::virtualtoscreen(point & p)
{
    // if fail
    if (!ok()) return false;

    // already in virtual coordinates. only one monitor
    if (_monitors.size() == 1) return true;

    // shift back. wrong. didn't work. doesn't need
    p.x = _monitors[0].rcMonitor.left + (p.x - _frames[0].left);
    p.y = _monitors[0].rcMonitor.top  + (p.y - _frames[0].top);

    return true;
}

/***********************************************************************************************************/
/* calculate far edge in multiple monitor systems */
bool monitors::edge(point & p, direction where)
{
    // if fail
    if (!ok()) return false;

    // find the frame that contains pt
    frame * foundframe = 0;

    // scan frames
    for (auto & frame : _frames) { if (frame.PtInRect(p)) foundframe = &frame; }

    // point outside of any frame
    if (!foundframe) return false;

    // current edge
    int edge; switch (where)
    {
        case L: edge = foundframe->left; break;
        case T: edge = foundframe->top; break;
        case R: edge = foundframe->right; break;
        case B: edge = foundframe->bottom; break;
    }

    // scan frames, calculate the far edge
    for (auto & f : _frames)
    {
        switch (where)
        {
        case L: if (f.PtInRect({edge - 1, p.y})) edge = f.left; break;
        case T: if (f.PtInRect({p.x, edge - 1})) edge = f.top; break;
        case R: if (f.PtInRect({edge + 1, p.y})) edge = f.right; break;
        case B: if (f.PtInRect({p.x, edge + 1})) edge = f.bottom; break;
        }
    }

    // store value
    switch (where)
    {
        case L: p.x = edge; break;
        case T: p.y = edge; break;
        case R: p.x = edge - 1; break;
        case B: p.y = edge - 1; break;
    }

    return true;
}

/***********************************************************************************************************/