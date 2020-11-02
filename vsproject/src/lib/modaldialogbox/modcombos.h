#if !defined modcombos_h
#define modcombos_h
/************************************************************************************************************ 
** three combobox controls for modifiers
**                    __                      __                
** .--------.-----.--|  |-----.-----.--------|  |--.-----.-----.
** |        |  _  |  _  |   __|  _  |        |  _  |  _  |__ --|
** |__|__|__|_____|_____|_____|_____|__|__|__|_____|_____|_____|
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include "controls.h"
#include "../win/gwl.h"

/***********************************************************************************************************/
/* стандартный набор модификаторов 4 штуки и нулевое значение, в нужном нам порядке */
struct mods {int code; char caption[10]; };
#define MODSCOUNT sizeof modifiers / sizeof modifiers[0]
static mods modifiers[] =
{
    {0x00,          ""},
    {MOD_CONTROL,   "Ctrl"},    // 0010
    {MOD_SHIFT,     "Shift"},   // 0100
    {MOD_WIN,       "Win"},     // 1000
    {MOD_ALT,       "Alt"}      // 0001
};

class modcombos
{
public:
    modcombos(HWND hwndparent, int id0, int id1, int id2)
    :   _hwndparent (hwndparent),
        _combo0     (hwndparent, id0),
        _combo1     (hwndparent, id1),
        _combo2     (hwndparent, id2),
        _mods       (0)
    {
        // fix combobox gap
        RECT r0; _combo0.hwnd().getwindowrect(r0);
        RECT r1; _combo1.hwnd().getwindowrect(r1);
        RECT r2; _combo2.hwnd().getwindowrect(r2);

        POINT pt; int gap;

        pt.x = r0.left;
        pt.y = r0.top;
        gap  = r1.left - r0.left;

        ScreenToClient(_hwndparent, &pt);
        _combo2.hwnd().move(pt.x + gap * 2, pt.y, r2.right - r2.left, r2.bottom - r2.top);

        // запихнем все три в массив
        _all[0] = &_combo0; _all[1] = &_combo1; _all[2] = &_combo2;

        // заполним боксы итемами, установим их на 0-й индекс
        for (int n = 0; n < 3; n++)
            for (int i = 0; i < MODSCOUNT; i++) { _all[n]->add(modifiers[i].caption); _all[n]->select(0); }
    }

    /* выбор итемов в зависимости от кода */
    void setup(UINT mods)
    {
        // убираем все лишние флаги если есть
        mods = mods & ~0xFFFFFFF0;

        // вычисляем с какого бокса начнем заполнение так чтобы выровнять по правому краю
        // если каким-то образом mods == 15, то есть 4 бита, 
        // то чтобы n не стал отрицательным - объявляем его как unsigned
        unsigned int n = 3 - pop(mods);

        // заполняем боксы
        for (n; n < 3; n++) { for (int i = 1; i < MODSCOUNT; i++) {
            if (modifiers[i].code & mods) {          // если бит устанвлен
                _all[n]->select(getindex(modifiers[i].code));
                mods = mods & ~modifiers[i].code;   // remove this bit
                break; 
        }}}

        // сохраняем в зависимости от выбранных контролов
        this->mods();
    }

    /* вычислить и отдать модификатор на основе состояния боксов */
    UINT mods()
    {
        _mods = 0;
        for (int n = 0; n < 3; n++) { _mods = _mods | modifiers[_all[n]->currentindex()].code; }
        return _mods;
    }

    void show() { _combo0.show(); _combo1.show(); _combo2.show(); }
    void hide() { _combo0.hide(); _combo1.hide(); _combo2.hide(); }

    handler handler0() { return _combo0.hwnd(); }
    handler handler1() { return _combo1.hwnd(); }
    handler handler2() { return _combo2.hwnd(); }

/***********************************************************************************************************/
private:
    // get index by code
    int getindex(UINT code)
    {
        int index = MODSCOUNT;
        while (index) { index--; if (modifiers[index].code == code) break; }
        return index;
    }

    //  hamming weight
    unsigned int bitcount(unsigned int value) 
    {
        unsigned int count = 0;
        while (value > 0)                   // until all bits are zero
        {
            if ((value & 1) == 1) count++;  // check lower bit
            value >>= 1;                    // shift bits, removing lower bit
        }
        return count;
    }

    //  Hamming weight хакерским методом ))
    int pop(unsigned x)
    {
        x =  x - ((x >> 1) & 0x55555555);
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
        x = (x + (x >> 4)) & 0x0F0F0F0F;
        x =  x + (x >> 8);
        x =  x + (x >> 16);
        return x & 0x0000003F;
    }

private:
    handler     _hwndparent;

    combo       _combo0;
    combo       _combo1;
    combo       _combo2;
    combo *     _all[3];

    UINT        _mods;
};

/***********************************************************************************************************/
#endif
