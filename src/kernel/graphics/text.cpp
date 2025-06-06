#include <stdarg.h>
#include "glyphs.hpp"
#include "../io/com.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"

word textCurX = 1;
word textCurY = 1;
word auxTextCurX = 1;
word auxTextCurY = 1;
word textLeftBoundX = 1;
word textLeftBoundY = 1;
word textRightBoundX = 1;
word textRightBoundY = 1;
bool textCurOnScreen = false;
bool textCurAllowed = false;

dword defaultTextCol;
dword defaultBGCol;

dword warnTextCol;
dword warnBGCol;

dword errorTextCol;
dword errorBGCol;

word screenWidth;
word screenHeight;

dword ticks = 0;

dword drawLine = 0;
word textScreenWidth;
word textScreenHeight;

void initText() {
    defaultTextCol = encodeRGB(1,1,1);
    defaultBGCol = encodeRGB(0,0,0);
    warnTextCol = encodeRGB(1,1,0);
    warnBGCol = encodeRGB(0.5,0.5,0);
    errorTextCol = encodeRGB(1,0,0);
    errorBGCol = encodeRGB(0.5,0,0);

    textScreenWidth = screenWidth  / 16;
    textScreenHeight = screenHeight / 24;

    textRightBoundX = textScreenWidth - 1;
    textRightBoundY = textScreenHeight - 1;

    textBufferBankSize = (textScreenWidth * textScreenHeight) * 12;
    textBuffer = (Character*)kmalloc(2*textBufferBankSize);
    memset(textBuffer, 2*textBufferBankSize, 0);
}

bool Character::operator!=(Character &c) {
    return this->character != c.character || this->charCol != c.charCol || this->bgCol != c.bgCol;
}

Glyph getglyph(byte code) {
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
    case ' ':
        return NULLGLYPH;
    default:
        return INVALIDCHAR;
    }
}

Glyph getglyph(char c) {
    return getglyph((byte)c);
}

Glyph getglyph(word unicode) {
    if (unicode < 0x80)
        return getglyph((byte)(unicode & 0x7F));
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

char getOctDigit(qword dig) {
    while (dig >= 8) {
        dig >>= 3;
    }
    switch (dig) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        default: return 'O';
    }
}

char getHexDigit(qword dig) {
    while (dig >= 16) {
        dig >>= 4;
    }
    switch (dig) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default: return 'X';
    }
}

bool isnullglyph(Glyph g) {
    for (byte i = 0; i < 24; i++) {
        if (g.lines[i] != 0)
            return false;
    }
    return true;
}

inline void printchar(Glyph glyph, dword charCol, dword bgCol) {
    putglyph(glyph, textCurX * 16, textCurY * 24, charCol, bgCol);
    textCurX++;
    if (textCurX > textRightBoundX) {
        textCurX = textLeftBoundX;
        textCurY ++;
        if (textCurY > textRightBoundY) {
            scroll();
            textCurY --;
        }
    }
}

inline void printchar(byte c, dword charCol, dword bgCol) {
    Glyph g = getglyph(c);
    setchar(textCurX, textCurY, c, charCol, bgCol);
    printchar(g, charCol, bgCol);
}

void scroll() {
    dword offset = textLeftBoundY * textScreenWidth;
    for (dword y = textLeftBoundY; y < textRightBoundY; y++) {
        for (dword x = textLeftBoundX; x <= textRightBoundX; x++) {
            if (textBuffer[offset + x] != textBuffer[offset + x + textScreenWidth]) {
                Glyph g = getglyph((word)textBuffer[offset + x + textScreenWidth].character);
                dword textCol = textBuffer[offset + x + textScreenWidth].charCol;
                dword bgCol = textBuffer[offset + x + textScreenWidth].bgCol;
                putglyph(g, x*16, y*24, textCol, bgCol);
            }
            textBuffer[offset + x] = textBuffer[offset + x + textScreenWidth];
        }
        offset += textScreenWidth;
    }
    for (dword x = textLeftBoundX; x <= textRightBoundX; x++) {
        setchar(x, textRightBoundY, ' ', defaultTextCol, defaultBGCol);
        putglyph(NULLGLYPH, x*16, textRightBoundY*24, defaultTextCol, defaultBGCol);
    }
}

void refreshScreen() {
    dword offset = drawLine * textScreenWidth;
    for (dword y = 0; y < textScreenHeight * 24; y += 24) {
        for (dword x = 0; x < textScreenWidth * 16; x += 16) {
            word c = (word)textBuffer[offset].character;
            Glyph g = getglyph(c);
            putglyph(g, x, y, textBuffer[offset].charCol, textBuffer[offset].bgCol);
            offset ++;
        }
    }
}

void hideCursor() {
    dword offset = textCurY * 24 * pitch + textCurX * 16 * (bpp/8);
    for (byte i = 0; i < 24; i++) {
        dword line = CURSOR.lines[i];
        word mask = 1 << 15;
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
    dword offset = textCurY * 24 * pitch + textCurX * 16 * (bpp/8);
    for (byte i = 0; i < 24; i++) {
        dword line = CURSOR.lines[i];
        word mask = 1 << 15;
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

void printBinUInt(qword num, dword charCol, dword bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('b', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    qword mask = 1;
    byte digits = 0;
    while (mask <= num && mask < (qword)(0x8000000000000000)) {
        mask <<= 1;
        digits ++;
    }
    mask >>= 1;
    digits --;
    if (textCurX + digits >= (textScreenWidth - 1)) {
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
        setchar(textCurX, textCurY, 0x30 + ((num & mask) ? 1 : 0), charCol, bgCol);
        printchar(g, charCol, bgCol);
        mask >>= 1;
    }
}

void printOctUInt(qword num, dword charCol, dword bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('o', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    qword mask = 7;
    byte digits = 1;
    while (mask < num && mask < (qword)(0x8000000000000000)) {
        mask <<= 3;
        digits ++;
    }
    if (textCurX + digits >= (textScreenWidth - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    if (!(num & mask))
        mask >>= 3;
    printchar('0', charCol, bgCol);
    printchar('o', charCol, bgCol);
    while (mask > 0) {
        char c = getOctDigit(num & mask);
        Glyph g = getglyph(c);
        setchar(textCurX, textCurY, c, charCol, bgCol);
        printchar(g, charCol, bgCol);
        mask >>= 3;
    }
}

void printHexUInt(qword num, dword charCol, dword bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        printchar('x', charCol, bgCol);
        printchar('0', charCol, bgCol);
        printchar('0', charCol, bgCol);
        return;
    }
    qword mask = 0xF;
    byte digits = 1;
    while (mask < num && mask < (qword)(0xF000000000000000)) {
        mask <<= 4;
        digits ++;
    }
    if (textCurX + digits >= (textScreenWidth - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    if (!(num & mask))
        mask >>= 4;
    printchar('0', charCol, bgCol);
    printchar('x', charCol, bgCol);
    if (mask == 0xF)
        printchar('0', charCol, bgCol);
    while (mask > 0) {
        char c = getHexDigit(num & mask);
        Glyph g = getglyph(c);
        setchar(textCurX, textCurY, c, charCol, bgCol);
        printchar(g, charCol, bgCol);
        mask >>= 4;
    }
}

void printDecUInt(qword num, dword charCol, dword bgCol) {
    if (num == 0) {
        printchar('0', charCol, bgCol);
        return;
    }
    qword numclone = num;
    byte digits = 0;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    if (textCurX + digits >= (textScreenWidth - 1)) {
        textCurX = 1;
        textCurY ++;
    }
    textCurX += digits - 1;
    while (num > 0) {
        byte digit = num % 10;
        char c = (char)(0x30 + digit);
        Glyph g = getglyph(c);
        setchar(textCurX, textCurY, c, charCol, bgCol);
        printchar(g, charCol, bgCol);
        num /= 10;
        textCurX -= 2;
    }
    textCurX += digits + 1;
}

void printFloat(double num, dword charCol, dword bgCol) {
    if (num < 0) {
        printchar('-', charCol, bgCol);
        num = -num;
    }
    dword wholenum = (dword)num;
    printDecUInt(wholenum, charCol, bgCol);
    if (wholenum == num)
        return;
    printchar(',', charCol, bgCol);
    num -= wholenum;
    for (byte i = 0; i < 3; i++) {
        num *= 10;
        byte digit = (byte)num;
        num -= digit;
        char d = (char)(0x30 + digit);
        Glyph g = getglyph(d);
        setchar(textCurX, textCurY, d, charCol, bgCol);
        printchar(g, charCol, bgCol);
        if (num == 0)
            break;
    }
    num *= 10;
    byte digit = (byte)num;
    num -= digit;
    num *= 10;
    if ((byte)num >= 5 && digit < 9)
        digit ++;
    char d = (byte)(0x30 + digit);
    Glyph g = getglyph(d);
    setchar(textCurX, textCurY, d, charCol, bgCol);
    printchar(g, charCol, bgCol);
}

void eraseChar() {
    disableCursor();
    if (textCurX == 1) {
        textCurY --;
        textCurX = textScreenWidth - 2;
    } else {
        textCurX --;
    }
    setchar(textCurX, textCurY, 0, defaultTextCol, defaultBGCol);
    putglyph(NULLGLYPH, textCurX * 16, textCurY * 24, defaultTextCol, defaultBGCol);
    enableCursor();
}

void printStr(const char* text, va_list args, dword charCol, dword bgCol, bool format) {
    disableCursor();
    byte state = 0;
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
        else if (symb == '%' && format) {
            text ++;
            symb = *text;
            if (symb == '%')
                state = 0;
            else if (symb == 'x')
                state = 16;
            else if (symb == 'X')
                state = 64;
            else if (symb == 'd')
                state = 10;
            else if (symb == 'D')
                state = 100;
            else if (symb == 'o')
                state = 8;
            else if (symb == 'O')
                state = 88;
            else if (symb == 'b')
                state = 22;
            else if (symb == 'B')
                state = 222;
            else if (symb == 'f')
                state = 6;
        }
        else
            state = 0;
        word unicode = 0;
        qword arg;
        double flarg;
        switch (Glyph g; state) {
            case 0:
                g = getglyph(symb);
                setchar(textCurX, textCurY, symb, charCol, bgCol);
                printchar(g, charCol, bgCol);
                break;
            case 1:
                unicode = symb << 8;
                text++;
                symb = *text;
                unicode += symb;
                g = getglyph(unicode);
                setchar(textCurX, textCurY, unicode, charCol, bgCol);
                printchar(g, charCol, bgCol);
                break;
            case 2:
                textCurX = textLeftBoundX;
                break;
            case 3:
                textCurY ++;
                textCurX = textLeftBoundX;
                if (textCurY > textRightBoundY) {
                    scroll();
                    textCurY --;
                }
                break;
            case 4:
                setchar(textCurX, textCurY, ' ', charCol, bgCol);
                printchar(NULLGLYPH, charCol, bgCol);  
                break;
            case 22:
                arg = va_arg(args, dword);
                arg &= 0xFFFFFFFF;
                printBinUInt(arg, charCol, bgCol);
                break;
            case 222:
                arg = va_arg(args, qword);
                printBinUInt(arg, charCol, bgCol);
                break;
            case 6:
                flarg = va_arg(args, double);
                printFloat(flarg, charCol, bgCol);
                break;
            case 8:
                arg = va_arg(args, dword);
                arg &= 0xFFFFFFFF;
                printOctUInt(arg, charCol, bgCol);
                break;
            case 88:
                arg = va_arg(args, qword);
                printOctUInt(arg, charCol, bgCol);
                break;
            case 10:
                arg = va_arg(args, dword);
                arg &= 0xFFFFFFFF;
                printDecUInt(arg, charCol, bgCol);
                break;
            case 100:
                arg = va_arg(args, qword);
                printDecUInt(arg, charCol, bgCol);
                break;
            case 16:
                arg = va_arg(args, dword);
                arg &= 0xFFFFFFFF;
                printHexUInt(arg, charCol, bgCol);
                break;
            case 64:
                arg = va_arg(args, qword);
                printHexUInt(arg, charCol, bgCol);
                break;
        }
        text++;
        symb = *text;
    }
    va_end(args);
    enableCursor();
}

void kprint(const char* text, ...) {
    va_list l;
    va_start(l, text);
    printStr(text, l, defaultTextCol, defaultBGCol, true);
}

void kwarn(const char* text, ...) {
    va_list l;
    va_start(l, text);
    printStr(text, l, warnTextCol, warnBGCol, true);
}

void kerror(const char* text, ...) {
    va_list l;
    va_start(l, text);
    printStr(text, l, errorTextCol, errorBGCol, true);
}