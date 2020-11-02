#if !defined themed_h
#define themed_h
/************************************************************************************************************ 
** theme detector
**  __   __                              __       __              __              
** |  |_|  |--.-----.--------.-----. .--|  |-----|  |_.-----.----|  |_.-----.----.
** |   _|     |  -__|        |  -__| |  _  |  -__|   _|  -__|  __|   _|  _  |   _|
** |____|__|__|_____|__|__|__|_____| |_____|_____|____|_____|____|____|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <uxtheme.h>
#include <Shlwapi.h>
#include <tchar.h> // для _T
#pragma comment(lib, "UxTheme.lib")

#include "versionhelper.h"

/***********************************************************************************************************/

class themed
{
public:
    themed()
    {
        if (isthemed()) _isthemed = true;
        else            _isthemed = false;
    }
   ~themed() {}
    
    bool yep()    { return _isthemed; }
    void update() { _isthemed = isthemed(); }

protected:

    bool isthemed()
    {
        bool ret = false;
        if (!win8plus())
        {
            //Windows XP detected
            typedef BOOL WINAPI isappthemed();
            typedef BOOL WINAPI isthemeactive();

            isappthemed   * pisappthemed = NULL;
            isthemeactive * pisthemeactive = NULL;

            HMODULE hmod = LoadLibrary(_T("uxtheme.dll"));

            if (hmod)
            {
                pisappthemed   = reinterpret_cast<isappthemed*>(GetProcAddress(hmod,_T("IsAppThemed")));
                pisthemeactive = reinterpret_cast<isthemeactive*>(GetProcAddress(hmod,_T("IsThemeActive")));

                if (pisappthemed && pisthemeactive) { if (pisappthemed() && pisthemeactive()) {                

                    typedef HRESULT CALLBACK dllgetversion(DLLVERSIONINFO*);
                    dllgetversion * pdllgetversion = NULL;
                    HMODULE hmodcomctl = LoadLibrary(_T("comctl32.dll"));

                    if (hmodcomctl)
                    {
                        pdllgetversion = reinterpret_cast<dllgetversion*>(GetProcAddress(hmodcomctl,_T("DllGetVersion")));

                        if (pdllgetversion)
                        {
                            DLLVERSIONINFO dvi = {0}; dvi.cbSize = sizeof dvi;
                            if (pdllgetversion(&dvi) == NOERROR ) { ret = dvi.dwMajorVersion >= 6; }
                        }
                        FreeLibrary(hmodcomctl);                    
                    }
                }}

                FreeLibrary(hmod);
            }
        }    
        return ret;
    }

private:
    bool    _isthemed;
};

/***********************************************************************************************************/
#endif