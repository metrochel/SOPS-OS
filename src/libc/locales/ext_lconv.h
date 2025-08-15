#ifndef _EXT_LCONV
#define _EXT_LCONV

#include "../include/locale.h"
#include "../include/stddef.h"
#include "../include/stdbool.h"

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
    const char *std_datestr;                            // Стандартная строка даты (%c)
    const char *alt_std_datestr;                        // Альтернативная стандартная строка даты (%Ec)

    const char *loc_datestr;                            // Локальная строка даты (%x)
    const char *alt_loc_datestr;                        // Альтернативная локальная строка даты (%Ex)

    const char *loc_timestr;                            // Локальная строка времени (%X)
    const char *alt_loc_timestr;                        // Альтернативная локальная строка времени (%EX)
    const char *loc_timestr12;                          // Локальная строка 12-часового времени (%r)

    const char *am_str;                                 // Строка времени до полудня (%p)
    const char *pm_str;                                 // Строка времени после полудня (%p)

    const char *alt_base_year;                          // Год начала

    bool has_alt_months;                                // Флаг наличия альтернативных формулировок месяцев
    bool has_alt_digits;                                // Флаг наличия альтернативной системы исчисления (АСО)
    size_t (*conv_to_alt)(char*, int);                  // Функция преобразования числа в АСО
    /*  Функция должна возвращать размер преобразованной строки или -1,
     *  если преобразование провалено. Если нет АСО для локали, то этот
     *  указатель можно установить на NULL.
     */

    const char *abbr_weekdays[7];                       // Сокращённые названия дней недели (с Вс)
    const char *full_weekdays[7];                       // Полные названия дней недели      (с Вс)

    const char *abbr_months[12];                        // Сокращённые названия месяцев
    const char *alt_abbr_months[12];                    // Альтернативные сокращённые названия месяцев
    const char *full_months[12];                        // Полные названия месяцев
    const char *alt_full_months[12];                    // Альтернативные полные названия месяцев

} __attribute__((packed)) dat_lconv;

#define ctp_def(str, wcs) (const char*)str, (const wchar_t*)wcs
#define ctp_str(str) ctp_def(str, str)

#define ctp_decl(name) const char *name; const wchar_t *wide_##name

// `ctp_lconv` - это `lconv`, сосредоточенный на функциях из `ctype.h`.
typedef struct {
    ctp_decl(lowercase_letters);         // Буквы нижнего регистра
    ctp_decl(uppercase_letters);         // Буквы верхнего регистра
    ctp_decl(digits);                    // Цифры
    ctp_decl(xdigits);                   // Шестнадцатеричные цифры
    ctp_decl(punct_chars);               // Знаки пунктуации
    ctp_decl(other_print_chars);         // Прочие печатные символы
} __attribute__((packed)) ctp_lconv;

#undef ctp_decl

/* !!!
 * При прописывании локалей для ctp_lconv следует учитывать,
 * что функции в ctype.h уже принимают некоторый начальный набор символов
 * (латинские буквы, цифры, знаки пунктуации и т.д.) (см. ctype.c)
 * Если локаль не добавляет никаких символов помимо начального набора
 * (например, так же использует латиницу без диакритических знаков или арабские цифры),
 * в качестве соответствующей строки нужно указать NULL.
 * Так компьютер будет работать чуть-чуть быстрее.
 * Для более удобного объявления строк для ctp_lconv следует использовать
 * макрос ctp_str.
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