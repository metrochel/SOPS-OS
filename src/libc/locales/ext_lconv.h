#ifndef _EXT_LCONV
#define _EXT_LCONV

#include "../include/locale.h"

typedef int (*eval_func)(int);

// `num_lconv` - это `lconv`, сосредоточенный на числах.
typedef struct {
    char *decimal_point;        // Строка-разделитель целой и дробной части числа
    char *thousands_sep;        // Строка-разделитель тысяч
    char *grouping;             // Группировка цифр
} __attribute__((packed)) num_lconv;


// `mon_lconv` - это `lconv`, сосредоточенный на деньгах.
typedef struct {
    char *mon_decimal_point;    // Строка-разделитель целой и дробной части денежной суммы
    char *mon_thousands_sep;    // Строка-разделитель тысяч денежной суммы
    char *mon_grouping;         // Группировка цифр денежной суммы
    char *positive_sign;        // Знак положительной денежной суммы
    char *negative_sign;        // Знак отрицательной денежной суммы
    char *currency_symbol;      // Символ валюты
    char frac_digits;           // Количество чисел дробной части валюты
    char p_cs_precedes;         // Указатель на положение символа валюты в отношении к положительной денежной сумме
    char n_cs_precedes;         // Указатель на положение символа валюты в отношении к отрицательной денежной сумме
    char p_sep_by_space;        // Указатель на деление пробелами
                                // символа валюты, знака и значения положительной денежной суммы
    char n_sep_by_space;        // Указатель на деление пробелами
                                // символа валюты, знака и значения положительной денежной суммы
    char p_sign_posn;           // Положение знака в положительной денежной сумме
    char n_sign_posn;           // Положение знака в отрицательной денежной сумме
    char *int_curr_symbol;      // Международное обозначение валюты
    char int_frac_digits;       // Как `frac_digits`, только международное
    char int_p_cs_precedes;     // Как `p_cs_precedes`, только международное
    char int_n_cs_precedes;     // Как `n_cs_precedes`, только международное
    char int_p_sep_by_space;    // Как `p_sep_by_space`, только международное
    char int_n_sep_by_space;    // Как `n_sep_by_space`, только международное
    char int_p_sign_posn;       // Как `p_sign_posn`, только международное
    char int_n_sign_posn;       // Как `n_sign_posn`, только международное
} __attribute__((packed)) mon_lconv;


// `dat_lconv` - это `lconv`, сосредоточенный на дате и времени.
typedef struct {
    char *date_format_str;          // Строка форматирования даты
    // Используется форматирование функции `strftime`.
    // Написанный в `date_format_str` формат подставляется вместо %c.

    char *mon_short_str;            // Сокращённое имя понедельника (%a)
    char *tue_short_str;            // Сокращённое имя вторника (%a)
    char *wed_short_str;            // Сокращённое имя среды (%a)
    char *thu_short_str;            // Сокращённое имя четверга (%a)
    char *fri_short_str;            // Сокращённое имя пятницы (%a)
    char *sat_short_str;            // Сокращённое имя субботы (%a)
    char *sun_short_str;            // Сокращённое имя воскресенья (%a)

    char *mon_long_str;             // Полное имя понедельника (%A)
    char *tue_long_str;             // Полное имя вторника (%A)
    char *wed_long_str;             // Полное имя среды (%A)
    char *thu_long_str;             // Полное имя четверга (%A)
    char *fri_long_str;             // Полное имя пятницы (%A)
    char *sat_long_str;             // Полное имя субботы (%A)
    char *sun_long_str;             // Полное имя воскресенья (%A)

    char *jan_short_str;            // Сокращённое имя января (%b)
    char *feb_short_str;            // Сокращённое имя февраля (%b)
    char *mar_short_str;            // Сокращённое имя марта (%b)
    char *apr_short_str;            // Сокращённое имя апреля (%b)
    char *may_short_str;            // Сокращённое имя мая (%b)
    char *jun_short_str;            // Сокращённое имя июня (%b)
    char *jul_short_str;            // Сокращённое имя июля (%b)
    char *aug_short_str;            // Сокращённое имя августа (%b)
    char *sep_short_str;            // Сокращённое имя сентября (%b)
    char *oct_short_str;            // Сокращённое имя октября (%b)
    char *nov_short_str;            // Сокращённое имя ноября (%b)
    char *dec_short_str;            // Сокращённое имя декабря (%b)

    char *jan_long_str;             // Полное имя января (%B)
    char *feb_long_str;             // Полное имя февраля (%B)
    char *mar_long_str;             // Полное имя марта (%B)
    char *apr_long_str;             // Полное имя апреля (%B)
    char *may_long_str;             // Полное имя мая (%B)
    char *jun_long_str;             // Полное имя июня (%B)
    char *jul_long_str;             // Полное имя июля (%B)
    char *aug_long_str;             // Полное имя августа (%B)
    char *sep_long_str;             // Полное имя сентября (%B)
    char *oct_long_str;             // Полное имя октября (%B)
    char *nov_long_str;             // Полное имя ноября (%B)
    char *dec_long_str;             // Полное имя декабря (%B)

} __attribute__((packed)) dat_lconv;


// `ctp_lconv` - это `lconv`, сосредоточенный на функциях из `ctype.h`.
typedef struct {
    char *lowercase_letters;            // Буквы нижнего регистра
    char *uppercase_letters;            // Буквы верхнего регистра
    char *digits;                       // Цифры
    char *xdigits;                      // Шестнадцатеричные цифры
    char *punct_chars;                  // Знаки пунктуации
    char *other_print_chars;            // Прочие печатные символы
} __attribute__((packed)) ctp_lconv;

/* !!!
 * При прописывании локалей для ctp_lconv следует учитывать,
 * что функции в ctype.h уже принимают некоторый начальный набор символов
 * (латинские буквы, цифры, знаки пунктуации и т.д.) (см. ctype.c)
 * Если локаль не добавляет никаких символов помимо начального набора
 * (например, так же использует латиницу без диакритических знаков или арабские цифры),
 * в качестве соответствующей строки нужно указать NULL.
 * Так компьютер будет работать чуть-чуть быстрее.
 */

// `col_lconv` - это `lconv`, сосредоточенный на сравнении символов.
typedef struct {
    char **order;                       // Порядок единиц сравнения (оканчивается (char**)(NULL))
} __attribute__((packed)) col_lconv;

// Это немного расширенный lconv, который хранит больше данных о
// текущей локали.
typedef struct {
    num_lconv nums;         // Числовые настройки локали
    mon_lconv money;        // Финансовые настройки локали
    dat_lconv dates;        // Дата-временные настройки локали
    ctp_lconv ctype;        // Настройки локали для функций из `ctype.h`
    col_lconv coll;         // Настройки локали для функции `strcoll`
} __attribute__((packed)) ext_lconv;

extern ext_lconv locale;    // Текущая локаль

#endif