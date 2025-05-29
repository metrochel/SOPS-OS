//
//  locale.h - Локализация
//
//  Позволяет настроить программу на определённую локацию/язык (например, Россия (русский)).
//

#ifndef _LOCALE_INCL
#define _LOCALE_INCL

#include <etc/decl.h>

#define __need_NULL
#include <stddef.h>

// Категории настроек локалей

// Всё
#define LC_ALL      1
// Поведение функций strcoll и strxfrm
#define LC_COLLATE  2
// Поведение функций из ctype.h
#define LC_CTYPE    3
// Денежки
#define LC_MONETARY 4
// Числа
#define LC_NUMERIC  5
// Время
#define LC_TIME     6

BEGIN_DECLS

// `lconv` - структура, описывающая текущие локальные настройки.
typedef struct {
    char *decimal_point;
    char *thousands_sep;
    char *grouping;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;
    char *currency_symbol;
    char frac_digits;
    char p_cs_precedes;
    char n_cs_precedes;
    char p_sep_by_space;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    char *int_curr_symbol;
    char int_frac_digits;
    char int_p_cs_precedes;
    char int_n_cs_precedes;
    char int_p_sep_by_space;
    char int_n_sep_by_space;
    char int_p_sign_posn;
    char int_n_sign_posn;
} lconv;

// Изменяет локаль на `locale` в категории `category`.
char* setlocale(int category, const char *locale);

// Получает параметры текущей локали.
lconv* localeconv();

END_DECLS

#endif