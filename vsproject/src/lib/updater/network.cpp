/************************************************************************************************************ 
**              __                       __    
** .-----.-----|  |_.--.--.--.-----.----|  |--.
** |     |  -__|   _|  |  |  |  _  |   _|    _| 
** |__|__|_____|____|________|_____|__| |__|__| 
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "network.h"

/***********************************************************************************************************/

bool network::init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) return false;
    _initiated = true;
    return true;
}

/***********************************************************************************************************/
/* cleanup winsock */
void network::cleanup() 
{
    _httpheader.clear();
    _firstblock.clear();
    if (!WSACleanup()) _initiated = false; 
}

/***********************************************************************************************************/
/* connect */
bool network::connect(string host, string port)
{
    // сохраняем хост и порт
    _host = host;
    _port = port;

    // variables
    addrinfo * result = NULL;
    addrinfo   hints;

    // get address
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int r = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
    if (r) { cleanup(); return false; }

    // attempt to connect to an address until one succeeds
    for (addrinfo * ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // create a socket for connecting to server
        _socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (_socket == INVALID_SOCKET) { freeaddrinfo(result); cleanup(); return false; }

        // connect to server.
        int r = ::connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (r == SOCKET_ERROR) { closesocket(); continue; }

        break;
    }

    // free addr and return
    freeaddrinfo(result);
    return true;
}

/***********************************************************************************************************/
/* начинаем качать пока не скачается заголовок,
** как только заголовок полностью скачан: 
** - сохраняем заголовок 
** - сохраняем кусок данных если перебрали 
** - возвращаем тру, иначе false */
bool network::httpget(string uri, string range)
{
    // clear used data storages
    _httpheader.clear(); _firstblock.clear();

    // делаем строку http запроса
    stringstream httprequest;

                         httprequest << "GET " << uri << " HTTP/1.0" << endl;
                         httprequest << "Host: " << _host << endl;
    if (!range.empty())  httprequest << "Range: " << "bytes=" << range << endl;
                         httprequest << endl;

    // конвертируем запрос в нужный формат
    const string httpstr = httprequest.str();
    const char * httpchr = httpstr.c_str();

    // send an initial buffer
    int result = send(_socket, httpchr, (int)strlen(httpchr), 0);
    if (result == SOCKET_ERROR) { closesocket(); return false; }

    // receive until the peer closes the connection
    datastore data; char recvbuf[DEFAULT_BUFLEN];
    while (true) 
    {   
        result = recv(_socket, recvbuf, DEFAULT_BUFLEN, 0);

        // get data
        if (result > 0) 
        {
            // сохраняем полученные дпанные
            data.insert(data.end(), recvbuf, recvbuf + result);

            // проверяем загружен ли заголовок полностью (ищем \r\n\r\n)
            smatch sm;
            string s (data.begin(), data.end());
            regex exp ("([\\s\\S]*)\\r*\\n\\r*\\n([\\s\\S]*)");
            if (regex_match(s, sm, exp))
            {
                // складываем строки заголовка в _httpheader
                // надо бы потом разобраться с \r в конце каждой строки 
                // которые нам выдает getline, но сейчас это не горит
                string hdr = sm[1].str();
                stringstream stream(hdr);
                string line;
                while (getline(stream, line)) _httpheader.push_back(line);

                // сохраняем кусок с данными в _lastblock
                string dta = sm[2].str();
                _firstblock.insert(_firstblock.begin(), dta.begin(), dta.end());

                // достатньо :)
                return true;
            }
        }
           
        if (result == 0) return true;   // connection closed
        if (result <  0) break;         // recv failed
    }

    // если добрались сюда - то случилась какая-то ошибка
    WSAGetLastError();
    closesocket();
    return false;
}

/***********************************************************************************************************/
/* получаем указатель куда вернем данные если есть
** возвращаем размер */
int network::receive(datastore & dataptr)
{
    // возвращаем запасенные данные
    if (!_firstblock.empty()) { dataptr = _firstblock; _firstblock.clear(); return dataptr.size(); }

    // если сокет заглючен: заканчиваем
    if (_socket == SOCKET_ERROR) return 0;

    // продолжаем качать данные
    int r; char buf[DEFAULT_BUFLEN];
    while (true)
    {
        r = recv(_socket, buf, DEFAULT_BUFLEN, 0);

        if (r >  0) { dataptr.insert(dataptr.end(), buf, buf + r); return r; }   // get data
        if (r == 0) { closesocket(); return 0; }                                 // connection closed
        if (r <  0) { break; }                                                   // recv failed with error
    }
        
    // если добрались сюда - то случилась какая-то ошибка
    WSAGetLastError();
    closesocket();
    return r;
}

/***********************************************************************************************************/
/* если сокет удачно закрылся - он возвращает 0 
** и мы устанавливает _socket в дефолтное значение */
int network::closesocket()
{
    int r = ::closesocket(_socket); if (r == 0) _socket = INVALID_SOCKET;
    return r;
}

/***********************************************************************************************************/

