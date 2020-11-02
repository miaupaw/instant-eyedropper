#if !defined updater_h
#define updater_h
/************************************************************************************************************ 
**                 __       __              
** .--.--.-----.--|  |-----|  |_.-----.----.
** |  |  |  _  |  _  |  _  |   _|  -__|   _|
** |_____|   __|_____|___._|____|_____|__|  
**       |__|                                                                         instant-eyedropper.com
************************************************************************************************************/

#include "network.h"                        // нужно инклудить первым, чтобы избежать глюков winsock
#include "../dynamic/dynamic.h"             // thread class
#include "../win/handler.h"                 // handler
#include "../version.h"                     // compare version numbers
#include "../../statics.h"                  // for the SERVERNAME definition

/***********************************************************************************************************/
#include <fstream>                          // для работы с файлами

#define URI "/InstantEyedropper.exe"        // обращение к sendfile

typedef struct tagfileinfo                  // для хранения данных о файле полученные с сервера
{
    string  versionnumber;                  // номер версии
    string  originalfilename;               // то что выколупали из "content-disposition"
    int     contentlength;                  // размер данных, тоже из http заголовка

} fileinfo, *lpfileinfo;

/***********************************************************************************************************/
class updater: public dynamic
{
public:
    updater(HWND hwnd, char * currentversion)
    :   _suspend        (true),
        _hwnd           (hwnd),
        _currentversion (currentversion),
        _button         (NULL)
    {
        // добудем и сохраним путь ко временной директории
        _tmppath = gettempdir();
    }
   ~updater() { kill(); }

    // resume thread, передают хендлер для "progressbar-a"
    void start(HWND buttonhwnd) 
    {
        _suspend = false;       // должно идти первым, уже забыл почему )
        _button  = buttonhwnd;  // передаем указатель на кнопку
        _thread.resume();       // запускаем тред
    }

    void suspend() { _suspend = true; _thread.suspend(); }

private:
    // вызывается при запуске
    void initthread() { }

    // тело потока
    void loop() { while (true) { 

        // магическое слово ). по этому ключевику содержащемуся в кнопке, 
        // определяется действие, которое мы будем предпринимать
        // при вызове всей этой процедуры (нажатии на кнопку)
        string magicword = "Install";

        // если все уже готово для инсталляции то просто запускаем
        if (objectcontains(_button, magicword)) exec(_fullname);

        // проверяем есть ли новая версия на сайте (range 0-0)
        // если есть, то:
        // качаем, если еще не скачана
        // готовим к запуску (прописываем магическое слово в кнопке)
        else
        {
            // маякнем
            _button.settext("Checking...");

            // запрос
            string host = SERVERNAME;
            string uri  = URI; uri += "?" + _currentversion;

            // проверим наличие новой версии,
            // если новой версии нету, сообщаем что все ок 
            // и делаем кнопку серенькой
            fileinfo info = newestversioninfo(host, uri);
            if (!(version(_currentversion) < version(info.versionnumber))) 
            {   
                _button.settext("Up-to-dated");
                _button.disable();
            }

            // если есть новая версия, 
            // то пытаемся скачать (если еще не скачана)
            // и превращаем кнопку в кнопку инсталляции
            else
            {
                // полный путь сохраняем глобально, 
                // чтобы переколдованная кнопка могла запустить инсталляшку
                _fullname = _tmppath + info.originalfilename; 

                // инициализация переменных
                // флаг готовности, текст для кнопки
                bool ready = false;
                string buttontxt;

                // проверяем готовность, если надо - скачиваем
                if (filesize(_fullname) == info.contentlength) ready = true;
                else ready = downloadnewversion(host, uri, _fullname);
                
                // в зависимости от готовности:
                // превращаем кнопку в кнопку инсталляции
                // или в кнопку информации об ошибке
                if (ready) buttontxt = magicword + " " + info.versionnumber;
                else       buttontxt = "Some error";
                
                // рисуем то что нужно
                _button.settext(buttontxt.c_str());
            }
        }
        suspend();
    }}

    // вызывается при уничтожении
    void flushthread() { }

/***********************************************************************************************************/
private:
    // пробуем получить информацию о самой новой версии
    // если версия новее текущей, в структуре возвращаем ее номер
    // если нет - это поле будет пустое
    fileinfo newestversioninfo(string host, string uri)
    {
        // инициализация пустой структуры
        fileinfo fi = {};

        // коннектимся и гребем информацию
        if (_net.init() && _net.connect(host) && _net.httpget(uri, "0-0"))  // коннектимся
        {
            // get original file name and size
            fi.originalfilename = _net.contentdisposition();
            fi.contentlength    = _net.contentrangefullsize();
            _net.closesocket();

            // добываем номер версии
            regex  exp (".*?(\\d+\\.*\\d*\\.*\\d*\\.*\\d*)\\.[a-zA-Z]+");   // паттерн
            smatch sm;                                                      // матчи
            
            // если паттерн совпал
            if (regex_match(fi.originalfilename, sm, exp)) fi.versionnumber = sm[1];
        }

        // возвращаем всегда, пустая структура или нет
        return fi;
    }

    // скачать новую версию
    bool downloadnewversion(string host, string uri, string storepath)
    {
        if (_net.connect(host) && _net.httpget(uri))
        {
            // content-length
            int contentlength = _net.contentlength();

            // продолжаем качать и выводить проценты
            datastore alldata;
            while (_net.receive(alldata)) 
            {
                int percent = alldata.size() * 100 / contentlength;
                string percentstr = to_string(percent) + " %";
                if (_button.visible()) _button.settext(percentstr.c_str());
                //Sleep(1);
            }

            // завершаем все по людски
            _net.closesocket(); // error?
            _net.cleanup();

            // если скачалось полностью - сохраним в файл и сохраним _fullname
            if (alldata.size() == contentlength)
            {
                // подготовим файл
                ofstream outfile(storepath, ofstream::binary);
                ostream_iterator<char> output_iterator(outfile);

                // сохраним данные в файл
                copy(alldata.begin(), alldata.end(), output_iterator);
                outfile.close();
                
                // типа все ок
                return true;
            }
        }

        // что-то пошло не так
        return false;
    }

/***********************************************************************************************************/
private:
    // добыть путь ко временной директории
    string gettempdir()
    {
        string tmppath; int maxlen = 1024;
        lptstr tmpp = new tchar[maxlen];
        GetTempPath(maxlen, tmpp); tmppath = tmpp; 
        delete[] tmpp;
        return tmppath;
    }

    // содержит ли текст виндовс-объекта строку
    bool objectcontains(handler hwnd, string str)
    {
        string objecttext = hwnd.gettext(512);
        if (objecttext.find(str) != string::npos ) return true;
 
        return false;
    }

    // filesize
    int filesize(string filename)
    {
        ifstream file(filename, ios::binary | ios::ate);
        int size = (int)file.tellg(); file.close();
        return size;
    }

    // запуск
    void exec(string path)
    {
        ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

private:
    bool        _suspend;           // флаг состояния треда
    handler     _hwnd;              // хендлер контроллера
    handler     _button;            // хендлер кнопки, получаем в start()
    string      _currentversion;    // контейнер для хранения номера текущей версии (из ресуров от контроллера)

    network     _net;               // еге ж )
    string      _tmppath;           // путь ко временной директории винды, вычисляем в конструкторе
    string      _fullname;          // полное имя: "путь + имя", вычисляем по ходу дела
};

/***********************************************************************************************************/
#endif