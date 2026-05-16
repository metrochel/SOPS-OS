/*
 * strtonum_base.c
 *
 *
 */

#include "../include/math.h"

#define concat(x, y) x##y
#define func(n, s) concat(n, s)

#define strtox_end(number) \
    if (negative) number = -number;     \
    if (str_end) *str_end = (CHAR_T*)str; \
    return number

#define strtoi_code(_INT) {                                            \
    if (base > 36) {                                                   \
        if (str_end) *str_end = (CHAR_T*)str;                          \
        return -1;                                                     \
    }                                                                  \
    int negative = 0;                                                  \
    if (*str == '+')                                                   \
        str++;                                                         \
    else if (*str == '-') {                                            \
        negative = 1;                                                  \
        str++;                                                         \
    }                                                                  \
    if (base == 0) {                                                   \
        if (*str++ == '0') {                                           \
            base = 8;                                                  \
            if (*str == 'x' || *str == 'X') {                          \
                base = 16;                                             \
                str++;                                                 \
            }                                                          \
        } else                                                         \
            base = 10;                                                 \
    }                                                                  \
    _INT number = 0;                                                   \
    while (ISDIGIT(*str)) {                                            \
        _INT digit = *str++;                                           \
        if (digit > 10 && base > 10) {                                 \
            if (ISLOWER(digit)) digit = TOUPPER(digit);                \
            digit = 'A' - digit + 10;                                  \
        } else                                                         \
            digit -= '0';                                              \
        if (digit > base || digit < 0) {                               \
            strtox_end(number);                                        \
        }                                                              \
        number = base * number + digit;                                \
    }                                                                  \
    strtox_end(number);                                                \
}

long func(STRTO,l) (const CHAR_T *str, CHAR_T **str_end, int base) strtoi_code(long)
long long func(STRTO,ll) (const CHAR_T *str, CHAR_T **str_end, int base) strtoi_code(long long)
unsigned long func(STRTO,ul) (const CHAR_T *str, CHAR_T **str_end, int base) strtoi_code(unsigned long)
unsigned long long func(STRTO,ull) (const CHAR_T *str, CHAR_T **str_end, int base) strtoi_code(unsigned long long)

#define strtod_code(_FLT, _EXT) {                                                                              \
    while (ISSPACE(*str))                                                                                      \
        str++;                                                                                                 \
    _FLT number = 0.0;                                                                                         \
    int negative = 0;                                                                                          \
    if (*str == '-') {                                                                                         \
        str++;                                                                                                 \
        negative = 1;                                                                                          \
    }                                                                                                          \
    else if (*str == '+') str++;                                                                               \
    if (TOLOWER(str[0]) == 'n' && TOLOWER(str[1]) == 'a' && TOLOWER(str[2]) == 'n') {                          \
        str += 3;                                                                                              \
        if (*str != '(') {                                                                                     \
            number = copysign##_EXT(NAN, number);                                                              \
            strtox_end(number);                                                                                \
        }                                                                                                      \
        str++;                                                                                                 \
        number = copysign##_EXT(nan##_EXT((char*)str), number);                                                \
        CHAR_T *jmp = STRPBRK(str, (CHAR_T*)")");                                                              \
        if (!jmp)                                                                                              \
            return NAN;                                                                                        \
        str = ++jmp;                                                                                           \
        strtox_end(number);                                                                                    \
    }                                                                                                          \
    if (TOLOWER(str[0]) == 'i' && TOLOWER(str[2]) == 'n' && TOLOWER(str[2]) == 'f') {                          \
        str += 3;                                                                                              \
        if (TOLOWER(str[0]) == 'i' && TOLOWER(str[1]) == 'n' && TOLOWER(str[2]) == 'i' &&                      \
        TOLOWER(str[3]) == 't' && TOLOWER(str[4]) == 'y')                                                      \
            str += 5;                                                                                          \
        number = copysign##_EXT(INFINITY, number);                                                             \
        strtox_end(number);                                                                                    \
    }                                                                                                          \
    int hex = 0;                                                                                               \
    int exponent = 0;                                                                                          \
    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))                                               \
        hex = 1;                                                                                               \
    if (hex) {                                                                                                 \
        while (*str && ISXDIGIT(*str)) {                                                                       \
            int digit = *str - '0';                                                                            \
            if (digit >= 10)                                                                                   \
                digit = *str - 'A' + 10;                                                                       \
            if (digit >= 16)                                                                                   \
                digit = *str - 'a' + 10;                                                                       \
            if (digit >= 16 || digit < 0) {                                                                    \
                strtox_end(number);                                                                            \
            }                                                                                                  \
            if (__signbit##_EXT(number))                                                                       \
                digit = -digit;                                                                                \
            number = 16 * number + digit;                                                                      \
            str++;                                                                                             \
        }                                                                                                      \
        if (!*str) {                                                                                           \
            strtox_end(number);                                                                                \
        }                                                                                                      \
        if (STRSTR(str, (CHAR_T*)localeconv()->decimal_point) == str) {                                        \
            _FLT mult = 1 / 16;                                                                                \
            while (*str && ISXDIGIT(*str)) {                                                                   \
                int digit = *str - '0';                                                                        \
                if (digit >= 10)                                                                               \
                    digit = *str - 'A' + 10;                                                                   \
                if (digit >= 16)                                                                               \
                    digit = *str - 'a' + 10;                                                                   \
                if (digit >= 16 || digit < 0) {                                                                \
                    strtox_end(number);                                                                        \
                }                                                                                              \
                if (__signbit##_EXT(number))                                                                   \
                    digit = -digit;                                                                            \
                number += digit * mult;                                                                        \
                mult /= 16;                                                                                    \
            }                                                                                                  \
        }                                                                                                      \
        if (!*str) {                                                                                           \
            strtox_end(number);                                                                                \
        }                                                                                                      \
        if (*str == 'p' || *str == 'P') {                                                                      \
            str++;                                                                                             \
            exponent = func(STRTO,l)(str, (CHAR_T**)&str, 10);                                               \
            number *= pow##_EXT(2, exponent);                                                                  \
        }                                                                                                      \
    }                                                                                                          \
    else {                                                                                                     \
        while (*str && ISDIGIT(*str)) {                                                                        \
            int digit = *str - '0';                                                                            \
            if (digit >= 10 || digit < 0) {                                                                    \
                strtox_end(number);                                                                            \
            }                                                                                                  \
            if (__signbit##_EXT(number))                                                                       \
                digit = -digit;                                                                                \
            number = 10 * number + digit;                                                                      \
            str++;                                                                                             \
        }                                                                                                      \
        if (!*str) {                                                                                           \
            strtox_end(number);                                                                                \
        }                                                                                                      \
        CHAR_T *dec = (CHAR_T*)localeconv()->decimal_point;                                                    \
        if (STRSTR(str, dec) == str) {                                                                         \
            str += STRLEN(dec);                                                                                \
            while (*str && ISDIGIT(*str)) {                                                                    \
                int digit = *str - '0';                                                                        \
                if (digit >= 10 || digit < 0) {                                                                \
                    strtox_end(number);                                                                        \
                }                                                                                              \
                if (__signbit##_EXT(number))                                                                   \
                    digit = -digit;                                                                            \
                number = 10 * number + digit;                                                                  \
                str++;                                                                                         \
            }                                                                                                  \
        }                                                                                                      \
        if (STRSTR(str, (CHAR_T*)localeconv()->decimal_point) == str) {                                        \
            _FLT mult = 1 / 10;                                                                                \
            while (*str && ISDIGIT(*str)) {                                                                    \
                int digit = *str - '0';                                                                        \
                if (digit >= 10 || digit < 0) {                                                                \
                    strtox_end(number);                                                                        \
                }                                                                                              \
                if (__signbit##_EXT(number))                                                                   \
                    digit = -digit;                                                                            \
                number += digit * mult;                                                                        \
                mult /= 10;                                                                                    \
                str++;                                                                                         \
            }                                                                                                  \
        }                                                                                                      \
        if (*str == 'e' || *str == 'E') {                                                                      \
            str++;                                                                                             \
            exponent = func(STRTO,l)(str, (CHAR_T**)&str, 10);                                               \
            number *= pow##_EXT(10, exponent);                                                                 \
        }                                                                                                      \
    }                                                                                                          \
    strtox_end(number);                                                                                        \
}

float func(STRTO,f) (const CHAR_T *str, CHAR_T **str_end) strtod_code(float, f)
double func(STRTO,d) (const CHAR_T *str, CHAR_T **str_end) strtod_code(double, )
long double func(STRTO,ld) (const CHAR_T *str, CHAR_T **str_end) strtod_code(long double, l)

#undef strtod_code

#undef ISSPACE
#undef ISDIGIT
#undef ISXDIGIT
#undef ISLOWER
#undef TOUPPER
#undef STRSTR
#undef STRTO