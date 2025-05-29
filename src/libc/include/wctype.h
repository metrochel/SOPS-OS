//
//  wctype.h - Классификация широких символов
//
//  Позволяет узнавать информацию о широких символах.
//

#if !defined _WCTYPE_INCL && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199500L

#include <etc/decl.h>
#include <etc/wint_t.h>

#undef WEOF
#define WEOF (wint_t)(-1)

typedef struct {
    
} wctype_t;

typedef struct {
    
} wctrans_t;

// Проверяет, является ли символ буквой или цифрой.
int iswalnum(wint_t c);

// Проверяет, является ли символ буквой.
int iswalpha(wint_t c);

// Проверяет, что символ в нижнем регистре.
int iswlower(wint_t c);

// Проверяет, что символ в верхнем регистре.
int iswupper(wint_t c);

// Проверяет, является ли символ цифрой.
int iswdigit(wint_t c);

// Проверяет, является ли символ шестнадцатеричной цифрой.
int iswxdigit(wint_t c);

// Проверяет, является ли символ непечатным управляющим символом.
int iswcntrl(wint_t c);

// Проверяет, является ли символ графическим.
int iswgraph(wint_t c);

// Проверяет, является ли символ пробелом.
int iswspace(wint_t c);

// Проверяет, является ли символ пустым.  
int iswblank(wint_t c);

// Проверяет, является ли символ печатным.
int iswprint(wint_t c);

// Проверяет, является ли символ пунктуационным знаком.
int iswpunct(wint_t c);

// Устанавливает тип широкого символа `c` в соответствии с локалью `LC_CTYPE`.
int iswctype(wint_t c);

// Создаёт значение для классификации широких символов на основании строки `str`.
wctype_t wctype(const char *str);

// Переводит широкий символ `c` в нижний регистр.
wint_t towlower(wint_t c);

// Переводит символ в широкий регистр.
wint_t towupper(wint_t c);

// Переводит символ в соответствии с таблицей `trans`.
wint_t towctrans(wint_t c, wctrans_t trans);

// Создаёт таблицу для перевода широких символов на основе строки `str`.
wctrans_t wctrans_t(const char *str);

#endif