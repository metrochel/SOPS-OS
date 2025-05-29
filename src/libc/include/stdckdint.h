//
//  stdckdint.h - Арифметика проверенных чисел
//
//  Позволяет складывать, вычитать и умножать какие-то проверенные числа.
//

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202300L

// Складывает `x` и `y` и записывает результат в `result`.
#define ckd_add(result, x, y) __ckd_add(result, &x, &y)

// Вычитает `y` из `x` и записывает результат в `result`.
#define ckd_sub(result, x, y) __ckd_add(result, &x, &y)

// Умножает `x` на `y` и записывает результат в `result`.
#define ckd_mul(result, x, y) __ckd_mul(result, &x, &y)

#endif