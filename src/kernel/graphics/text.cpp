#include "graphics.hpp"
#include "glyphs.hpp"

uint16_t textCurX = 1;
uint16_t textCurY = 1;

uint32_t defaultTextCol;
uint32_t defaultBGCol;

uint32_t warnTextCol;
uint32_t warnBGCol;

uint32_t errorTextCol;
uint32_t errorBGCol;

uint16_t screenWidth;
uint16_t screenHeight;

Glyph getglyph(uint8_t code) {
    switch (code)
    {
    case 'A':
        return LATIN_UPPERCASE_A;
    case 'B':
        return LATIN_UPPERCASE_B;
    case 'C': 
        return LATIN_UPPERCASE_C;
    case 'D':
        return LATIN_UPPERCASE_D;
    case 'E':
        return LATIN_UPPERCASE_E;
    case 'F':
        return LATIN_UPPERCASE_F;
    case 'G':
        return LATIN_UPPERCASE_G;
    case 'H':
        return LATIN_UPPERCASE_H;
    case 'I':
        return LATIN_UPPERCASE_I;
    case 'J':
        return LATIN_UPPERCASE_J;
    case 'K':
        return LATIN_UPPERCASE_K;
    case 'L':
        return LATIN_UPPERCASE_L;
    case 'M':
        return LATIN_UPPERCASE_M;
    case 'N':
        return LATIN_UPPERCASE_N;
    case 'O':
        return LATIN_UPPERCASE_O;
    case 'P':
        return LATIN_UPPERCASE_P;
    case 'Q':
        return LATIN_UPPERCASE_Q;
    case 'R':
        return LATIN_UPPERCASE_R;
    case 'S':
        return LATIN_UPPERCASE_S;
    case 'T':
        return LATIN_UPPERCASE_T;
    case 'U':
        return LATIN_UPPERCASE_U;
    case 'V':
        return LATIN_UPPERCASE_V;
    case 'W':
        return LATIN_UPPERCASE_W;
    case 'X':
        return LATIN_UPPERCASE_X;
    case 'Y':
        return LATIN_UPPERCASE_Y;
    case 'Z':
        return LATIN_UPPERCASE_Z;
    case 'a':
        return LATIN_LOWERCASE_A;
    case 'b':
        return LATIN_LOWERCASE_B;
    case 'c': 
        return LATIN_LOWERCASE_C;
    case 'd':
        return LATIN_LOWERCASE_D;
    case 'e':
        return LATIN_LOWERCASE_E;
    case 'f':
        return LATIN_LOWERCASE_F;
    case 'g':
        return LATIN_LOWERCASE_G;
    case 'h':
        return LATIN_LOWERCASE_H;
    case 'i':
        return LATIN_LOWERCASE_I;
    case 'j':
        return LATIN_LOWERCASE_J;
    case 'k':
        return LATIN_LOWERCASE_K;
    case 'l':
        return LATIN_LOWERCASE_L;
    case 'm':
        return LATIN_LOWERCASE_M;
    case 'n':
        return LATIN_LOWERCASE_N;
    case 'o':
        return LATIN_LOWERCASE_O;
    case 'p':
        return LATIN_LOWERCASE_P;
    case 'q':
        return LATIN_LOWERCASE_Q;
    case 'r':
        return LATIN_LOWERCASE_R;
    case 's':
        return LATIN_LOWERCASE_S;
    case 't':
        return LATIN_LOWERCASE_T;
    case 'u':
        return LATIN_LOWERCASE_U;
    case 'v':
        return LATIN_LOWERCASE_V;
    case 'w':
        return LATIN_LOWERCASE_W;
    case 'x':
        return LATIN_LOWERCASE_X;
    case 'y':
        return LATIN_LOWERCASE_Y;
    case 'z':
        return LATIN_LOWERCASE_Z;
    case '0':
        return NUMBER_0;
    case '1':
        return NUMBER_1;
    case '2':
        return NUMBER_2;
    case '3':
        return NUMBER_3;
    case '4':
        return NUMBER_4;
    case '5':
        return NUMBER_5;
    case '6':
        return NUMBER_6;
    case '7':
        return NUMBER_7;
    case '8':
        return NUMBER_8;
    case '9':
        return NUMBER_9;
    case '.':
        return DOT;
    case ',':
        return COMMA;
    case ':':
        return COLON;
    case ';':
        return SEMICOLON;
    case '!':
        return EXCLAMATION_POINT;
    case '?':
        return QUESTION_MARK;
    case '@':
        return AT_SYMBOL;
    case '#':
        return HASH;
    case '$':
        return DOLLAR_SIGN;
    case '%':
        return PERCENTAGE;
    case '^':
        return UP;
    case '&':
        return AMPERSAND;
    case '*':
        return STAR;
    case '(':
        return OPEN_ROUND_BRACKET;
    case ')':
        return CLOSE_ROUND_BRACKET;
    case '[':
        return OPEN_SQUARE_BRACKET;
    case ']':
        return CLOSE_SQUARE_BRACKET;
    case '{':
        return OPEN_CURLY_BRACKET;
    case '}':
        return CLOSE_CURLY_BRACKET;
    case '-':
        return MINUS;
    case '+':
        return PLUS;
    case '=':
        return EQUALS;
    case '/':
        return SLASH;
    case '\\':
        return BACKSLASH;
    case '|':
        return LINE;
    case '<':
        return LESS_SYMBOL;
    case '>':
        return MORE_SYMBOL;
    case '\'':
        return APOSTROPHE;
    case '"':
        return QUOTATION_MARK;
    case '~':
        return TILDA;
    case '_':
        return UNDERSCORE;
    default:
        return INVALIDCHAR;
    }
}

Glyph getglyph(uint16_t unicode) {
    if (unicode < 0x80)
        return getglyph((uint8_t)(unicode & 0x7F));
    switch (unicode) {
        case 'А':
            return CYRILLIC_UPPERCASE_A;
        case 'Б':
            return CYRILLIC_UPPERCASE_BE;
        case 'В':
            return CYRILLIC_UPPERCASE_VE;
        case 'Г':
            return CYRILLIC_UPPERCASE_GE;
        case 'Д':
            return CYRILLIC_UPPERCASE_DE;
        case 'Е':
            return CYRILLIC_UPPERCASE_YE;
        case 'Ё':
            return CYRILLIC_UPPERCASE_YO;
        case 'Ж':
            return CYRILLIC_UPPERCASE_ZHE;
        case 'З':
            return CYRILLIC_UPPERCASE_ZE;
        case 'И':
            return CYRILLIC_UPPERCASE_I;
        case 'Й':
            return CYRILLIC_UPPERCASE_I_KRATKOYE;
        case 'К':
            return CYRILLIC_UPPERCASE_KA;
        case 'Л':
            return CYRILLIC_UPPERCASE_EL;
        case 'М':
            return CYRILLIC_UPPERCASE_EM;
        case 'Н':
            return CYRILLIC_UPPERCASE_EN;
        case 'О':
            return CYRILLIC_UPPERCASE_O;
        case 'П':
            return CYRILLIC_UPPERCASE_PE;
        case 'Р':
            return CYRILLIC_UPPERCASE_ER;
        case 'С':
            return CYRILLIC_UPPERCASE_ES;
        case 'Т':
            return CYRILLIC_UPPERCASE_TE;
        case 'У':
            return CYRILLIC_UPPERCASE_U;
        case 'Ф':
            return CYRILLIC_UPPERCASE_EF;
        case 'Х':
            return CYRILLIC_UPPERCASE_HA;
        case 'Ц':
            return CYRILLIC_UPPERCASE_TSE;
        case 'Ч':
            return CYRILLIC_UPPERCASE_CHE;
        case 'Ш':
            return CYRILLIC_UPPERCASE_SHA;
        case 'Щ':
            return CYRILLIC_UPPERCASE_SHYA;
        case 'Ъ':
            return CYRILLIC_UPPERCASE_TVERDIY_ZNAK;
        case 'Ы':
            return CYRILLIC_UPPERCASE_UI;
        case 'Ь':
            return CYRILLIC_UPPERCASE_MYAGKIY_ZNAK;
        case 'Э':
            return CYRILLIC_UPPERCASE_IE;
        case 'Ю':
            return CYRILLIC_UPPERCASE_YU;
        case 'Я':
            return CYRILLIC_UPPERCASE_YA;
        case 'а':
            return CYRILLIC_LOWERCASE_A;
        case 'б':
            return CYRILLIC_LOWERCASE_BE;
        case 'в':
            return CYRILLIC_LOWERCASE_VE;
        case 'г':
            return CYRILLIC_LOWERCASE_GE;
        case 'д':
            return CYRILLIC_LOWERCASE_DE;
        case 'е':
            return CYRILLIC_LOWERCASE_YE;
        case 'ё':
            return CYRILLIC_LOWERCASE_YO;
        case 'ж':
            return CYRILLIC_LOWERCASE_ZHE;
        case 'з':
            return CYRILLIC_LOWERCASE_ZE;
        case 'и':
            return CYRILLIC_LOWERCASE_I;
        case 'й':
            return CYRILLIC_LOWERCASE_I_KRATKOYE;
        case 'к':
            return CYRILLIC_LOWERCASE_KA;
        case 'л':
            return CYRILLIC_LOWERCASE_EL;
        case 'м':
            return CYRILLIC_LOWERCASE_EM;
        case 'н':
            return CYRILLIC_LOWERCASE_EN;
        case 'о':
            return CYRILLIC_LOWERCASE_O;
        case 'п':
            return CYRILLIC_LOWERCASE_PE;
        case 'р':
            return CYRILLIC_LOWERCASE_ER;
        case 'с':
            return CYRILLIC_LOWERCASE_ES;
        case 'т':
            return CYRILLIC_LOWERCASE_TE;
        case 'у':
            return CYRILLIC_LOWERCASE_U;
        case 'ф':
            return CYRILLIC_LOWERCASE_EF;
        case 'х':
            return CYRILLIC_LOWERCASE_HA;
        case 'ц':
            return CYRILLIC_LOWERCASE_TSE;
        case 'ч':
            return CYRILLIC_LOWERCASE_CHE;
        case 'ш':
            return CYRILLIC_LOWERCASE_SHA;
        case 'щ':
            return CYRILLIC_LOWERCASE_SHYA;
        case 'ъ':
            return CYRILLIC_LOWERCASE_TVERDIY_ZNAK;
        case 'ы':
            return CYRILLIC_LOWERCASE_UI;
        case 'ь':
            return CYRILLIC_LOWERCASE_MYAGKIY_ZNAK;
        case 'э':
            return CYRILLIC_LOWERCASE_IE;
        case 'ю':
            return CYRILLIC_LOWERCASE_YU;
        case 'я':
            return CYRILLIC_LOWERCASE_YA;
        default:
            return INVALIDCHAR;
    }
} 

bool isnullglyph(Glyph g) {
    for (int i = 0; i < 24; i++) {
        if (g.lines[i] != 0)
            return false;
    }
    return true;
}

void putglyph(Glyph glyph, uint16_t x, uint16_t y, uint32_t letter_col, uint32_t back_col) {
    uint32_t offset = y * pitch + x * (bpp/8);
    for (int i = 0; i < 24; i++) {
        uint32_t line = glyph.lines[i];
        uint16_t mask = 1 << 15;
        for (int j = 0; j < 16; j++) {
            uint32_t pixcol = (line & mask) ? letter_col : back_col;
            putpixel(offset, pixcol);
            mask >>= 1;
            offset += bpp/8;
        }
        offset -= 16*(bpp/8);
        offset += pitch;
    }
}

void kprint(const char str[]) {
    uint8_t state = 0;
    unsigned char symb = *str;
    while (*str != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == 0x20)
            state = 4;
        else
            state = 0;
        uint16_t unicode = 0;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                putglyph(g, textCurX * 16, textCurY * 24, defaultTextCol, defaultBGCol);
                textCurX ++;
                break;
            case 1:
                unicode = symb << 8;
                str++;
                symb = *str;
                unicode += symb;
                g = getglyph(unicode);
                putglyph(g, textCurX * 16, textCurY * 24, defaultTextCol, defaultBGCol);
                textCurX++;
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                putglyph(NULLGLYPH, textCurX * 16, textCurY * 24, defaultTextCol, defaultBGCol);
                textCurX ++;
                break;
        }
        if (textCurX >= screenWidth / 16) {
            textCurX = 1;
            textCurY ++;
        }
        str++;
        symb = *str;
    }
    textCurX = 1;
    textCurY ++;
}

void kwarn(const char str[]) {
    uint8_t state = 0;
    uint8_t *dbgPtr = (uint8_t*)0x100300;
    unsigned char symb = *str;
    while (*str != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == 0x20)
            state = 4;
        else
            state = 0;
        uint16_t unicode = 0;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                putglyph(g, textCurX * 16, textCurY * 24, warnTextCol, warnBGCol);
                textCurX ++;
                break;
            case 1:
                unicode = symb << 8;
                str++;
                symb = *str;
                unicode += symb;
                *(uint16_t*)dbgPtr = unicode;
                dbgPtr += 2;
                g = getglyph(unicode);
                putglyph(g, textCurX * 16, textCurY * 24, warnTextCol, warnBGCol);
                textCurX++;
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                putglyph(NULLGLYPH, textCurX * 16, textCurY * 24, warnTextCol, warnBGCol);
                textCurX ++;
                break;
        }
        str++;
        symb = *str;
    }
    textCurX = 1;
    textCurY ++;
}

void kerror(const char str[]) {
    uint8_t state = 0;
    uint8_t *dbgPtr = (uint8_t*)0x100300;
    unsigned char symb = *str;
    while (*str != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == 0x20)
            state = 4;
        else
            state = 0;
        uint16_t unicode = 0;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                putglyph(g, textCurX * 16, textCurY * 24, errorTextCol, errorBGCol);
                textCurX ++;
                break;
            case 1:
                unicode = symb << 8;
                str++;
                symb = *str;
                unicode += symb;
                *(uint16_t*)dbgPtr = unicode;
                dbgPtr += 2;
                g = getglyph(unicode);
                putglyph(g, textCurX * 16, textCurY * 24, errorTextCol, errorBGCol);
                textCurX++;
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                putglyph(NULLGLYPH, textCurX * 16, textCurY * 24, errorTextCol, errorBGCol);
                textCurX ++;
                break;
        }
        str++;
        symb = *str;
    }
    textCurX = 1;
    textCurY ++;
}