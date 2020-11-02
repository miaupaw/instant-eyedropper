#if !defined color_h
#define color_h
/************************************************************************************************************
** color
**             __             
** .----.-----|  |-----.----.
** |  __|  _  |  |  _  |   _|
** |____|_____|__|_____|__|  
**                                                                                    instant-eyedropper.com
************************************************************************************************************/

#include <string>   // string
#include <sstream>  // stringstream
#include <iomanip>  // setfill setw
#include <regex>    // regex

using namespace std;

// stored templates typedef struct
#include "../statics.h"

// для hsv и hsl
#define min_f(a, b, c) (fminf(a, fminf(b, c)))
#define max_f(a, b, c) (fmaxf(a, fmaxf(b, c)))

/***********************************************************************************************************/
class color
{
public:
    color(COLORREF color,  vector<colorformats> * templates) : _color(color), _colorstring("")
    {   
        settemplates(templates);
    }

    color() : _color (0), _colorstring ("") { }
   ~color() { }

    void setcolor(COLORREF color) { _color = color; }
    void settemplates(vector<colorformats> * templates)
    {
        for (auto t : *templates)
        {
            if (t.format == ID_FORMAT_HTML)     _template_html   = t.templates[0]; _template_html   = unescape(_template_html);
            if (t.format == ID_FORMAT_HEX)      _template_hex    = t.templates[0]; _template_hex    = unescape(_template_hex);
            if (t.format == ID_FORMAT_DELPHI)   _template_delphi = t.templates[0]; _template_delphi = unescape(_template_delphi);
            if (t.format == ID_FORMAT_VB)       _template_vb     = t.templates[0]; _template_vb     = unescape(_template_vb);
            if (t.format == ID_FORMAT_RGB)      _template_rgb    = t.templates[0]; _template_rgb    = unescape(_template_rgb);
            if (t.format == ID_FORMAT_RGBFLOAT) _template_float  = t.templates[0]; _template_float  = unescape(_template_float);
            if (t.format == ID_FORMAT_HSV)      _template_hsv    = t.templates[0]; _template_hsv    = unescape(_template_hsv);
            if (t.format == ID_FORMAT_HSL)      _template_hsl    = t.templates[0]; _template_hsl    = unescape(_template_hsl);
            if (t.format == ID_FORMAT_LONG)     _template_long   = t.templates[0]; _template_long   = unescape(_template_long);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string hex(bool asis = true, string tlt = "")
    {
        int r = (int)GetRValue(_color);
        int g = (int)GetGValue(_color);
        int b = (int)GetBValue(_color);

        string out;

        // если прислали темплейт, будем использовать его
        if (tlt.empty()) out = _template_hex;
        else             out = tlt;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\(r\\)"), int_to_hex(r, 0));
        out = regex_replace(out, regex("\\(g\\)"), int_to_hex(g, 0));
        out = regex_replace(out, regex("\\(b\\)"), int_to_hex(b, 0));
        out = regex_replace(out, regex("\\(R\\)"), int_to_hex(r, 1));
        out = regex_replace(out, regex("\\(G\\)"), int_to_hex(g, 1));
        out = regex_replace(out, regex("\\(B\\)"), int_to_hex(b, 1));

        // заменяем эскейпы на пробелы
        if (!asis) out = regex_replace(out, regex("(\\n|\\t)"), " ");

        _colorstring = out; return _colorstring;
    }

    COLORREF hex(LPTSTR colorcode)
    {
        _color = (COLORREF)strtol(colorcode, 0, 16);
        return _color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // отдает цвет в html формате 
    string html(bool asis = true)
    {
        _colorstring = hex(asis, _template_html);
        return _colorstring;
    }

    // получает цвет в html формате, переводит в базовый, сохраняет и возвращает
    COLORREF html(string colorcode)
    {
        // сюда будем временно складировать данные
        stringstream sstr;

        // паттерн для хтмл кода цвета
        regex  exp("#{0,1}([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})");
        smatch sm; // матчи

        // если паттерн совпал
        if (regex_match(colorcode, sm, exp)) sstr << sm[3] << sm[2] << sm[1];

        // string to hex
        sstr >> std::hex >> _color;

        // return COLORREF
        return _color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string delphi(bool asis = true)
    {
        _colorstring = hex(asis, _template_delphi);
        return _colorstring;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string vb(bool asis = true)
    {
        _colorstring = hex(asis, _template_vb);
        return _colorstring;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string rgb(bool asis = true)
    {
        int r = (int)GetRValue(_color);
        int g = (int)GetGValue(_color);
        int b = (int)GetBValue(_color);

        string out = _template_rgb;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\((r|R)\\)"), to_string(r));
        out = regex_replace(out, regex("\\((g|G)\\)"), to_string(g));
        out = regex_replace(out, regex("\\((b|B)\\)"), to_string(b));

        // заменяем эскейпы на пробелы
        if (!asis) out = regex_replace(out, regex("(\\n|\\t)"), " ");

        _colorstring = out; return _colorstring;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string rgbfloat(bool asis = true, int precision = 5)
    {
        double r = (double)GetRValue(_color);
        double g = (double)GetGValue(_color);
        double b = (double)GetBValue(_color);

        double fr = (1.0/255.0f)*r;
        double fg = (1.0/255.0f)*g;
        double fb = (1.0/255.0f)*b;

        stringstream sstr;
        sstr << fixed << setprecision(precision);

        // генерируем строки для каждого цвета
        sstr.str(""); sstr << fr; string red = sstr.str();
        sstr.str(""); sstr << fg; string grn = sstr.str();
        sstr.str(""); sstr << fb; string blu = sstr.str();

        string out = _template_float;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\((r|R)\\)"), red);
        out = regex_replace(out, regex("\\((g|G)\\)"), grn);
        out = regex_replace(out, regex("\\((b|B)\\)"), blu);

        // вставляем переносы строк
        if (!asis) { sstr.str(""); sstr << red << '\n' << grn << '\n' << blu; out = sstr.str(); }

        _colorstring = out; return _colorstring;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////
    string longvalue(bool asis = true)
    {
        long r = (long)GetRValue(_color);
        long g = (long)GetGValue(_color);
        long b = (long)GetBValue(_color);

        long longvalue =  r + (256 * g) + (65536 * b);
        
        string out = _template_long;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\(long\\)"), to_string(longvalue));

        // заменяем эскейпы на ничто
        if (!asis) out = regex_replace(out, regex("(\\n|\\t)"), "");

        _colorstring = out; return _colorstring;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string hsv(bool asis = true, int precision = 0)
    {
        float r = (float)GetRValue(_color)/255.0f;
        float g = (float)GetGValue(_color)/255.0f;
        float b = (float)GetBValue(_color)/255.0f;

        float h, s, v; // h(0-360.0), s(0.0-1.0), v(0.0-1.0)

        float max = max_f(r, g, b);
        float min = min_f(r, g, b);

        v = max;

        if            (max == 0.0f) { s = 0; h = 0; }
        else if (max - min == 0.0f) { s = 0; h = 0; }
        else                        { s = (max - min) / max;

            if      (max == r) { h = 60 * ((g - b) / (max - min)) + 0; }
            else if (max == g) { h = 60 * ((b - r) / (max - min)) + 120; }
            else               { h = 60 * ((r - g) / (max - min)) + 240; }
        }

        if (h < 0) h += 360.0f;
        s = s * 100;
        v = v * 100;

        // складируем
        stringstream sstr; sstr << fixed << setprecision(precision);

        // генерируем строки для каждого значения
        sstr.str(""); sstr << h; string hue = sstr.str();
        sstr.str(""); sstr << s; string sat = sstr.str();
        sstr.str(""); sstr << v; string val = sstr.str();

        string out = _template_hsv;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\((h|H)\\)"), hue);
        out = regex_replace(out, regex("\\((s|S)\\)"), sat);
        out = regex_replace(out, regex("\\((v|V)\\)"), val);

        // вставляем пробелы
        if (!asis) { sstr.str(""); sstr << hue << " " << sat << " " << val; out = sstr.str(); }

        _colorstring = out; return _colorstring;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    string hsl(bool asis = true, int precision = 0)
    {
        float r = (float)GetRValue(_color)/255.0f;
        float g = (float)GetGValue(_color)/255.0f;
        float b = (float)GetBValue(_color)/255.0f;

        float h, s, l; // h(0-360.0), s(0.0-1.0), l(0.0-1.0)

        float max = max_f(r, g, b);
        float min = min_f(r, g, b);

	    l = (max + min) / 2;

	    if (max == min) { s = 0; h = 0; }
	    else
	    {
		    if (l < .50)  { s = (max - min) / (max + min); }
		    else          { s = (max - min) / (2 - max - min); }

		    if (max == r) { h =     (g - b) / (max - min); }
		    if (max == g) { h = 2 + (b - r) / (max - min); }
		    if (max == b) { h = 4 + (r - g) / (max - min); }
	    }

	    h = h * 60.0f; if (h < 0) h += 360.0f;
        s = s * 100;
        l = l * 100;

        // складируем
        stringstream sstr; sstr << fixed << setprecision(precision);

        // генерируем строки для каждого значения
        sstr.str(""); sstr << h; string hue = sstr.str();
        sstr.str(""); sstr << s; string sat = sstr.str();
        sstr.str(""); sstr << l; string lit = sstr.str();

        string out = _template_hsv;

        // делаем из шаблона реальное значение
        out = regex_replace(out, regex("\\((h|H)\\)"), hue);
        out = regex_replace(out, regex("\\((s|S)\\)"), sat);
        out = regex_replace(out, regex("\\((l|L)\\)"), lit);

        // вставляем пробелы
        if (!asis) { sstr.str(""); sstr << hue << " " << sat << " " << lit; out = sstr.str(); }

        _colorstring = out; return _colorstring;
    }

/***********************************************************************************************************/
private:
    // convert int to hex
    template<typename type> string int_to_hex(type i , bool uppercase = false)
    {
      stringstream stream;
      if (uppercase) stream << std::uppercase;
      stream << std::setfill('0') << std::setw(2) << std::hex << uintptr_t(i);
      return stream.str();
    }

    // делает из виртуальных эскейпов - реальные
    string unescape(const string & s)
    {
        string res;
        string::const_iterator it = s.begin();
        while (it != s.end())
        {
            char c = *it++;
            if (c == '\\' && it != s.end())
            {
                switch (*it++) 
                {
                case '\\': c = '\\'; break;
                case 'n' : c = '\n'; break;
                case 't' : c = '\t'; break;
                // all other escapes
                default: 
                // invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
                continue;
                }
            }
            res += c;
        }
        return res;
    }

/***********************************************************************************************************/
private:
    COLORREF                    _color;
    string                      _colorstring;
    
    string                      _template_html;
    string                      _template_hex;
    string                      _template_delphi;
    string                      _template_vb;
    string                      _template_rgb;
    string                      _template_float;
    string                      _template_hsv;
    string                      _template_hsl;
    string                      _template_long;
};

/***********************************************************************************************************/
#endif