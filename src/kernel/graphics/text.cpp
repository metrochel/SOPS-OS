#include <stdarg.h>
#include "glyphs.hpp"

uint16_t textCurX = 1;
uint16_t textCurY = 1;
bool textCurOnScreen = false;
bool textCurAllowed = false;

uint32_t defaultTextCol;
uint32_t defaultBGCol;

uint32_t warnTextCol;
uint32_t warnBGCol;

uint32_t errorTextCol;
uint32_t errorBGCol;

uint16_t screenWidth;
uint16_t screenHeight;

uint8_t ticks = 0;

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
    case '`':
        return ACUTE;
    case '_':
        return UNDERSCORE;
    default:
        return INVALIDCHAR;
    }
}

Glyph getglyph(char c) {
    return getglyph((uint8_t)c);
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

Glyph getOctDigit(uint32_t dig) {
    while (dig >= 8) {
        dig >>= 3;
    }
    switch (dig) {
        case 0: return NUMBER_0;
        case 1: return NUMBER_1;
        case 2: return NUMBER_2;
        case 3: return NUMBER_3;
        case 4: return NUMBER_4;
        case 5: return NUMBER_5;
        case 6: return NUMBER_6;
        case 7: return NUMBER_7;
        default: return LATIN_UPPERCASE_O;
    }
}

Glyph getHexDigit(uint32_t dig) {
    while (dig >= 16) {
        dig >>= 4;
    }
    switch (dig) {
        case 0: return NUMBER_0;
        case 1: return NUMBER_1;
        case 2: return NUMBER_2;
        case 3: return NUMBER_3;
        case 4: return NUMBER_4;
        case 5: return NUMBER_5;
        case 6: return NUMBER_6;
        case 7: return NUMBER_7;
        case 8: return NUMBER_8;
        case 9: return NUMBER_9;
        case 10: return LATIN_UPPERCASE_A;
        case 11: return LATIN_UPPERCASE_B;
        case 12: return LATIN_UPPERCASE_C;
        case 13: return LATIN_UPPERCASE_D;
        case 14: return LATIN_UPPERCASE_E;
        case 15: return LATIN_UPPERCASE_F;
        default: return LATIN_UPPERCASE_X;
    }
}

bool isnullglyph(Glyph g) {
    for (uint8_t i = 0; i < 24; i++) {
        if (g.lines[i] != 0)
            return false;
    }
    return true;
}

void putglyph(Glyph glyph, uint16_t x, uint16_t y, uint32_t letter_col, uint32_t back_col) {
    uint32_t offset = y * pitch + x * (bpp/8);
    for (uint8_t i = 0; i < 24; i++) {
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

void printchar(Glyph glyph, uint32_t charCol, uint32_t bgCol) {
    putglyph(glyph, textCurX * 16, textCurY * 24, charCol, bgCol);
    textCurX++;
    if (textCurX >= (screenWidth / 16 - 1)) {
        textCurX = 1;
        textCurY ++;
    }
}

void printchar(uint8_t c, uint32_t charCol, uint32_t bgCol) {
    Glyph g = getglyph(c);
    printchar(g, charCol, bgCol);
}

void hideCursor() {
    uint32_t offset = textCurY * 24 * pitch + textCurX * 16 * (bpp/8);
    for (uint8_t i = 0; i < 24; i++) {
        uint32_t line = CURSOR.lines[i];
        uint16_t mask = 1 << 15;
        for (int j = 0; j < 16; j++) {
            if (line & mask)
                putpixel(offset, defaultBGCol);
            mask >>= 1;
            offset += bpp/8;
        }
        offset -= 16*(bpp/8);
        offset += pitch;
    }
    textCurOnScreen = false;
}

void disableCursor() {
    hideCursor();
    textCurAllowed = false;
}

void showCursor() {
    uint32_t offset = textCurY * 24 * pitch + textCurX * 16 * (bpp/8);
    for (uint8_t i = 0; i < 24; i++) {
        uint32_t line = CURSOR.lines[i];
        uint16_t mask = 1 << 15;
        for (int j = 0; j < 16; j++) {
            if (line & mask)
                putpixel(offset, defaultTextCol);
            mask >>= 1;
            offset += bpp/8;
        }
        offset -= 16*(bpp/8);
        offset += pitch;
    }
    textCurOnScreen = true;
}

void enableCursor() {
    showCursor();
    textCurAllowed = true;
    ticks = 0;
}

void updateCursor() {
    if (!textCurAllowed)
        return;
    ticks ++;
    if (ticks >= 10) {
        ticks = 0;
        if (textCurOnScreen)
            hideCursor();
        else
            showCursor();
    }
}

void printBinUInt(uint32_t num, uint32_t charCol, uint32_t bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('b', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    uint32_t mask = 1;
    uint8_t digits = 0;
    while (mask <= num && mask < (uint32_t)(1 << 31)) {
        mask <<= 1;
        digits ++;
    }
    mask >>= 1;
    digits --;
    if (textCurX + digits >= (screenWidth / 16 - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    printchar('0', charCol, bgCol);
    printchar('b', charCol, bgCol);
    while (mask > 0) {
        Glyph g;
        if (num & mask)
            g = NUMBER_1;
        else
            g = NUMBER_0;
        printchar(g, charCol, bgCol);
        mask >>= 1;
    }
}

void printOctUInt(uint32_t num, uint32_t charCol, uint32_t bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('o', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    uint32_t mask = 7;
    uint8_t digits = 1;
    while (mask < num && mask < (uint32_t)(1 << 31)) {
        mask <<= 3;
        digits ++;
    }
    if (textCurX + digits >= (screenWidth / 16 - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    if (!(num & mask))
        mask >>= 3;
    printchar('0', charCol, bgCol);
    printchar('o', charCol, bgCol);
    while (mask > 0) {
        Glyph g = getOctDigit(num & mask);
        printchar(g, charCol, bgCol);
        mask >>= 3;
    }
}

void printHexUInt(uint32_t num, uint32_t charCol, uint32_t bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('x', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    uint32_t mask = 0xF;
    uint8_t digits = 1;
    while (mask < num && mask < (uint32_t)(1 << 31)) {
        mask <<= 4;
        digits ++;
    }
    if (textCurX + digits >= (screenWidth / 16 - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    if (!(num & mask))
        mask >>= 4;
    printchar('0', charCol, bgCol);
    printchar('x', charCol, bgCol);
    while (mask > 0) {
        Glyph g = getHexDigit(num & mask);
        printchar(g, charCol, bgCol);
        mask >>= 4;
    }
}

void printDecUInt(uint32_t num, uint32_t charCol, uint32_t bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        return;
    }
    uint32_t numclone = num;
    uint8_t digits = 0;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    if (textCurX + digits >= (screenWidth / 16 - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    textCurX += digits - 1;
    while (num > 0) {
        uint8_t digit = num % 10;
        Glyph g = getglyph((uint8_t)(0x30 + digit));
        printchar(g, charCol, bgCol);
        num /= 10;
        textCurX -= 2;
    }
    textCurX += digits + 1;
}

void printFloat(double num, uint32_t charCol, uint32_t bgCol) {
    if (num < 0) {
        printchar('-', charCol, bgCol);
        num = -num;
    }
    uint32_t wholenum = (uint32_t)num;
    printDecUInt(wholenum, charCol, bgCol);
    if (wholenum == num)
        return;
    printchar(',', charCol, bgCol);
    num -= wholenum;
    for (uint8_t i = 0; i < 3; i++) {
        num *= 10;
        uint8_t digit = (uint8_t)num;
        num -= digit;
        Glyph g = getglyph((uint8_t)(0x30 + digit));
        printchar(g, charCol, bgCol);
        if (num == 0)
            break;
    }
    num *= 10;
    uint8_t digit = (uint8_t)num;
    num -= digit;
    num *= 10;
    if ((uint8_t)num >= 5 && digit < 9)
        digit ++;
    Glyph g = getglyph((uint8_t)(0x30 + digit));
    printchar(g, charCol, bgCol);
}

void eraseChar() {
    disableCursor();
    if (textCurX == 1) {
        textCurY --;
        textCurX = screenWidth / 16 - 2;
    } else {
        textCurX --;
    }
    putglyph(NULLGLYPH, textCurX * 16, textCurY * 24, defaultTextCol, defaultBGCol);
    enableCursor();
}

void kprint(const char* text, ...) {
    disableCursor();
    va_list l;
    va_start(l, text);
    uint8_t state = 0;
    unsigned char symb = *text;
    while (*text != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == ' ')
            state = 4;
        else if (symb == '%') {
            text ++;
            symb = *text;
            if (symb == '%')
                state = 0;
            else if (symb == 'x')
                state = 16;
            else if (symb == 'd')
                state = 10;
            else if (symb == 'o')
                state = 8;
            else if (symb == 'b')
                state = 5;
            else if (symb == 'f')
                state = 6;
        }
        else
            state = 0;
        uint16_t unicode = 0;
        uint32_t arg;
        double flarg;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                printchar(g, defaultTextCol, defaultBGCol);
                break;
            case 1:
                unicode = symb << 8;
                text++;
                symb = *text;
                unicode += symb;
                g = getglyph(unicode);
                printchar(g, defaultTextCol, defaultBGCol);
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                printchar(NULLGLYPH, defaultTextCol, defaultBGCol);  
                break;
            case 5:
                arg = va_arg(l, unsigned int);
                printBinUInt(arg, defaultTextCol, defaultBGCol);
                break;
            case 6:
                flarg = va_arg(l, double);
                printFloat(flarg, defaultTextCol, defaultBGCol);
                break;
            case 8:
                arg = va_arg(l, unsigned int);
                printOctUInt(arg, defaultTextCol, defaultBGCol);
                break;
            case 10:
                arg = va_arg(l, unsigned int);
                printDecUInt(arg, defaultTextCol, defaultBGCol);
                break;
            case 16:
                arg = va_arg(l, unsigned int);
                printHexUInt(arg, defaultTextCol, defaultBGCol);
                break;
        }
        text++;
        symb = *text;
    }
    va_end(l);
    enableCursor();
}

void kwarn(const char* text, ...) {
    disableCursor();
    va_list l;
    va_start(l, text);
    uint8_t state = 0;
    unsigned char symb = *text;
    while (*text != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == 0x20)
            state = 4;
        else if (symb == '%') {
            text ++;
            symb = *text;
            if (symb == 'x')
                state = 16;
            else if (symb == 'd')
                state = 10;
            else if (symb == 'o')
                state = 8;
            else if (symb == 'b')
                state = 5;
            else if (symb == 'f')
                state = 6;
            else
                state = 0;
        }
        else
            state = 0;
        uint16_t unicode = 0;
        uint32_t arg;
        double flarg;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                printchar(g, warnTextCol, warnBGCol);
                break;
            case 1:
                unicode = symb << 8;
                text++;
                symb = *text;
                unicode += symb;
                g = getglyph(unicode);
                printchar(g, warnTextCol, warnBGCol);
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                printchar(NULLGLYPH, warnTextCol, warnBGCol);  
                break;
            case 5:
                arg = va_arg(l, unsigned int);
                printBinUInt(arg, warnTextCol, warnBGCol);
                break;
            case 6:
                flarg = va_arg(l, double);
                printFloat(flarg, warnTextCol, warnBGCol);
                break;
            case 8:
                arg = va_arg(l, unsigned int);
                printOctUInt(arg, warnTextCol, warnBGCol);
                break;
            case 10:
                arg = va_arg(l, unsigned int);
                printDecUInt(arg, warnTextCol, warnBGCol);
                break;
            case 16:
                arg = va_arg(l, unsigned int);
                printHexUInt(arg, warnTextCol, warnBGCol);
                break;
        }
        text++;
        symb = *text;
    }
    va_end(l);
    enableCursor();
}

void kerror(const char* text, ...) {
    disableCursor();
    va_list l;
    va_start(l, text);
    uint8_t state = 0;
    unsigned char symb = *text;
    while (*text != 0) {
        if (symb >= 0x80)
            state = 1;
        else if (symb == CR)
            state = 2;
        else if (symb == LF)
            state = 3;
        else if (symb == 0x20)
            state = 4;
        else if (symb == '%') {
            text ++;
            symb = *text;
            if (symb == '%')
                state = 0;
            else if (symb == 'x')
                state = 16;
            else if (symb == 'd')
                state = 10;
            else if (symb == 'o')
                state = 8;
            else if (symb == 'b')
                state = 5;
            else if (symb == 'f')
                state = 6;
        }
        else
            state = 0;
        uint16_t unicode = 0;
        uint32_t arg;
        double flarg;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                printchar(g, errorTextCol, errorBGCol);
                break;
            case 1:
                unicode = symb << 8;
                text++;
                symb = *text;
                unicode += symb;
                g = getglyph(unicode);
                printchar(g, errorTextCol, errorBGCol);
                break;
            case 2:
                textCurX = 1;
                break;
            case 3:
                textCurY ++;
                textCurX = 1;
                break;
            case 4:
                printchar(NULLGLYPH, errorTextCol, errorBGCol);  
                break;
            case 5:
                arg = va_arg(l, unsigned int);
                printBinUInt(arg, errorTextCol, errorBGCol);
                break;
            case 6:
                flarg = va_arg(l, double);
                printFloat(flarg, errorTextCol, errorBGCol);
                break;
            case 8:
                arg = va_arg(l, unsigned int);
                printOctUInt(arg, errorTextCol, errorBGCol);
                break;
            case 10:
                arg = va_arg(l, unsigned int);
                printDecUInt(arg, errorTextCol, errorBGCol);
                break;
            case 16:
                arg = va_arg(l, unsigned int);
                printHexUInt(arg, errorTextCol, errorBGCol);
                break;
        }
        text++;
        symb = *text;
    }
    va_end(l);
    enableCursor();
}