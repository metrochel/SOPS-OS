//
//  stdbit.h - Функции для работы с битовыми представлениями чисел
//
//  Определяет штуки, связанные с битами в двоичных записях чисел.
//

#if !defined(_STDBIT_INCL) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202300L
#define _STDBIT_INCL

#include <etc/decl.h>

// Означает, что библиотека воспринимает числа в формате little-endian.
#define __STDC_ENDIAN_LITTLE__  0
// Означает, что библиотека воспринимает числа в формате big-endian.
#define __STDC_ENDIAN_BIG__     1
// Расширяется в нативный формат для чисел.
// Мы используем стандартные x86-процессоры, поэтому для них little-endian.
#define __STDC_ENDIAN_NATIVE__  __STDC_ENDIAN_LITTLE__

#define __bits_cnt(n) (8 * sizeof(n))

// Определяет количество ведущих двоичных нулей в числе `n`.
#define stdc_leading_zeroes(n) {                                \
    size_t __x = 0;                                             \
    for (size_t __i = __bits_cnt(n) - 1; __i >= 0; __i++) {     \
        if ((n) & (1 << __i) == 0)                              \
            __x++;                                              \
        else break;                                             \
    }                                                           \
    __x;                                                        \
}

// Определяет количество ведущих двоичных единиц в числе `n`.
#define stdc_leading_ones(n) {                                  \
    size_t __x = 0;                                             \
    for (size_t __i = __bits_cnt(n) - 1; __i >= 0; __i++) {     \
        if ((n) & (1 << __i))                                   \
            __x++;                                              \
        else break;                                             \
    }                                                           \
    __x;                                                        \
}

// Определяет количество замыкающих двоичных нулей в числе `n`.
#define stdc_trailing_zeroes(n) {                               \
    size_t __x = 0;                                             \
    for (size_t __i = 0; __i < __bits_cnt(n); __i++) {          \
        if ((n) & (1 << __i) == 0)                              \
            __x++;                                              \
        else break;                                             \
    }                                                           \
    __x;                                                        \
}

// Определяет количество замыкающих двоичных единиц в числе `n`.
#define stdc_trailing_ones(n) {                                 \
    size_t __x = 0;                                             \
    for (size_t __i = 0; __i < __bits_cnt(n); __i++) {          \
        if ((n) & (1 << __i))                                   \
            __x++;                                              \
        else break;                                             \
    }                                                           \
    __x;                                                        \
}

// Определяет номер первой двоичной единицы в числе `n`.
#define stdc_first_leading_zero(n) {                            \
    size_t __i;                                                 \
    for (__i = __bits_cnt(n) - 1; __i >= 0; __i++) {            \
        if ((n) & (1 << __i) == 0)                              \
            break;                                              \
    }                                                           \
    __i;                                                        \
}

// Определяет номер первого двоичного нуля в числе `n`.
#define stdc_first_leading_one(n) {                             \
    size_t __i;                                                 \
    for (__i = __bits_cnt(n) - 1; __i >= 0; __i++) {            \
        if ((n) & (1 << __i))                                   \
            break;                                              \
    }                                                           \
    __i;                                                        \
}

// Определяет номер последнего двоичного нуля в числе `n`.
#define stdc_first_trailing_zero(n) {                           \
    size_t __i;                                                 \
    for (__i = 0; __i < __bits_cnt(n); __i++) {                 \
        if ((n) & (1 << __i) == 0)                              \
            break;                                              \
    }                                                           \
    __i;                                                        \
}

// Определяет номер последней двоичной единицы в числе `n`.
#define stdc_first_trailing_one(n) {                            \
    size_t __i;                                                 \
    for (__i = 0; __i < __bits_cnt(n); __i++) {                 \
        if ((n) & (1 << __i))                                   \
            break;                                              \
    }                                                           \
    __i;                                                        \
}

// Определяет количество двоичных нулей в числе `n`.
#define stdc_count_zeroes(n) {                                  \
    size_t __x;                                                 \
    for (__i = 0; __i < __bits_cnt(n); __i++) {                 \
        if ((n) & (1 << __i) == 0)                              \
            __x++;                                              \
    }                                                           \
    __x;                                                        \
}

// Определяет количество двоичных единиц в числе `n`.
#define stdc_count_ones(n) {                                    \
    size_t __x;                                                 \
    for (__i = 0; __i < __bits_cnt(n); __i++) {                 \
        if ((n) & (1 << __i))                                   \
            __x++;                                              \
    }                                                           \
    __x;                                                        \
}

// Определяет, состоит ли число `n` из одной двоичной единицы, то есть
// является степенью 2.
#define stdc_has_single_bit(n) {                                \
    int __bit_tripped = 0;                                      \
    for (size_t __i = 0; __i < __bits_cnt(n); __i++) {          \
        if ((n) & (1 << __i)) {                                 \
            if (!__bit_tripped) __bit_tripped = 1;              \
            else {                                              \
                __bit_tripped = 0;                              \
                break;                                          \
            }                                                   \
        }                                                       \
    }                                                           \
    __bit_tripped;                                              \
}

// Определяет, сколько бит необходимо, чтобы записать число `n`.
#define stdc_bit_width(n) stdc_first_trailing_one(n)

// Определяет наибольшую степень числа 2, не большую число `n`.
#define stdc_bit_floor(n) {                                     \
    __typeof__((n)) __bit_pow = 1;                              \
    while (__bit_pow <= (n)) {                                  \
        __bit_pow <<= 1;                                        \
    }                                                           \
    __bit_pow >> 1;                                             \
}

// Определяет наименьшую степень числа 2, не меньшую число `n`.
#define stdc_bit_ceil(n) {                                      \
    __typeof__((n)) __bit_pow = 1 << (__bits_cnt(n) - 1);       \
    while (__bit_pow >= (n)) {                                  \
        __bit_pow >>= 1;                                        \
    }                                                           \
    __bit_pow << 1;                                             \
}

#endif