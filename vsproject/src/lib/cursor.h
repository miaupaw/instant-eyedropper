#if !defined cursor_h
#define cursor_h
/************************************************************************************************************ 
** custom cursor
** .----.--.--.----.-----.-----.----.
** |  __|  |  |   _|__ --|  _  |   _|
** |____|_____|__| |_____|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/
#include <windows.h>

#define DROPPER 0
#define ARROW   2

/***********************************************************************************************************/

class cursor 
{
public:
    cursor(HINSTANCE hinst) : _hinst  (hinst)
    {
        BYTE droppermask[] =
        {                           
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        };

        BYTE dropper[] =
        { 
            0x01, 0x00, // 0000000100000000
            0x01, 0x00, // 0000000100000000  
            0x01, 0x00, // 0000000100000000
            0x07, 0xC0, // 0000011111000000 
            0x08, 0x20, // 0000100000100000
            0x10, 0x10, // 0001000000010000
            0x10, 0x10, // 0001000000010000
            0xF1, 0x1E, // 1111000100011110
            0x10, 0x10, // 0001000000010000
            0x10, 0x10, // 0001000000010000
            0x08, 0x20, // 0000100000100000
            0x07, 0xC0, // 0000011111000000
            0x01, 0x00, // 0000000100000000
            0x01, 0x00, // 0000000100000000
            0x01, 0x00, // 0000000100000000
            0x00, 0x00, // 0000000000000000
        };

        int sizedropper = sizeof(dropper)/sizeof(BYTE);

        _droppermask = new BYTE[256];
        _dropper     = new BYTE[256];

        for (int i = 0; i < sizedropper; i++)
        {
            _droppermask[i] = droppermask[i];
            _dropper[i]     = dropper[i];
        }

        _cdropper = CreateCursor(_hinst, 7, 7, 16, 16, _droppermask, _dropper);
        _carrow   = LoadCursor(0, IDC_ARROW);
    }

   ~cursor()
    {
        DestroyCursor(_cdropper);

        delete[] _droppermask;
        delete[] _dropper;
    }

    HCURSOR mode(UINT cpointer)
    {
        if (cpointer == DROPPER) return _cdropper;
        if (cpointer == ARROW)   return _carrow;
        
        return 0;
    }

private:
    HINSTANCE   _hinst;

    HCURSOR     _carrow;
    HCURSOR     _cdropper;
    BYTE *      _droppermask;
    BYTE *      _dropper;
};

/***********************************************************************************************************/
#endif

