#if !defined (winregistry)
#define winregistry
/************************************************************************************************************ 
** класс для манипулирования реестром
**                   __       __              
** .----.-----.----.|__|-----|  |_.----.--.--.
** |   _|  -__|  _  |  |__ --|   _|   _|  |  |
** |__| |_____|___  |__|_____|____|__| |___  |
**            |_____|                  |_____|                                        instant-eyedropper.com
************************************************************************************************************/
//#pragma warning(disable : 4996)

#include <windows.h>
#include <winerror.h>
#pragma comment(lib, "Advapi32.lib")

#include <string>
#include <sstream>
using namespace std;

/***********************************************************************************************************/
class registry
{
public:
    /* конструктор для работы в независимом режиме и для чтения значений */
    registry() : _rootkey (0), _path ("") {}

    /* конструктор для работы с предуствновленными параметрами */
    registry(HKEY rootkey, char * path) : _rootkey (rootkey), _path (path) {}

    /*************************************************************************************************/
    /* независимый метод чтения */
    LPTSTR queryvalue(HKEY hkey, char * path, char * valuename)
    {
        HKEY   key;
        DWORD  dwBufLen = MAX_PATH;
        LPTSTR value = new char[MAX_PATH]();

        if (RegOpenKeyEx(hkey, path, 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
        {
            LONG result = RegQueryValueEx(key, valuename, NULL, NULL, (LPBYTE)value, &dwBufLen);
            RegCloseKey(key);

            if ((result == ERROR_SUCCESS) && (value[0] != NULL)) return value;
        }

        return "";
    }

    /* чтение с использованием предуставновленных параметров */
    string queryvalue(char * valuename, string defaultvalue)
    {
        char * rv = queryvalue(_rootkey, _path, valuename);
        string str = rv;

        if (strcmp(rv, "") != 0) return str;
        else                     return defaultvalue;
    }

    LPTSTR queryvalue(char * valuename, char * defaultvalue)
    {
        char * rv = queryvalue(_rootkey, _path, valuename);
        if (strcmp(rv, "") != 0) return rv;
        else                     return defaultvalue;
    }

    int queryvalue(char * valuename, int defaultvalue)
    {
        char * rv = queryvalue(_rootkey, _path, valuename);
        if (strcmp(rv, "") != 0) return atoi(rv);
        else                     return defaultvalue;
    }

    uint queryvalue(char * valuename, uint defaultvalue)
    {
        char * rv = queryvalue(_rootkey, _path, valuename);
        if (strcmp(rv, "") != 0) return atoi(rv);
        else                     return defaultvalue;
    }

    bool queryvalue(char * valuename, bool defaultvalue)
    {
        char * rv = queryvalue(_rootkey, _path, valuename);
        if (strcmp(rv, "") != 0) return atoi(rv) != 0;
        else                     return defaultvalue;
    }

    /*************************************************************************************************/
    /* независимый метод создания */
    long createvalue(HKEY hkey, char * path, char * valuename, string value)
    {
        HKEY  key;
        DWORD dwType = REG_SZ;
        DWORD dwSize;

        if (RegCreateKeyEx(hkey, path, 0, 0, 0, KEY_SET_VALUE, NULL, &key, &dwSize) == ERROR_SUCCESS)
        {
            LONG rv = RegSetValueEx(key, valuename, 0, dwType, (const byte *)value.c_str(), strlen(value.c_str()));
            RegCloseKey(key);

            if (rv == ERROR_SUCCESS) return rv;
        }

        return -1;
	}

    /* создание с использованием предуставновленных параметров */
    // string
    long createvalue(char * valuename, string value)
    {
        return createvalue(_rootkey, _path, valuename, value);
    }

    // char *
    long createvalue(char * valuename, char * value)
    {
        return createvalue(_rootkey, _path, valuename, value);
    }

    // template: int uint bool
    template<typename type> long createvalue(char * valuename, type value)
    {
        return createvalue(_rootkey, _path, valuename, to_string((int)value));
    }

    /*************************************************************************************************/
    /* независимый метод удаления */
    long deletevalue(HKEY hkey, char * path, char * valuename)
    {
        HKEY key;

        if (RegOpenKeyEx(hkey, path, 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS)
        {
            LONG rv = RegDeleteValue(key, valuename);
            RegCloseKey(key);

            if (rv == ERROR_SUCCESS) return rv;
        }

        return -1;
    }

private:
    HKEY        _rootkey;               // HKEY_CURRENT_USER HKEY_LOCAL_MACHINE и все в таком духе
    char *      _path;                  // ветка реестра
};

/***********************************************************************************************************/
#endif