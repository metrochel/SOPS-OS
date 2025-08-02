/*
 * inttypes.c
 *
 *
 */

#include "include/inttypes.h"

#include "include/ctype.h"
#include "include/wctype.h"

intmax_t imaxabs(intmax_t x) {
    return x < 0 ? -x : x;
}

imaxdiv_t imaxdiv(intmax_t x, intmax_t y) {
    return (imaxdiv_t){ x / y, x % y };
}

#define strtoimax_exit(num, _CHR)           \
    if (str_end) *str_end = (_CHR*)str;     \
    return negative && (-num < 0) ? -num : num;

#define strtoimax_code(_CHR, _INT, _ISDIGIT, _ISLOWER, _TOUPPER) {  \
    int negative = 0;                                               \
    if (*str == '+')                                                \
        str++;                                                      \
    else if (*str == '-') {                                         \
        negative = 1;                                               \
        str++;                                                      \
    }                                                               \
    if (base == 0) {                                                \
        if (*str == '0') {                                          \
            base = 8;                                               \
            str++;                                                  \
            if (*str == 'x' || *str == 'X') {                       \
                base = 16;                                          \
                str++;                                              \
            }                                                       \
        } else                                                      \
        base = 10;                                                  \
    }                                                               \
    _INT number = 0;                                                \
    while (_ISDIGIT(*str)) {                                        \
        _CHR digit = *str++;                                        \
        if (_ISLOWER(digit))                                        \
            digit = _TOUPPER(digit);                                \
        if (digit >= 'A' && base > 10)                              \
            digit -= 'A' - 10;                                      \
        if (digit >= base)                                          \
            break;                                                  \
        number = base * number + digit;                             \
    }                                                               \
    strtoimax_exit(number, _CHR)                                    \
}

intmax_t strtoimax(const char *str, char **str_end, int base)
strtoimax_code(char, intmax_t, isdigit, islower, toupper)

uintmax_t strtoumax(const char *str, char **str_end, int base)
strtoimax_code(char, uintmax_t, isdigit, islower, toupper)

intmax_t wcstoimax(const wchar_t *str, wchar_t **str_end, int base)
strtoimax_code(wchar_t, intmax_t, iswdigit, iswlower, towupper)

uintmax_t wcstoumax(const wchar_t *str, wchar_t **str_end, int base)
strtoimax_code(wchar_t, uintmax_t, iswdigit, iswlower, towupper)
