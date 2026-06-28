/*
 * glyph_from_symbol.cpp - Функция определения графических данных символа
 *
 */

#include "glyphs.hpp"

using namespace graphics::glyphs;

#define glyph_case(symb, glyph) \
    case (dword)(symb): return (glyph);

const glyph& glyph_from_symb_latin_upper(dword symbol) {
    switch (symbol) {
        glyph_case('A', LATIN_UPPERCASE_A)
        glyph_case('B', LATIN_UPPERCASE_B)
        glyph_case('C', LATIN_UPPERCASE_C)
        glyph_case('D', LATIN_UPPERCASE_D)
        glyph_case('E', LATIN_UPPERCASE_E)
        glyph_case('F', LATIN_UPPERCASE_F)
        glyph_case('G', LATIN_UPPERCASE_G)
        glyph_case('H', LATIN_UPPERCASE_H)
        glyph_case('I', LATIN_UPPERCASE_I)
        glyph_case('J', LATIN_UPPERCASE_J)
        glyph_case('K', LATIN_UPPERCASE_K)
        glyph_case('L', LATIN_UPPERCASE_L)
        glyph_case('M', LATIN_UPPERCASE_M)
        glyph_case('N', LATIN_UPPERCASE_N)
        glyph_case('O', LATIN_UPPERCASE_O)
        glyph_case('P', LATIN_UPPERCASE_P)
        glyph_case('Q', LATIN_UPPERCASE_Q)
        glyph_case('R', LATIN_UPPERCASE_R)
        glyph_case('S', LATIN_UPPERCASE_S)
        glyph_case('T', LATIN_UPPERCASE_T)
        glyph_case('U', LATIN_UPPERCASE_U)
        glyph_case('V', LATIN_UPPERCASE_V)
        glyph_case('W', LATIN_UPPERCASE_W)
        glyph_case('X', LATIN_UPPERCASE_X)
        glyph_case('Y', LATIN_UPPERCASE_Y)
        glyph_case('Z', LATIN_UPPERCASE_Z)
        default: return INVALIDCHAR;
    }
}

const glyph& glyph_from_symb_latin_lower(dword symbol) {
    switch (symbol) {
        glyph_case('a', LATIN_LOWERCASE_A)
        glyph_case('b', LATIN_LOWERCASE_B)
        glyph_case('c', LATIN_LOWERCASE_C)
        glyph_case('d', LATIN_LOWERCASE_D)
        glyph_case('e', LATIN_LOWERCASE_E)
        glyph_case('f', LATIN_LOWERCASE_F)
        glyph_case('g', LATIN_LOWERCASE_G)
        glyph_case('h', LATIN_LOWERCASE_H)
        glyph_case('i', LATIN_LOWERCASE_I)
        glyph_case('j', LATIN_LOWERCASE_J)
        glyph_case('k', LATIN_LOWERCASE_K)
        glyph_case('l', LATIN_LOWERCASE_L)
        glyph_case('m', LATIN_LOWERCASE_M)
        glyph_case('n', LATIN_LOWERCASE_N)
        glyph_case('o', LATIN_LOWERCASE_O)
        glyph_case('p', LATIN_LOWERCASE_P)
        glyph_case('q', LATIN_LOWERCASE_Q)
        glyph_case('r', LATIN_LOWERCASE_R)
        glyph_case('s', LATIN_LOWERCASE_S)
        glyph_case('t', LATIN_LOWERCASE_T)
        glyph_case('u', LATIN_LOWERCASE_U)
        glyph_case('v', LATIN_LOWERCASE_V)
        glyph_case('w', LATIN_LOWERCASE_W)
        glyph_case('x', LATIN_LOWERCASE_X)
        glyph_case('y', LATIN_LOWERCASE_Y)
        glyph_case('z', LATIN_LOWERCASE_Z)
        default: return INVALIDCHAR;
    }
}

const glyph& glyph_from_symb_digit(dword symbol) {
    switch (symbol) {
        glyph_case('0', NUMBER_0)
        glyph_case('1', NUMBER_1)
        glyph_case('2', NUMBER_2)
        glyph_case('3', NUMBER_3)
        glyph_case('4', NUMBER_4)
        glyph_case('5', NUMBER_5)
        glyph_case('6', NUMBER_6)
        glyph_case('7', NUMBER_7)
        glyph_case('8', NUMBER_8)
        glyph_case('9', NUMBER_9)
        default: return INVALIDCHAR;
    }
}

const glyph& glyph_from_symb_cyrillic_upper(dword symb) {
    switch (symb) {
        glyph_case('А', CYRILLIC_UPPERCASE_A)
        glyph_case('Б', CYRILLIC_UPPERCASE_BE)
        glyph_case('В', CYRILLIC_UPPERCASE_VE)
        glyph_case('Г', CYRILLIC_UPPERCASE_GE)
        glyph_case('Д', CYRILLIC_UPPERCASE_DE)
        glyph_case('Е', CYRILLIC_UPPERCASE_YE)
        glyph_case('Ё', CYRILLIC_UPPERCASE_YO)
        glyph_case('Ж', CYRILLIC_UPPERCASE_ZHE)
        glyph_case('З', CYRILLIC_UPPERCASE_ZE)
        glyph_case('И', CYRILLIC_UPPERCASE_I)
        glyph_case('Й', CYRILLIC_UPPERCASE_I_KRATKOYE)
        glyph_case('К', CYRILLIC_UPPERCASE_KA)
        glyph_case('Л', CYRILLIC_UPPERCASE_EL)
        glyph_case('М', CYRILLIC_UPPERCASE_EM)
        glyph_case('Н', CYRILLIC_UPPERCASE_EN)
        glyph_case('О', CYRILLIC_UPPERCASE_O)
        glyph_case('П', CYRILLIC_UPPERCASE_PE)
        glyph_case('Р', CYRILLIC_UPPERCASE_ER)
        glyph_case('С', CYRILLIC_UPPERCASE_ES)
        glyph_case('Т', CYRILLIC_UPPERCASE_TE)
        glyph_case('У', CYRILLIC_UPPERCASE_U)
        glyph_case('Ф', CYRILLIC_UPPERCASE_EF)
        glyph_case('Х', CYRILLIC_UPPERCASE_HA)
        glyph_case('Ц', CYRILLIC_UPPERCASE_TSE)
        glyph_case('Ч', CYRILLIC_UPPERCASE_CHE)
        glyph_case('Ш', CYRILLIC_UPPERCASE_SHA)
        glyph_case('Щ', CYRILLIC_UPPERCASE_SHYA)
        glyph_case('Ъ', CYRILLIC_UPPERCASE_TVERDIY_ZNAK)
        glyph_case('Ы', CYRILLIC_UPPERCASE_UI)
        glyph_case('Ь', CYRILLIC_UPPERCASE_MYAGKIY_ZNAK)
        glyph_case('Э', CYRILLIC_UPPERCASE_IE)
        glyph_case('Ю', CYRILLIC_UPPERCASE_YU)
        glyph_case('Я', CYRILLIC_UPPERCASE_YA)
        default: return INVALIDCHAR;
    }
}

const glyph& glyph_from_symb_cyrillic_lower(dword symb) {
    switch (symb) {
        glyph_case('а', CYRILLIC_LOWERCASE_A)
        glyph_case('б', CYRILLIC_LOWERCASE_BE)
        glyph_case('в', CYRILLIC_LOWERCASE_VE)
        glyph_case('г', CYRILLIC_LOWERCASE_GE)
        glyph_case('д', CYRILLIC_LOWERCASE_DE)
        glyph_case('е', CYRILLIC_LOWERCASE_YE)
        glyph_case('ё', CYRILLIC_LOWERCASE_YO)
        glyph_case('ж', CYRILLIC_LOWERCASE_ZHE)
        glyph_case('з', CYRILLIC_LOWERCASE_ZE)
        glyph_case('и', CYRILLIC_LOWERCASE_I)
        glyph_case('й', CYRILLIC_LOWERCASE_I_KRATKOYE)
        glyph_case('к', CYRILLIC_LOWERCASE_KA)
        glyph_case('л', CYRILLIC_LOWERCASE_EL)
        glyph_case('м', CYRILLIC_LOWERCASE_EM)
        glyph_case('н', CYRILLIC_LOWERCASE_EN)
        glyph_case('о', CYRILLIC_LOWERCASE_O)
        glyph_case('п', CYRILLIC_LOWERCASE_PE)
        glyph_case('р', CYRILLIC_LOWERCASE_ER)
        glyph_case('с', CYRILLIC_LOWERCASE_ES)
        glyph_case('т', CYRILLIC_LOWERCASE_TE)
        glyph_case('у', CYRILLIC_LOWERCASE_U)
        glyph_case('ф', CYRILLIC_LOWERCASE_EF)
        glyph_case('х', CYRILLIC_LOWERCASE_HA)
        glyph_case('ц', CYRILLIC_LOWERCASE_TSE)
        glyph_case('ч', CYRILLIC_LOWERCASE_CHE)
        glyph_case('ш', CYRILLIC_LOWERCASE_SHA)
        glyph_case('щ', CYRILLIC_LOWERCASE_SHYA)
        glyph_case('ъ', CYRILLIC_LOWERCASE_TVERDIY_ZNAK)
        glyph_case('ы', CYRILLIC_LOWERCASE_UI)
        glyph_case('ь', CYRILLIC_LOWERCASE_MYAGKIY_ZNAK)
        glyph_case('э', CYRILLIC_LOWERCASE_IE)
        glyph_case('ю', CYRILLIC_LOWERCASE_YU)
        glyph_case('я', CYRILLIC_LOWERCASE_YA)
        default: return INVALIDCHAR;
    }
}

const glyph& glyph_from_symb_misc(dword symb) {
    switch (symb) {
        glyph_case('.', DOT)
        glyph_case(',', COMMA)
        glyph_case(':', COLON)
        glyph_case(';', SEMICOLON)
        glyph_case('!', EXCLAMATION_POINT)
        glyph_case('?', QUESTION_MARK)
        glyph_case('@', AT_SYMBOL)
        glyph_case('#', HASH)
        glyph_case('$', DOLLAR_SIGN)
        glyph_case('%', PERCENTAGE)
        glyph_case('^', UP)
        glyph_case('&', AMPERSAND)
        glyph_case('*', STAR)
        glyph_case('(', OPEN_ROUND_BRACKET)
        glyph_case(')', CLOSE_ROUND_BRACKET)
        glyph_case('[', OPEN_SQUARE_BRACKET)
        glyph_case(']', CLOSE_SQUARE_BRACKET)
        glyph_case('{', OPEN_CURLY_BRACKET)
        glyph_case('}', CLOSE_CURLY_BRACKET)
        glyph_case('-', MINUS)
        glyph_case('+', PLUS)
        glyph_case('=', EQUALS)
        glyph_case('/', SLASH)
        glyph_case('\\', BACKSLASH)
        glyph_case('|', LINE)
        glyph_case('<', LESS_SYMBOL)
        glyph_case('>', MORE_SYMBOL)
        glyph_case('\'', APOSTROPHE)
        glyph_case('"', QUOTATION_MARK)
        glyph_case('~', TILDA)
        glyph_case('`', ACUTE)
        glyph_case('_', UNDERSCORE)
        default: return INVALIDCHAR;
    }
}

const glyph& graphics::glyph_from_symbol(dword symb) {
    if (symb == ' ')
        return NULLGLYPH;
    if ('A' <= symb && symb <= 'Z')
        return glyph_from_symb_latin_upper(symb);
    if ('a' <= symb && symb <= 'z')
        return glyph_from_symb_latin_lower(symb);
    if ('0' <= symb && symb <= '9')
        return glyph_from_symb_digit(symb);

//    dword *dbg_ptr = (dword*)0x9510;
//    dbg_ptr[0] = 'А';
//    dbg_ptr[1] = symb;
//    dbg_ptr[2] = 'Я';

    if ('А' <= symb && symb <= 'Я' || symb == 'Ё')
        return glyph_from_symb_cyrillic_upper(symb);
    if ('а' <= symb && symb <= 'я' || symb == 'ё')
        return glyph_from_symb_cyrillic_lower(symb);

    return glyph_from_symb_misc(symb);
}