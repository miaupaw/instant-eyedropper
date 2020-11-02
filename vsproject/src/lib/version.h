#if !defined version_h
#define version_h
/************************************************************************************************************
**                         __              
** .--.--.-----.----.-----|__|-----.-----.
** |  |  |  -__|   _|__ --|  |  _  |     |
**  \___/|_____|__| |_____|__|_____|__|__|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <string>
#include <iostream>

using namespace std;

/***********************************************************************************************************/
struct version
{
    int major{0}, minor{0}, build{0}, revis{0};

    version(const string & version)
    {
        sscanf_s(version.c_str(), "%d.%d.%d.%d", &major, &minor, &build, &revis);
        if (major < 0) major = 0;
        if (minor < 0) minor = 0;
        if (build < 0) build = 0;
        if (revis < 0) revis = 0;
    }

    bool operator < (const version & other)
    {
        if (major < other.major) return true;
        if (major > other.major) return false;

        if (minor < other.minor) return true;
        if (minor > other.minor) return false;

        if (build < other.build) return true;
        if (build > other.build) return false;

        if (revis < other.revis) return true;
        if (revis > other.revis) return false;

        return false;
    }

    bool operator == (const version & other)
    {
        return major == other.major 
            && minor == other.minor 
            && build == other.build
            && revis == other.revis;
    }

    friend ostream & operator << (ostream & stream, const version & ver) 
    {
        stream << ver.major;
        stream << '.';
        stream << ver.minor;
        stream << '.';
        stream << ver.build;
        stream << '.';
        stream << ver.revis;
        return stream;
    }
};

/***********************************************************************************************************/
#endif
