#if !defined network_h
#define network_h
/************************************************************************************************************ 
**              __                       __    
** .-----.-----|  |_.--.--.--.-----.----|  |--.
** |     |  -__|   _|  |  |  |  _  |   _|    _| 
** |__|__|_____|____|________|_____|__| |__|__| 
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

//#define WIN32_LEAN_AND_MEAN

/* http://www.zachburlingame.com/2011/05/resolving-redefinition-errors-betwen-ws2def-h-and-winsock-h/
** By explicitly including WinSock2.h before every place that you Windows.h, you prevent the collision.
** However, this can be quite cumbersome at times.
** UPDATE 2011 - 11 - 12:This method can cause issues with struct packing and alignment if WIN32 isn’t
** defined before including WinSock2.h(see Oren’s comment below).To resolve this issue, either define
** WIN32 as a preprocessor flag in your project or explicitly define #WIN32 prior to the WinSock2.h
** include as I’ve done below. */
#ifndef WIN32
#define WIN32
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT  "80"

/***********************************************************************************************************/

#include <vector>
#include <string>
#include <sstream>
#include <regex>
using namespace std;

typedef unsigned char         byte;         // это
typedef vector<unsigned char> datastore;    // для
typedef vector<string>        httpheader;   // красоты )

/***********************************************************************************************************/
class network
{
public:
    network()
    :   _initiated  (false),
        _socket     (INVALID_SOCKET),
        _host       (""),
        _port       (DEFAULT_PORT) {}
   ~network() { }

    bool init(void);                                        // init winsock
    void cleanup(void);                                     // cleanup winsock
    bool connect(string host, string port = DEFAULT_PORT);  // connect
    bool httpget(string uri,  string range = "");           // get header
    int  receive(datastore &);                              // receive data
    int  closesocket();                                     // close socket

/***********************************************************************************************************/
public:
    /* get content length */
    int contentlength()
    {
        smatch sm;
        regex  exp ("Content-Length:\\s+(\\d+)[\\s\\S]*");
        for (auto i : _httpheader) { if (regex_match(i, sm, exp))
        {
            string length = sm[1].str();
            return stoi(length);
        }}
        return 0;
    }

    /* get content range full size */
    int contentrangefullsize()
    {
        smatch sm;
        regex  exp ("Content-Range:.*?/(\\d+)[\\s\\S]*");
        for (auto i : _httpheader) { if (regex_match(i, sm, exp))
        {
            string length = sm[1].str();
            return stoi(length);
        }}
        return 0;
    }

    /* узнает реальный путь вместо запрашиваемого */
    string contentdisposition()
    {
        smatch sm;
        regex  exp ("Content-Disposition: attachment; filename=(.*)[\\s\\S]*");
        for (auto i : _httpheader) { if (regex_match(i, sm, exp))
        {
            string name = sm[1].str();
            return name;
        }}
        return "";
    }

private:
    bool        _initiated;
    SOCKET      _socket;

    string      _host;
    string      _port;

    httpheader  _httpheader;
    datastore   _firstblock;
};

/***********************************************************************************************************/
#endif
