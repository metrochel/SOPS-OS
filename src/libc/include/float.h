//
//  float.h - Числа с плавающей точкой
//
//  Определяет макросы для работы с float, double и т.д.
//

#ifndef _FLOAT_INCL
#define _FLOAT_INCL 1

// Для всех макросов описано, к какому типу они относятся:
// FLT - к float;
// DBL - к double;
// LDBL - к long double.

// Режим округления
#define FLT_ROUNDS  (__builtin_flt_rounds())

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L
    // Режим формата приведения результатов вычисления
    #define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#endif

// Основание одной цифры в мантиссе (2 <=> цифра = бит)
#define FLT_RADIX __FLT_RADIX__

// Число цифр (бит) в мантиссе
#define FLT_MANT_DIG  __FLT_MANT_DIG__
#define DBL_MANT_DIG  __DBL_MANT_DIG__
#define LDBL_MANT_DIG __LDBL_MANT_DIG__

#if (defined(__cplusplus) && __cplusplus >= 201100L) \
    || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L)
    // Минимальное число десятичных цифр, с которым производится приведение long double к десятичному числу
    #define DECIMAL_DIG __DECIMAL_DIG__
#endif

#if defined(__cplusplus) && __cplusplus >= 201700L \
    || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L)
    // Минимальное число десятичных цифр, с которым производится приведение типа к десятичному числу
    #define FLT_DECIMAL_DIG  __FLT_DECIMAL_DIG__
    #define DBL_DECIMAL_DIG  __DBL_DECIMAL_DIG__
    #define LDBL_DECIMAL_DIG __LDBL_DECIMAL_DIG__
#endif

// Наименьшее возможное нормализованное положительное число
#define FLT_MIN  __FLT_MIN__
#define DBL_MIN  __DBL_MIN__
#define LDBL_MIN __LDBL_MIN__

#if defined(__cplusplus) && __cplusplus >= 201700L \
    || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L)
    // Наименьшее возможное ненормализованное положительное число
    #define FLT_TRUE_MIN  __FLT_DENORM_MIN__
    #define DBL_TRUE_MIN  __DBL_DENORM_MIN__
    #define LDBL_TRUE_MIN __LDBL_DENORM_MIN__
#endif

// Наибольшее возможное число
#define FLT_MAX  __FLT_MAX__
#define DBL_MAX  __DBL_MAX__
#define LDBL_MAX __LDBL_MAX__

// Наименьшее число e такое, что верно 1.0 + e != 1.0
#define FLT_EPSILON  __FLT_EPSILON__
#define DBL_EPSILON  __DBL_EPSILON__
#define LDBL_EPSILON __LDBL_EPSILON__

// Количество десятичных цифр, которые будут гарантированно верны
// при переводе числа в текстовый вид
#define FLT_DIG __FLT_DIG__
#define DBL_DIG __DBL_DIG__
#define LDBL_DIG __LDBL_DIG__

// Количество цифр мантиссы по основанию FLT_RADIX (то есть бит), которые
// могут быть записаны без потери точности
#define FLT_MANT_DIG  __FLT_MANT_DIG__
#define DBL_MANT_DIG  __DBL_MANT_DIG__
#define LDBL_MANT_DIG __LDBL_MANT_DIG__

// Наименьшая возможная степень такая, что FLT_RADIX в степени
// на 1 меньше будет нормализованным числом
#define FLT_MIN_EXP  __FLT_MIN_EXP__
#define DBL_MIN_EXP  __DBL_MIN_EXP__
#define LDBL_MIN_EXP __LDBL_MIN_EXP__

// Наименьшая возможная степень такая, что 10 в этой степени
// будет нормализованным числом
#define FLT_MIN_10_EXP  __FLT_MIN_10_EXP__
#define DBL_MIN_10_EXP  __DBL_MIN_10_EXP__
#define LDBL_MIN_10_EXP __LDBL_MIN_10_EXP__

// Наибольшая возможная степень такая, что FLT_RADIX в степени на 1 меньше
// будет конечным записываемым числом
#define FLT_MAX_EXP  __FLT_MAX_EXP__
#define DBL_MAX_EXP  __DBL_MAX_EXP__
#define LDBL_MAX_EXP __LDBL_MAX_EXP__

// Наибольшая возможная степень такая, что 10 в этой степени
// будет конечным записываемым числом
#define FLT_MAX_10_EXP  __FLT_MAX_10_EXP__
#define DBL_MAX_10_EXP  __DBL_MAX_10_EXP__
#define LDBL_MAX_10_EXP __LDBL_MAX_10_EXP__

#if defined(__cplusplus) && __cplusplus >= 201700L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L
    #define FLT_HAS_SUBNORM __FLT_HAS_DENORM__
    #define DBL_HAS_SUBNORM __DBL_HAS_DENORM__
    #define LDBL_HAS_SUBNORM __LDBL_HAS_DENORM__
#endif

#endif