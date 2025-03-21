#ifndef _GLYPHS_INCL
#define _GLYPHS_INCL

#include <stdarg.h>
#include "graphics.hpp"
#include "../util/nums.hpp"
// ======================================== Работа с текстом ============================================

#define LF 0x0A
#define CR 0x0D

// Абсцисса курсора текста
extern word textCurX;
// Ордината курсора текста
extern word textCurY;

extern word auxTextCurX;
extern word auxTextCurY;
extern dword drawLine;

extern word textLeftBoundX;
extern word textLeftBoundY;
extern word textRightBoundX;
extern word textRightBoundY;

// Стандартный цвет текста
extern dword defaultTextCol;
// Стандартный цвет заднего фона текста
extern dword defaultBGCol;
// Цвет текста предупреждения
extern dword warnTextCol;
// Цвет заднего фона предупреждения
extern dword warnBGCol;
// Цвет текста ошибки
extern dword errorTextCol;
// Цвет заднего фона ошибки
extern dword errorBGCol;

// Символ
struct Glyph {
    word lines[24];      // Бит-поля, отмечающие пикселы символа
} __attribute__((packed));

/// @brief Инициализирует работу с текстом.
void initText();

/// @brief Выдаёт разметку символа, воспринимаемую системой.
/// @param code Код символа в Юникоде
/// @param unicode Флаг Юникода
/// @return Разметка
Glyph getglyph(word code);

/// @brief Выдаёт разметку символа, воспринимаемую системой.
/// @param code Код символа в ASCII
/// @return Разметка
Glyph getglyph(byte code);

/// @brief Фиксирует один символ в буфере текстового вывода.
/// @param x Абсцисса символа (текстовые координаты)
/// @param y Ордината символа (текстовые координаты)
/// @param c Символ в кодировке UTF-8
/// @param textCol Цвет символа
/// @param bgCol Цвет заднего фона символа
inline void setchar(word x, word y, dword c, dword textCol, dword bgCol) {
    dword write = y*textScreenWidth + x;
    if (3*write + 2 > textBufferBankSize * 2)
        return;

    textBuffer[write] = {c,textCol,bgCol};
}

/// @brief Размещает символ на координатах.
/// @param glyph Символ
/// @param x Абсцисса символа (отн. левого верхнего угла)
/// @param y Ордината символа (отн. левого верхнего угла)
/// @param letter_col Цвет самого символа
/// @param back_col Цвет заднего фона символа
inline void putglyph(Glyph glyph, word x, word y, dword letter_col, dword back_col) {
    dword offset = y * pitch + x * (bpp/8);
    for (byte i = 0; i < 24; i++) {
        dword line = glyph.lines[i];
        word mask = 1 << 15;
        for (int j = 0; j < 16; j++) {
            dword pixcol = (line & mask) ? letter_col : back_col;
            putpixel(offset, pixcol);
            mask >>= 1;
            offset += bpp/8;
        }
        offset -= 16*(bpp/8);
        offset += pitch;
    }
}

/// @brief Выключает курсор.
void disableCursor();

/// @brief Включает курсор.
void enableCursor();

/// @brief Выводит на экран число в двоичном представлении.
/// @param num Число
/// @param charCol Цвет числа
/// @param bgCol Цвет заднего фона
void printBinUInt(qword num, dword charCol, dword bgCol);

/// @brief Выводит на экран число в восьмеричном представлении.
/// @param num Число
/// @param charCol Цвет числа
/// @param bgCol Цвет заднего фона
void printOctUInt(qword num, dword charCol, dword bgCol);

/// @brief Выводит на экран число.
/// @param num Число
/// @param charCol Цвет числа
/// @param bgCol Цвет заднего фона
void printDecUInt(qword num, dword charCol, dword bgCol);

/// @brief Выводит на экран число в шестнадцатеричном представлении.
/// @param num Число
/// @param charCol Цвет числа
/// @param bgCol Цвет заднего фона
void printHexUInt(qword num, dword charCol, dword bgCol);

/// @brief Выводит на экран дробное число.
/// @param num Число
/// @param charCol Цвет числа
/// @param bgCol Цвет заднего фона
void printFloat(double num, dword charCol, dword bgCol);

/// @brief Стирает один символ
void eraseChar();

/// @brief Обновляет курсор.
void updateCursor();

/// @brief Заново отрисовывает экран по данным буфера.
void refreshScreen();

/// @brief Листает экран на одну строчку вниз.
void scroll();

// ========================================

/// @brief Выводит строку на экран.
/// @param text Строка
/// @param args Аргументы для форматирования
/// @param charCol Цвет текста строки
/// @param bgCol Цвет заднего фона строки
/// @param format Флаг; `true`, если строка должна форматироваться
void printStr(const char* text, va_list args, dword charCol, dword bgCol, bool format);

/// @brief Выводит на экран простой текст.
/// @param text Форматированный текст
void kprint(const char* text, ...);

/// @brief Выводит на экран предупреждение.
/// @param text Форматированное сообщение предупреждения
void kwarn(const char* text, ...);

/// @brief Выводит на экран ошибку.
/// @param text Форматированное сообщение ошибки
void kerror(const char* text, ...);

// ========================================

// Нулевой символ
extern const Glyph NULLGLYPH;
// Несуществующий символ
extern const Glyph INVALIDCHAR;
// Текстовый курсор
extern const Glyph CURSOR;

// Латинский алфавит

// Латинская заглавная буква A
extern const Glyph LATIN_UPPERCASE_A;
// Латинская заглавная буква B
extern const Glyph LATIN_UPPERCASE_B;
// Латинская заглавная буква C
extern const Glyph LATIN_UPPERCASE_C;
// Латинская заглавная буква D
extern const Glyph LATIN_UPPERCASE_D;
// Латинская заглавная буква E
extern const Glyph LATIN_UPPERCASE_E;
// Латинская заглавная буква F
extern const Glyph LATIN_UPPERCASE_F;
// Латинская заглавная буква G
extern const Glyph LATIN_UPPERCASE_G;
// Латинская заглавная буква H
extern const Glyph LATIN_UPPERCASE_H;
// Латинская заглавная буква I
extern const Glyph LATIN_UPPERCASE_I;
// Латинская заглавная буква J
extern const Glyph LATIN_UPPERCASE_J;
// Латинская заглавная буква K
extern const Glyph LATIN_UPPERCASE_K;
// Латинская заглавная буква L
extern const Glyph LATIN_UPPERCASE_L;
// Латинская заглавная буква M
extern const Glyph LATIN_UPPERCASE_M;
// Латинская заглавная буква N
extern const Glyph LATIN_UPPERCASE_N;
// Латинская заглавная буква O
extern const Glyph LATIN_UPPERCASE_O;
// Латинская заглавная буква P
extern const Glyph LATIN_UPPERCASE_P;
// Латинская заглавная буква Q
extern const Glyph LATIN_UPPERCASE_Q;
// Латинская заглавная буква R
extern const Glyph LATIN_UPPERCASE_R;
// Латинская заглавная буква S
extern const Glyph LATIN_UPPERCASE_S;
// Латинская заглавная буква T
extern const Glyph LATIN_UPPERCASE_T;
// Латинская заглавная буква U
extern const Glyph LATIN_UPPERCASE_U;
// Латинская заглавная буква V
extern const Glyph LATIN_UPPERCASE_V;
// Латинская заглавная буква W
extern const Glyph LATIN_UPPERCASE_W;
// Латинская заглавная буква X
extern const Glyph LATIN_UPPERCASE_X;
// Латинская заглавная буква Y
extern const Glyph LATIN_UPPERCASE_Y;
// Латинская заглавная буква Z
extern const Glyph LATIN_UPPERCASE_Z;
// Латинская строчная буква A
extern const Glyph LATIN_LOWERCASE_A;
// Латинская строчная буква B
extern const Glyph LATIN_LOWERCASE_B;
// Латинская строчная буква C
extern const Glyph LATIN_LOWERCASE_C;
// Латинская строчная буква D
extern const Glyph LATIN_LOWERCASE_D;
// Латинская строчная буква E
extern const Glyph LATIN_LOWERCASE_E;
// Латинская строчная буква F
extern const Glyph LATIN_LOWERCASE_F;
// Латинская строчная буква G
extern const Glyph LATIN_LOWERCASE_G;
// Латинская строчная буква H
extern const Glyph LATIN_LOWERCASE_H;
// Латинская строчная буква I
extern const Glyph LATIN_LOWERCASE_I;
// Латинская строчная буква J
extern const Glyph LATIN_LOWERCASE_J;
// Латинская строчная буква K
extern const Glyph LATIN_LOWERCASE_K;
// Латинская строчная буква L
extern const Glyph LATIN_LOWERCASE_L;
// Латинская строчная буква M
extern const Glyph LATIN_LOWERCASE_M;
// Латинская строчная буква N
extern const Glyph LATIN_LOWERCASE_N;
// Латинская строчная буква O
extern const Glyph LATIN_LOWERCASE_O;
// Латинская строчная буква P
extern const Glyph LATIN_LOWERCASE_P;
// Латинская строчная буква Q
extern const Glyph LATIN_LOWERCASE_Q;
// Латинская строчная буква R
extern const Glyph LATIN_LOWERCASE_R;
// Латинская строчная буква S
extern const Glyph LATIN_LOWERCASE_S;
// Латинская строчная буква T
extern const Glyph LATIN_LOWERCASE_T;
// Латинская строчная буква U
extern const Glyph LATIN_LOWERCASE_U;
// Латинская строчная буква V
extern const Glyph LATIN_LOWERCASE_V;
// Латинская строчная буква W
extern const Glyph LATIN_LOWERCASE_W;
// Латинская строчная буква X
extern const Glyph LATIN_LOWERCASE_X;
// Латинская строчная буква Y
extern const Glyph LATIN_LOWERCASE_Y;
// Латинская строчная буква Z
extern const Glyph LATIN_LOWERCASE_Z;

// Цифры

// Цифра 0
extern const Glyph NUMBER_0;
// Цифра 1
extern const Glyph NUMBER_1;
// Цифра 2
extern const Glyph NUMBER_2;
// Цифра 3
extern const Glyph NUMBER_3;
// Цифра 4
extern const Glyph NUMBER_4;
// Цифра 5
extern const Glyph NUMBER_5;
// Цифра 6
extern const Glyph NUMBER_6;
// Цифра 7
extern const Glyph NUMBER_7;
// Цифра 8
extern const Glyph NUMBER_8;
// Цифра 9
extern const Glyph NUMBER_9;

// Пунктуационные знаки

// Точка
extern const Glyph DOT;
// Запятая
extern const Glyph COMMA;
// Двоеточие
extern const Glyph COLON;
// Точка с запятой
extern const Glyph SEMICOLON;
// Восклицательный знак
extern const Glyph EXCLAMATION_POINT;
// Вопросительный знак
extern const Glyph QUESTION_MARK;
// "Собачка"
extern const Glyph AT_SYMBOL;
// Решётка
extern const Glyph HASH;
// Символ доллара
extern const Glyph DOLLAR_SIGN;
// Процент
extern const Glyph PERCENTAGE;
// Стрелочка вверх (которая Shift+6)
extern const Glyph UP;
// Амперсанд
extern const Glyph AMPERSAND;
// Звёздочка
extern const Glyph STAR;
// Круглая открывающаяся скобка
extern const Glyph OPEN_ROUND_BRACKET;
// Круглая закрывающаяся скобка
extern const Glyph CLOSE_ROUND_BRACKET;
// Квадратная открывающаяся скобка
extern const Glyph OPEN_SQUARE_BRACKET;
// Квадратная закрывающаяся скобка
extern const Glyph CLOSE_SQUARE_BRACKET;
// Фигурная открывающаяся скобка
extern const Glyph OPEN_CURLY_BRACKET;
// Фигурная закрывающаяся скобка
extern const Glyph CLOSE_CURLY_BRACKET;
// Минус
extern const Glyph MINUS;
// Плюс
extern const Glyph PLUS;
// Равно
extern const Glyph EQUALS;
// Слэш
extern const Glyph SLASH;
// Бэкслэш
extern const Glyph BACKSLASH;
// Прямая черта ("|")
extern const Glyph LINE;
// Знак меньше
extern const Glyph LESS_SYMBOL;
// Знак больше
extern const Glyph MORE_SYMBOL;
// Апостроф
extern const Glyph APOSTROPHE;
// Кавычка
extern const Glyph QUOTATION_MARK;
// Тильда
extern const Glyph TILDA;
// Акут
extern const Glyph ACUTE;
// Нижнее подчёркивание
extern const Glyph UNDERSCORE;

// Кириллица

// Кириллическая заглавная буква А
extern const Glyph CYRILLIC_UPPERCASE_A;
// Кириллическая заглавная буква Б
extern const Glyph CYRILLIC_UPPERCASE_BE;
// Кириллическая заглавная буква В
extern const Glyph CYRILLIC_UPPERCASE_VE;
// Кириллическая заглавная буква Г
extern const Glyph CYRILLIC_UPPERCASE_GE;
// Кириллическая заглавная буква Д
extern const Glyph CYRILLIC_UPPERCASE_DE;
// Кириллическая заглавная буква Е
extern const Glyph CYRILLIC_UPPERCASE_YE;
// Кириллическая заглавная буква Ё
extern const Glyph CYRILLIC_UPPERCASE_YO;
// Кириллическая заглавная буква Ж
extern const Glyph CYRILLIC_UPPERCASE_ZHE;
// Кириллическая заглавная буква З
extern const Glyph CYRILLIC_UPPERCASE_ZE;
// Кириллическая заглавная буква И
extern const Glyph CYRILLIC_UPPERCASE_I;
// Кириллическая заглавная буква Й
extern const Glyph CYRILLIC_UPPERCASE_I_KRATKOYE;
// Кириллическая заглавная буква К
extern const Glyph CYRILLIC_UPPERCASE_KA;
// Кириллическая заглавная буква Л
extern const Glyph CYRILLIC_UPPERCASE_EL;
// Кириллическая заглавная буква М
extern const Glyph CYRILLIC_UPPERCASE_EM;
// Кириллическая заглавная буква Н
extern const Glyph CYRILLIC_UPPERCASE_EN;
// Кириллическая заглавная буква О
extern const Glyph CYRILLIC_UPPERCASE_O;
// Кириллическая заглавная буква П
extern const Glyph CYRILLIC_UPPERCASE_PE;
// Кириллическая заглавная буква Р
extern const Glyph CYRILLIC_UPPERCASE_ER;
// Кириллическая заглавная буква С
extern const Glyph CYRILLIC_UPPERCASE_ES;
// Кириллическая заглавная буква Т
extern const Glyph CYRILLIC_UPPERCASE_TE;
// Кириллическая заглавная буква У
extern const Glyph CYRILLIC_UPPERCASE_U;
// Кириллическая заглавная буква Ф
extern const Glyph CYRILLIC_UPPERCASE_EF;
// Кириллическая заглавная буква Х
extern const Glyph CYRILLIC_UPPERCASE_HA;
// Кириллическая заглавная буква Ц
extern const Glyph CYRILLIC_UPPERCASE_TSE;
// Кириллическая заглавная буква Ч
extern const Glyph CYRILLIC_UPPERCASE_CHE;
// Кириллическая заглавная буква Ш
extern const Glyph CYRILLIC_UPPERCASE_SHA;
// Кириллическая заглавная буква Щ
extern const Glyph CYRILLIC_UPPERCASE_SHYA;
// Кириллическая заглавная буква Ъ
extern const Glyph CYRILLIC_UPPERCASE_TVERDIY_ZNAK;
// Кириллическая заглавная буква Ы
extern const Glyph CYRILLIC_UPPERCASE_UI;
// Кириллическая заглавная буква Ь
extern const Glyph CYRILLIC_UPPERCASE_MYAGKIY_ZNAK;
// Кириллическая заглавная буква Э
extern const Glyph CYRILLIC_UPPERCASE_IE;
// Кириллическая заглавная буква Ю
extern const Glyph CYRILLIC_UPPERCASE_YU;
// Кириллическая заглавная буква Я
extern const Glyph CYRILLIC_UPPERCASE_YA;
// Кириллическая строчная буква А
extern const Glyph CYRILLIC_LOWERCASE_A;
// Кириллическая строчная буква Б
extern const Glyph CYRILLIC_LOWERCASE_BE;
// Кириллическая строчная буква В
extern const Glyph CYRILLIC_LOWERCASE_VE;
// Кириллическая строчная буква Г
extern const Glyph CYRILLIC_LOWERCASE_GE;
// Кириллическая строчная буква Д
extern const Glyph CYRILLIC_LOWERCASE_DE;
// Кириллическая строчная буква Е
extern const Glyph CYRILLIC_LOWERCASE_YE;
// Кириллическая строчная буква Ё
extern const Glyph CYRILLIC_LOWERCASE_YO;
// Кириллическая строчная буква Ж
extern const Glyph CYRILLIC_LOWERCASE_ZHE;
// Кириллическая строчная буква З
extern const Glyph CYRILLIC_LOWERCASE_ZE;
// Кириллическая строчная буква И
extern const Glyph CYRILLIC_LOWERCASE_I;
// Кириллическая строчная буква Й
extern const Glyph CYRILLIC_LOWERCASE_I_KRATKOYE;
// Кириллическая строчная буква К
extern const Glyph CYRILLIC_LOWERCASE_KA;
// Кириллическая строчная буква Л
extern const Glyph CYRILLIC_LOWERCASE_EL;
// Кириллическая строчная буква М
extern const Glyph CYRILLIC_LOWERCASE_EM;
// Кириллическая строчная буква Н
extern const Glyph CYRILLIC_LOWERCASE_EN;
// Кириллическая строчная буква О
extern const Glyph CYRILLIC_LOWERCASE_O;
// Кириллическая строчная буква П
extern const Glyph CYRILLIC_LOWERCASE_PE;
// Кириллическая строчная буква Р
extern const Glyph CYRILLIC_LOWERCASE_ER;
// Кириллическая строчная буква С
extern const Glyph CYRILLIC_LOWERCASE_ES;
// Кириллическая строчная буква Т
extern const Glyph CYRILLIC_LOWERCASE_TE;
// Кириллическая строчная буква У
extern const Glyph CYRILLIC_LOWERCASE_U;
// Кириллическая строчная буква Ф
extern const Glyph CYRILLIC_LOWERCASE_EF;
// Кириллическая строчная буква Х
extern const Glyph CYRILLIC_LOWERCASE_HA;
// Кириллическая строчная буква Ц
extern const Glyph CYRILLIC_LOWERCASE_TSE;
// Кириллическая строчная буква Ч
extern const Glyph CYRILLIC_LOWERCASE_CHE;
// Кириллическая строчная буква Ш
extern const Glyph CYRILLIC_LOWERCASE_SHA;
// Кириллическая строчная буква Щ
extern const Glyph CYRILLIC_LOWERCASE_SHYA;
// Кириллическая строчная буква Ъ
extern const Glyph CYRILLIC_LOWERCASE_TVERDIY_ZNAK;
// Кириллическая строчная буква Ы
extern const Glyph CYRILLIC_LOWERCASE_UI;
// Кириллическая строчная буква Ь
extern const Glyph CYRILLIC_LOWERCASE_MYAGKIY_ZNAK;
// Кириллическая строчная буква Э
extern const Glyph CYRILLIC_LOWERCASE_IE;
// Кириллическая строчная буква Ю
extern const Glyph CYRILLIC_LOWERCASE_YU;
// Кириллическая строчная буква Я
extern const Glyph CYRILLIC_LOWERCASE_YA;

#endif