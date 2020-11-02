#if !defined combotemplate_h
#define combotemplate_h
/************************************************************************************************************ 
** special combobox for color templates
**                      __          __                        __       __         
** .----.-----.--------|  |--.-----|  |_.-----.--------.-----|  |-----|  |_.-----.
** |  __|  _  |        |  _  |  _  |   _|  -__|        |  _  |  |  _  |   _|  -__|
** |____|_____|__|__|__|_____|_____|____|_____|__|__|__|   __|__|___._|____|_____|
**                                                     |__|                           instant-eyedropper.com
************************************************************************************************************/

#include <string>
#include <vector>
using namespace std;

#include "../../statics.h"
#include "controls.h"

/***********************************************************************************************************/
struct colortemplate { int format; string value; };

/* наследник комбо со своими приколами */
class combotemplate: public combo
{
public:
    combotemplate(HWND hwndparent, int id) 
    :   combo       (hwndparent, id), 
        _hwndparent (hwndparent),
        _id         (id),
        _topvalues  (),
        _formats    ()
    {
        InitCommonControls();
    }
   ~combotemplate() { }

    // передают указатель на данные
    void init(vector<colorformats> * f) 
    {
        // сохраняем указатель
        _formats = f; 

        // заполняем локальную пачку текущих рабочих параметров
        for (auto pack : *_formats) { _topvalues.push_back( {pack.format, pack.templates[0]} ); }
    }

    // шлют формат, нужно собрать контрол
    void setup(int format)
    {
        // вдруг еще нету
        if (!_formats) return;

        // ага
        clear();

        // набиваем комбо
        for (auto pack : *_formats) { if (pack.format == format) 
        {   
            // edit
            int i = 0; for (auto v : _topvalues) 
            {   if (v.format == format) { set(_topvalues[i].value.c_str()); } i++; }

            // combo
            // пропустим первый элемент (default minus dynamic см. base)
            int j =-1; for (auto s : pack.templates) { if (j) { j++; continue; } add(s.c_str()); }
        }}
    }
    
    // обработка события CBN_EDITCHANGE
    void echange(int format)
    {
        int i = 0; for (auto v : _topvalues) 
        {
            // считывает текст с контрола
            if (v.format == format) { _topvalues[i].value = _hwnd.gettext(512); } 
            i++; 
        }
    }

    // обработка события CBN_SELCHANGE
    // приходится обрабатывать это событие, потому что при выборе элемента из выпадающего списка
    // мессага CBN_EDITCHANGE не приходит 
    void schange(int format)
    {
        int i = 0; for (auto pack : *_formats) 
        {
            if (pack.format == format) { _topvalues[i].value = pack.templates[currentindex()+1]; }
            i++;
        }
    }

    // сохраняет изменения уже в данных на базе
    void storedata()
    {
        int i = 0; for (auto pack : *_formats)
        {
            for (auto v : _topvalues)
            {
                if (v.format == pack.format) { _formats->at(i).templates[0] = v.value; }
            }
            i++;
        }
    }

/***********************************************************************************************************/
private:
    // удаляет все элементы
    void clear() { _hwnd.sendmessage(CB_RESETCONTENT, 0, 0); }

private:
    handler                     _hwndparent;
    int                         _id;

    vector<colortemplate>       _topvalues;
    vector<colorformats> *      _formats;
};

/***********************************************************************************************************/
#endif
