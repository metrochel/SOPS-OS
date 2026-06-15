/*
 * glyphs.hpp - Символы
 *
 * Предоставляет постоянные-символы для bitmap-шрифта.
 */

#ifndef _GLYPHS_INCL
#define _GLYPHS_INCL

#include "libk/nums.hpp"

#define declare_glyph(name) extern const glyph name

namespace graphics::inline glyphs {
    /// Ширина символа в пикселах
    inline const dword glyph_width = 16;
    /// Высота символа в пикселах
    inline const dword glyph_height = 24;

    /// @c glyph - это структура, хранящая данные, достаточные для отрисовки символа.
    struct glyph {
        word lines[glyph_height];    /// Бит-поля, отмечающие пикселы символа
    } __attribute__((packed));

    /// Возвращает данные для отрисовки символа в соответствии с числовым кодом символа.
    /// @param symbol Символ
    /// @return Графический символ или @c INVALIDCHAR, если символа нет в шрифте
    const glyph& glyph_from_symbol(dword symbol);

    /// Нулевой символ
    declare_glyph(NULLGLYPH);
    /// Несуществующий символ
    declare_glyph(INVALIDCHAR);
    /// Текстовый курсор
    declare_glyph(CURSOR);

    /// === Латинский алфавит ===

    /// Латинская заглавная буква A
    declare_glyph(LATIN_UPPERCASE_A);
    /// Латинская заглавная буква B
    declare_glyph(LATIN_UPPERCASE_B);
    /// Латинская заглавная буква C
    declare_glyph(LATIN_UPPERCASE_C);
    /// Латинская заглавная буква D
    declare_glyph(LATIN_UPPERCASE_D);
    /// Латинская заглавная буква E
    declare_glyph(LATIN_UPPERCASE_E);
    /// Латинская заглавная буква F
    declare_glyph(LATIN_UPPERCASE_F);
    /// Латинская заглавная буква G
    declare_glyph(LATIN_UPPERCASE_G);
    /// Латинская заглавная буква H
    declare_glyph(LATIN_UPPERCASE_H);
    /// Латинская заглавная буква I
    declare_glyph(LATIN_UPPERCASE_I);
    /// Латинская заглавная буква J
    declare_glyph(LATIN_UPPERCASE_J);
    /// Латинская заглавная буква K
    declare_glyph(LATIN_UPPERCASE_K);
    /// Латинская заглавная буква L
    declare_glyph(LATIN_UPPERCASE_L);
    /// Латинская заглавная буква M
    declare_glyph(LATIN_UPPERCASE_M);
    /// Латинская заглавная буква N
    declare_glyph(LATIN_UPPERCASE_N);
    /// Латинская заглавная буква O
    declare_glyph(LATIN_UPPERCASE_O);
    /// Латинская заглавная буква P
    declare_glyph(LATIN_UPPERCASE_P);
    /// Латинская заглавная буква Q
    declare_glyph(LATIN_UPPERCASE_Q);
    /// Латинская заглавная буква R
    declare_glyph(LATIN_UPPERCASE_R);
    /// Латинская заглавная буква S
    declare_glyph(LATIN_UPPERCASE_S);
    /// Латинская заглавная буква T
    declare_glyph(LATIN_UPPERCASE_T);
    /// Латинская заглавная буква U
    declare_glyph(LATIN_UPPERCASE_U);
    /// Латинская заглавная буква V
    declare_glyph(LATIN_UPPERCASE_V);
    /// Латинская заглавная буква W
    declare_glyph(LATIN_UPPERCASE_W);
    /// Латинская заглавная буква X
    declare_glyph(LATIN_UPPERCASE_X);
    /// Латинская заглавная буква Y
    declare_glyph(LATIN_UPPERCASE_Y);
    /// Латинская заглавная буква Z
    declare_glyph(LATIN_UPPERCASE_Z);
    /// Латинская строчная буква A
    declare_glyph(LATIN_LOWERCASE_A);
    /// Латинская строчная буква B
    declare_glyph(LATIN_LOWERCASE_B);
    /// Латинская строчная буква C
    declare_glyph(LATIN_LOWERCASE_C);
    /// Латинская строчная буква D
    declare_glyph(LATIN_LOWERCASE_D);
    /// Латинская строчная буква E
    declare_glyph(LATIN_LOWERCASE_E);
    /// Латинская строчная буква F
    declare_glyph(LATIN_LOWERCASE_F);
    /// Латинская строчная буква G
    declare_glyph(LATIN_LOWERCASE_G);
    /// Латинская строчная буква H
    declare_glyph(LATIN_LOWERCASE_H);
    /// Латинская строчная буква I
    declare_glyph(LATIN_LOWERCASE_I);
    /// Латинская строчная буква J
    declare_glyph(LATIN_LOWERCASE_J);
    /// Латинская строчная буква K
    declare_glyph(LATIN_LOWERCASE_K);
    /// Латинская строчная буква L
    declare_glyph(LATIN_LOWERCASE_L);
    /// Латинская строчная буква M
    declare_glyph(LATIN_LOWERCASE_M);
    /// Латинская строчная буква N
    declare_glyph(LATIN_LOWERCASE_N);
    /// Латинская строчная буква O
    declare_glyph(LATIN_LOWERCASE_O);
    /// Латинская строчная буква P
    declare_glyph(LATIN_LOWERCASE_P);
    /// Латинская строчная буква Q
    declare_glyph(LATIN_LOWERCASE_Q);
    /// Латинская строчная буква R
    declare_glyph(LATIN_LOWERCASE_R);
    /// Латинская строчная буква S
    declare_glyph(LATIN_LOWERCASE_S);
    /// Латинская строчная буква T
    declare_glyph(LATIN_LOWERCASE_T);
    /// Латинская строчная буква U
    declare_glyph(LATIN_LOWERCASE_U);
    /// Латинская строчная буква V
    declare_glyph(LATIN_LOWERCASE_V);
    /// Латинская строчная буква W
    declare_glyph(LATIN_LOWERCASE_W);
    /// Латинская строчная буква X
    declare_glyph(LATIN_LOWERCASE_X);
    /// Латинская строчная буква Y
    declare_glyph(LATIN_LOWERCASE_Y);
    /// Латинская строчная буква Z
    declare_glyph(LATIN_LOWERCASE_Z);

    /// === Цифры ===

    /// Цифра 0
    declare_glyph(NUMBER_0);
    /// Цифра 1
    declare_glyph(NUMBER_1);
    /// Цифра 2
    declare_glyph(NUMBER_2);
    /// Цифра 3
    declare_glyph(NUMBER_3);
    /// Цифра 4
    declare_glyph(NUMBER_4);
    /// Цифра 5
    declare_glyph(NUMBER_5);
    /// Цифра 6
    declare_glyph(NUMBER_6);
    /// Цифра 7
    declare_glyph(NUMBER_7);
    /// Цифра 8
    declare_glyph(NUMBER_8);
    /// Цифра 9
    declare_glyph(NUMBER_9);

    /// === Пунктуационные знаки ===

    /// Точка
    declare_glyph(DOT);
    /// Запятая
    declare_glyph(COMMA);
    /// Двоеточие
    declare_glyph(COLON);
    /// Точка с запятой
    declare_glyph(SEMICOLON);
    /// Восклицательный знак
    declare_glyph(EXCLAMATION_POINT);
    /// Вопросительный знак
    declare_glyph(QUESTION_MARK);
    /// "Собачка"
    declare_glyph(AT_SYMBOL);
    /// Решётка
    declare_glyph(HASH);
    /// Символ доллара
    declare_glyph(DOLLAR_SIGN);
    /// Процент
    declare_glyph(PERCENTAGE);
    /// Стрелочка вверх (которая Shift+6)
    declare_glyph(UP);
    /// Амперсанд
    declare_glyph(AMPERSAND);
    /// Звёздочка
    declare_glyph(STAR);
    /// Круглая открывающаяся скобка
    declare_glyph(OPEN_ROUND_BRACKET);
    /// Круглая закрывающаяся скобка
    declare_glyph(CLOSE_ROUND_BRACKET);
    /// Квадратная открывающаяся скобка
    declare_glyph(OPEN_SQUARE_BRACKET);
    /// Квадратная закрывающаяся скобка
    declare_glyph(CLOSE_SQUARE_BRACKET);
    /// Фигурная открывающаяся скобка
    declare_glyph(OPEN_CURLY_BRACKET);
    /// Фигурная закрывающаяся скобка
    declare_glyph(CLOSE_CURLY_BRACKET);
    /// Минус
    declare_glyph(MINUS);
    /// Плюс
    declare_glyph(PLUS);
    /// Равно
    declare_glyph(EQUALS);
    /// Слэш
    declare_glyph(SLASH);
    /// Бэкслэш
    declare_glyph(BACKSLASH);
    /// Прямая черта ("|")
    declare_glyph(LINE);
    /// Знак меньше
    declare_glyph(LESS_SYMBOL);
    /// Знак больше
    declare_glyph(MORE_SYMBOL);
    /// Апостроф
    declare_glyph(APOSTROPHE);
    /// Кавычка
    declare_glyph(QUOTATION_MARK);
    /// Тильда
    declare_glyph(TILDA);
    /// Акут
    declare_glyph(ACUTE);
    /// Нижнее подчёркивание
    declare_glyph(UNDERSCORE);

    /// === Кириллица ===

    /// Кириллическая заглавная буква А
    declare_glyph(CYRILLIC_UPPERCASE_A);
    /// Кириллическая заглавная буква Б
    declare_glyph(CYRILLIC_UPPERCASE_BE);
    /// Кириллическая заглавная буква В
    declare_glyph(CYRILLIC_UPPERCASE_VE);
    /// Кириллическая заглавная буква Г
    declare_glyph(CYRILLIC_UPPERCASE_GE);
    /// Кириллическая заглавная буква Д
    declare_glyph(CYRILLIC_UPPERCASE_DE);
    /// Кириллическая заглавная буква Е
    declare_glyph(CYRILLIC_UPPERCASE_YE);
    /// Кириллическая заглавная буква Ё
    declare_glyph(CYRILLIC_UPPERCASE_YO);
    /// Кириллическая заглавная буква Ж
    declare_glyph(CYRILLIC_UPPERCASE_ZHE);
    /// Кириллическая заглавная буква З
    declare_glyph(CYRILLIC_UPPERCASE_ZE);
    /// Кириллическая заглавная буква И
    declare_glyph(CYRILLIC_UPPERCASE_I);
    /// Кириллическая заглавная буква Й
    declare_glyph(CYRILLIC_UPPERCASE_I_KRATKOYE);
    /// Кириллическая заглавная буква К
    declare_glyph(CYRILLIC_UPPERCASE_KA);
    /// Кириллическая заглавная буква Л
    declare_glyph(CYRILLIC_UPPERCASE_EL);
    /// Кириллическая заглавная буква М
    declare_glyph(CYRILLIC_UPPERCASE_EM);
    /// Кириллическая заглавная буква Н
    declare_glyph(CYRILLIC_UPPERCASE_EN);
    /// Кириллическая заглавная буква О
    declare_glyph(CYRILLIC_UPPERCASE_O);
    /// Кириллическая заглавная буква П
    declare_glyph(CYRILLIC_UPPERCASE_PE);
    /// Кириллическая заглавная буква Р
    declare_glyph(CYRILLIC_UPPERCASE_ER);
    /// Кириллическая заглавная буква С
    declare_glyph(CYRILLIC_UPPERCASE_ES);
    /// Кириллическая заглавная буква Т
    declare_glyph(CYRILLIC_UPPERCASE_TE);
    /// Кириллическая заглавная буква У
    declare_glyph(CYRILLIC_UPPERCASE_U);
    /// Кириллическая заглавная буква Ф
    declare_glyph(CYRILLIC_UPPERCASE_EF);
    /// Кириллическая заглавная буква Х
    declare_glyph(CYRILLIC_UPPERCASE_HA);
    /// Кириллическая заглавная буква Ц
    declare_glyph(CYRILLIC_UPPERCASE_TSE);
    /// Кириллическая заглавная буква Ч
    declare_glyph(CYRILLIC_UPPERCASE_CHE);
    /// Кириллическая заглавная буква Ш
    declare_glyph(CYRILLIC_UPPERCASE_SHA);
    /// Кириллическая заглавная буква Щ
    declare_glyph(CYRILLIC_UPPERCASE_SHYA);
    /// Кириллическая заглавная буква Ъ
    declare_glyph(CYRILLIC_UPPERCASE_TVERDIY_ZNAK);
    /// Кириллическая заглавная буква Ы
    declare_glyph(CYRILLIC_UPPERCASE_UI);
    /// Кириллическая заглавная буква Ь
    declare_glyph(CYRILLIC_UPPERCASE_MYAGKIY_ZNAK);
    /// Кириллическая заглавная буква Э
    declare_glyph(CYRILLIC_UPPERCASE_IE);
    /// Кириллическая заглавная буква Ю
    declare_glyph(CYRILLIC_UPPERCASE_YU);
    /// Кириллическая заглавная буква Я
    declare_glyph(CYRILLIC_UPPERCASE_YA);
    /// Кириллическая строчная буква А
    declare_glyph(CYRILLIC_LOWERCASE_A);
    /// Кириллическая строчная буква Б
    declare_glyph(CYRILLIC_LOWERCASE_BE);
    /// Кириллическая строчная буква В
    declare_glyph(CYRILLIC_LOWERCASE_VE);
    /// Кириллическая строчная буква Г
    declare_glyph(CYRILLIC_LOWERCASE_GE);
    /// Кириллическая строчная буква Д
    declare_glyph(CYRILLIC_LOWERCASE_DE);
    /// Кириллическая строчная буква Е
    declare_glyph(CYRILLIC_LOWERCASE_YE);
    /// Кириллическая строчная буква Ё
    declare_glyph(CYRILLIC_LOWERCASE_YO);
    /// Кириллическая строчная буква Ж
    declare_glyph(CYRILLIC_LOWERCASE_ZHE);
    /// Кириллическая строчная буква З
    declare_glyph(CYRILLIC_LOWERCASE_ZE);
    /// Кириллическая строчная буква И
    declare_glyph(CYRILLIC_LOWERCASE_I);
    /// Кириллическая строчная буква Й
    declare_glyph(CYRILLIC_LOWERCASE_I_KRATKOYE);
    /// Кириллическая строчная буква К
    declare_glyph(CYRILLIC_LOWERCASE_KA);
    /// Кириллическая строчная буква Л
    declare_glyph(CYRILLIC_LOWERCASE_EL);
    /// Кириллическая строчная буква М
    declare_glyph(CYRILLIC_LOWERCASE_EM);
    /// Кириллическая строчная буква Н
    declare_glyph(CYRILLIC_LOWERCASE_EN);
    /// Кириллическая строчная буква О
    declare_glyph(CYRILLIC_LOWERCASE_O);
    /// Кириллическая строчная буква П
    declare_glyph(CYRILLIC_LOWERCASE_PE);
    /// Кириллическая строчная буква Р
    declare_glyph(CYRILLIC_LOWERCASE_ER);
    /// Кириллическая строчная буква С
    declare_glyph(CYRILLIC_LOWERCASE_ES);
    /// Кириллическая строчная буква Т
    declare_glyph(CYRILLIC_LOWERCASE_TE);
    /// Кириллическая строчная буква У
    declare_glyph(CYRILLIC_LOWERCASE_U);
    /// Кириллическая строчная буква Ф
    declare_glyph(CYRILLIC_LOWERCASE_EF);
    /// Кириллическая строчная буква Х
    declare_glyph(CYRILLIC_LOWERCASE_HA);
    /// Кириллическая строчная буква Ц
    declare_glyph(CYRILLIC_LOWERCASE_TSE);
    /// Кириллическая строчная буква Ч
    declare_glyph(CYRILLIC_LOWERCASE_CHE);
    /// Кириллическая строчная буква Ш
    declare_glyph(CYRILLIC_LOWERCASE_SHA);
    /// Кириллическая строчная буква Щ
    declare_glyph(CYRILLIC_LOWERCASE_SHYA);
    /// Кириллическая строчная буква Ъ
    declare_glyph(CYRILLIC_LOWERCASE_TVERDIY_ZNAK);
    /// Кириллическая строчная буква Ы
    declare_glyph(CYRILLIC_LOWERCASE_UI);
    /// Кириллическая строчная буква Ь
    declare_glyph(CYRILLIC_LOWERCASE_MYAGKIY_ZNAK);
    /// Кириллическая строчная буква Э
    declare_glyph(CYRILLIC_LOWERCASE_IE);
    /// Кириллическая строчная буква Ю
    declare_glyph(CYRILLIC_LOWERCASE_YU);
    /// Кириллическая строчная буква Я
    declare_glyph(CYRILLIC_LOWERCASE_YA);
}

#undef declare_glyph

#endif //_GLYPHS_INCL
