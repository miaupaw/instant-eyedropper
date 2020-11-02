#if !defined autoload_h
#define autoload_h
/************************************************************************************************************ 
** автозагрузка CurrentVersion\\Run\\
**              __         __                __ 
** .-----.--.--|  |_.-----|  |-----.-----.--|  |
** |  _  |  |  |   _|  _  |  |  _  |  _  |  _  |
** |___._|_____|____|_____|__|_____|___._|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "registry/registry.h"

/***********************************************************************************************************/

static char * const rpath  = "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";
static char * const rvalue = "instanteyedropper";

/***********************************************************************************************************/
class autoload
{
public:
    autoload(bool state)
    {
        /* состояние по умолчанию */
        _state = state;

        /* вычислим образец ключа (полный путь к .exe с кавычками) 
        ** с которым будем впоследствии сравнивать */
        char * path = new char[MAX_PATH]();        
        GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);   //полное имя файла (с путем)

        stringstream sstr; sstr << std::nouppercase << _sample << "\"" << path << "\"";
        _sample = sstr.str();

        delete[] path;
    }

    /* прверят наличие автозагрузки в реестре и устанавливает 
    ** состояние локальной переменной _state в соответсвующее значение */
    void check()
    {
        /* колдуем с реестром */
        /* сравним образец с реальным значением */
        registry reg; string rdata  = reg.queryvalue(HKEY_CURRENT_USER, rpath, rvalue);
        if (rdata.compare(_sample) == 0) _state = true;
    }

    /* установить если надо ключ */
    void set(bool is)
    {
        /* установим или удалим запись */
        registry reg(HKEY_CURRENT_USER, rpath);
        if (is) { if (reg.createvalue(rvalue, _sample) == ERROR_SUCCESS) _state = true; }
        else    {     reg.deletevalue(HKEY_CURRENT_USER, rpath, rvalue); _state = false; }
    }

    /* возвращает текущее состояние */
    const bool * state() { return &_state; }

private:
    bool    _state;
    string  _sample;
};

/***********************************************************************************************************/
#endif
