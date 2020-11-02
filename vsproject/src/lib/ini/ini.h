#if !defined (ini_h)
#define ini_h
/************************************************************************************************************ 
** класс для работы с ini файлами
**  __         __ 
** |__|.-----.|__|
** |  ||     ||  |
** |__||__|__||__|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/
//#pragma warning(disable : 4996)

#include <windows.h>
#include <string>
using namespace std;

#include <shlwapi.h>
#pragma  comment(lib, "shlwapi.lib") 

/***********************************************************************************************************/

class ini
{
public:
    ini(string name) : _filepath ("")
    {
        /* конструктор. получает в качестве параметра имя будушего ини-файла
        ** определяет место запуска и делает абсолютный путь к файлу
        ** который и сохраняет внутри. создает этот файл если он отсутствует
        ** или открывает его на чтение/запись */

        // generate path
        char fullpath[MAX_PATH];
        GetModuleFileName(GetModuleHandle(NULL), fullpath, sizeof(fullpath));
        PathRemoveFileSpec(fullpath);

        // fenerate ini fullname
        stringstream sstr; sstr << fullpath << "\\" << name << ".ini";

        // store string and flag
        _filepath = sstr.str();
        _writeable = WritePrivateProfileString("common", "check", NULL, _filepath.c_str());

        // a little memory space
        _container = new char[MAX_PATH]();
    }

   ~ini() { delete[] _container; }

    bool access()
    {
        if (_writeable) return true;
        else            return false;
    }

    /* читает параметр из текущего ини-файла *******************************************************/
    // char *
    LPTSTR readvalue(LPTSTR section, LPTSTR param, LPTSTR default)
    {
        DWORD r = GetPrivateProfileString(section, param, NULL, _container, MAX_PATH - 1, _filepath.c_str());
        if (r) return _container; else return default;
    }

    // string
    string readvalue(LPTSTR section, LPTSTR param, string default)
    {
        string str = readvalue(section, param, "");
        if (str.empty()) return default; else return str;
    }

    // int
    int readvalue(LPTSTR section, LPTSTR param, int default)
    {
        LPTSTR r = readvalue(section, param, "");
        if (strlen(r) != 0) return stoi(r); else return default;
    }

    // uint
    uint readvalue(LPTSTR section, LPTSTR param, uint default)
    {
        LPTSTR r = readvalue(section, param, "");
        if (strlen(r) != 0) return stoi(r); else return default;
    }

    // bool
    bool readvalue(LPTSTR section, LPTSTR param, bool default)
    {
        LPTSTR r = readvalue(section, param, "");
        if (strlen(r) != 0) return (stoi(r) != 0 ); else return default;
    }

    /* записывает параметр  в текущий ини-файл *****************************************************/
    // string
    void writevalue(LPTSTR section, LPTSTR param, string value)
    {
        value = "\"" + value + "\"";
        WritePrivateProfileString(section, param, value.c_str(), _filepath.c_str());
    }

    // template
    template<typename type> void writevalue(LPTSTR section, LPTSTR param, type value)
    {
        writevalue(section, param, to_string((int)value));
    }

private:
    string      _filepath;      // абсолютный путь к текущему ини-файлу
    BOOL        _writeable;     // access flag
    char *      _container;     // for api functions
};

/***********************************************************************************************************/
#endif