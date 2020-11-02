#ifndef _versionhelper_h
#define _versionhelper_h
/***********************************************************************************************************/

#define _WIN32_WINNT_WIN8 0x0602

/***********************************************************************************************************/
// if windows 8 or higher
inline bool win8plus()
{
    OSVERSIONINFOEXW osversioninfo = { sizeof(osversioninfo), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG  const conditionmask = VerSetConditionMask(
                                     VerSetConditionMask(
                                     VerSetConditionMask(
        0, VER_MAJORVERSION,     VER_GREATER_EQUAL),
           VER_MINORVERSION,     VER_GREATER_EQUAL),
           VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osversioninfo.dwMajorVersion    = HIBYTE(_WIN32_WINNT_WIN8);
    osversioninfo.dwMinorVersion    = LOBYTE(_WIN32_WINNT_WIN8);
    osversioninfo.wServicePackMajor = 0;

    return VerifyVersionInfoW(&osversioninfo, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, 
                              conditionmask) != FALSE;
}

/***********************************************************************************************************/
#endif
