/*
 *
 *  math.h
 *
 */
#include "include/math.h"
#include "include/limits.h"
#include "include/float.h"
#include "include/fenv.h"
#include "include/stdio.h"
#include "include/stdlib.h"

/* ============================================   Макросы   ======================================== */

#define p2xm1(x) (((unsigned long)1 << (x)) - 1)

#define decl_math_funcf(func_ret, func_name, func_args, func_code) \
    func_ret(float) func_name##f func_args(float) func_code(float, f)

#define decl_math_funcd(func_ret, func_name, func_args, func_code) \
    func_ret(double) func_name func_args(double) func_code(double, )

#define decl_math_funcl(func_ret, func_name, func_args, func_code) \
    func_ret(long double) func_name##l func_args(long double) func_code(long double, l)

#define decl_math_func(func_ret, func_name, func_args, func_code)   \
    decl_math_funcf(func_ret, func_name, func_args, func_code)      \
    decl_math_funcd(func_ret, func_name, func_args, func_code)      \
    decl_math_funcl(func_ret, func_name, func_args, func_code)

#define decl decl_math_func(func_ret, func_name, func_args, func_code)

#define _UNI(_EXT) _EXT##union_t

#define _B(_EXT) b_##_EXT

#define _L(_EXT) l_##_EXT

#define _P(_EXT) p_##_EXT

#define _MAX_EXP(_EXT) p2xm1(_P(_EXT))

#define _EPS(x) _Generic((x), \
    float: FLT_EPSILON,       \
    default: DBL_EPSILON,     \
    long double: LDBL_EPSILON)

#define b_f 127
#define l_f 23
#define p_f 8

#define b_ 1023
#define l_ 52
#define p_ 11

#define b_l 4095
#define l_l 112
#define p_l 15

/* ============================================     Типы    ======================================== */

typedef struct {
    unsigned int mantissa : 23;
    unsigned int exponent : 8;
    unsigned int sign     : 1;
} fbits_t;

typedef union {
    float number;
    fbits_t bits;
} funion_t;

typedef struct {
    unsigned long long mantissa : 52;
    unsigned long long exponent : 11;
    unsigned long long sign     : 1;
} bits_t;

typedef union {
    double number;
    bits_t bits;
} union_t;

typedef struct {
    unsigned long long mantissa1;
    unsigned long long mantissa2 : 48;
    unsigned long long exponent  : 15;
    unsigned long long sign      : 1;
} lbits_t;

typedef union {
    long double number;
    lbits_t bits;
} lunion_t;

/* ============================================ Постоянные ========================================= */

/* Значения постоянных взяты с Онлайн-энциклопедии целочисленных последовательностей
 * (OEIS, oeis.org)
 * Надеюсь, они не против (:
 */

/* Число Эйлера */
const float Ef          = 2.7182818284590452353602874713527;
const double E          = 2.7182818284590452353602874713527;
const long double El    = 2.7182818284590452353602874713527;

/* Натуральный логарифм 2 (ln 2) */
const float LN2f        = 0.6931471805599453094172321214582;
const double LN2        = 0.6931471805599453094172321214582;
const long double LN2l  = 0.6931471805599453094172321214582;

/* Натуральный логарифм 10 (ln 10) */
const float LN10f       = 2.3025850929940456840179914546844;
const double LN10       = 2.3025850929940456840179914546844;
const long double LN10l = 2.3025850929940456840179914546844;

/* Число пи */
const float PIf         = 3.1415926535897932384626433832795;
const double PI         = 3.1415926535897932384626433832795;
const long double PIl   = 3.1415926535897932384626433832795;

/* ============================================   Функции   ======================================== */

/* fabs */

#define func_ret(_FLT) _FLT
#define func_name fabs
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    _UNI(_EXT) uni;             \
    uni.number = x;             \
    uni.bits.sign = 0;          \
    return uni.number;          \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* fmod */

#define func_ret(_FLT) _FLT
#define func_name fmod
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) {                     \
    y = fabs##_EXT(y);                              \
    _FLT res = remainder##_EXT(fabs##_EXT(x), y);   \
    if (__signbit##_EXT(res))                       \
        res += y;                                   \
    return copysign##_EXT(res, x);                  \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* remainder */

#define func_ret(_FLT) _FLT
#define func_name remainder
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) {                     \
    if (__isnan##_EXT(x) || __isnan##_EXT(y))       \
        return NAN;                                 \
    if (y == 0.0 || __isinf##_EXT(x)) {             \
        feraiseexcept(FE_INVALID);                  \
        return NAN;                                 \
    }                                               \
    _FLT xy = x / y;                                \
    _FLT txy = round##_EXT(xy);                     \
    _FLT n = txy;                                   \
    _FLT result;                                    \
    if (fabs##_EXT(xy - txy) == 0.5) {              \
        result = x - n * y;                         \
        if (result / 2 == trunc##_EXT(result / 2)) {\
            n = txy + 1;                            \
            result = x - n * y;                     \
        }                                           \
    } else                                          \
        result = x - n * y;                         \
    return result;                                  \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* remquo */

#define func_ret(_FLT) _FLT
#define func_name remquo
#define func_args(_FLT) (_FLT x, _FLT y, int *quo)
#define func_code(_FLT, _EXT) {                     \
    if (__isnan##_EXT(x) || __isnan##_EXT(y))       \
        return NAN;                                 \
    if (y == 0.0 || __isinf##_EXT(x)) {             \
        feraiseexcept(FE_INVALID);                  \
        return NAN;                                 \
    }                                               \
    _FLT xy = x / y;                                \
    _FLT txy = round##_EXT(xy);                     \
    _FLT n = txy;                                   \
    _FLT result;                                    \
    if (fabs##_EXT(xy - txy) == 0.5) {              \
        result = x - n * y;                         \
        if (result / 2 == trunc##_EXT(result / 2)) {\
            n = txy + 1;                            \
            result = x - n * y;                     \
        }                                           \
    } else                                          \
        result = x - n * y;                         \
    int quot = ((int)n) & 7;                        \
    if (n < 0)                                      \
        quot = -quot;                               \
    *quo = quot;                                    \
    return result;                                  \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* fma */

#define func_ret(_FLT) _FLT
#define func_name fma
#define func_args(_FLT) (_FLT x, _FLT y, _FLT z)
#define func_code(_FLT, _EXT) { \
    return (x * y) + z;         \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* fmax */

#define func_ret(_FLT) _FLT
#define func_name fmax
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) { \
    if (__isnan##_EXT(x))       \
        return y;               \
    if (__isnan##_EXT(y))       \
        return x;               \
    if (isgreater(x, y))        \
        return x;               \
    else                        \
        return y;               \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* fmin */

#define func_ret(_FLT) _FLT
#define func_name fmin
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) { \
    if (__isnan##_EXT(x))       \
        return y;               \
    if (__isnan##_EXT(y))       \
        return x;               \
    if (isless(x, y))           \
        return x;               \
    else                        \
        return y;               \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* fdim */

#define func_ret(_FLT) _FLT
#define func_name fdim
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) {                 \
    if (__isnan##_EXT(x) || __isnan##_EXT(y))   \
        return NAN;                             \
    return fmax(0.0, x - y);                    \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* nan */

#define func_ret(_FLT) _FLT
#define func_name nan
#define func_args(_FLT) (const char *str)
#define func_code(_FLT, _EXT) {         \
    char buf[strlen(str) + 6];          \
    sprintf(buf, "NAN(%s)", str);       \
    return strtof(buf, (char**)NULL);   \
}

decl_math_funcf(func_ret, nan, func_args, func_code)

#undef func_code
#define func_code(_FLT, _EXT) {                 \
    char buf[strlen(str) + 6];                  \
    sprintf(buf, "NAN(%s)", str);               \
    return strto##_EXT##d(buf, (char**)NULL);   \
}

decl_math_funcd(func_ret, nan, func_args, func_code)
decl_math_funcl(func_ret, nan, func_args, func_code)

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* __int_pow */

#define func_ret(_FLT) _FLT
#define func_name __int_pow
#define func_args(_FLT) (_FLT x, long long y)
#define func_code(_FLT, _EXT) { \
    _FLT xy = 1;                \
    while (y) {                 \
        if (y & 1) {            \
            xy *= x;            \
            y --;               \
        }                       \
        x *= x;                 \
        y >>= 1;                \
    }                           \
    return xy;                  \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* __flt_exp */

#define func_ret(_FLT) _FLT
#define func_name __flt_exp
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    _FLT ex = 1 + x;            \
    _FLT fac = 2;               \
    _FLT px = x * x;            \
    int fac_no = 3;             \
    while (px / fac > _EPS(x)) {\
        ex += px / fac;         \
        px *= x;                \
        fac *= fac_no;          \
        fac_no ++;              \
    }                           \
    return ex;                  \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* exp */

#define func_ret(_FLT) _FLT
#define func_name exp
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                         \
    if (x == 0.0)                                       \
        return 1.0;                                     \
    if (__isinf##_EXT(x) && __signbit##_EXT(x))         \
        return 0.0;                                     \
    if (__isinf##_EXT(x) && !__signbit##_EXT(x))        \
        return INFINITY;                                \
    if (__isnan##_EXT(x))                               \
        return NAN;                                     \
    long long int_pow = (long long)(trunc##_EXT(x));    \
    _FLT frac_pow = x - int_pow;                        \
    _FLT eix = __int_pow##_EXT(E##_EXT, int_pow);       \
    _FLT efx = __flt_exp##_EXT(frac_pow);               \
    return eix * efx;                                   \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* exp2 */

#define func_ret(_FLT) _FLT
#define func_name exp2
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    return exp##_EXT(x * LN2##_EXT);    \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* expm1 */

#define func_ret(_FLT) _FLT
#define func_name expm1
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    return exp##_EXT(x) - 1;    \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* log */

#define func_ret(_FLT) _FLT
#define func_name log
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                \
    if (__isnan##_EXT(x))                      \
        return NAN;                            \
    if (__signbit##_EXT(x)) {                  \
        __raise_domain_err();                  \
        feraiseexcept(FE_INVALID);             \
        return NAN;                            \
    }                                          \
    if (x == 0.0) {                            \
        feraiseexcept(FE_DIVBYZERO);           \
        return -INFINITY;                      \
    }                                          \
    if (x == 1.0)                              \
        return +0.0;                           \
    if (__isinf##_EXT(x))                      \
        return INFINITY;                       \
    _FLT z = 1 - 2 / (x + 1);                  \
    _FLT lnz = 2 * z;                          \
    _FLT pz = 2 * z * z * z;                   \
    _FLT z2 = z * z;                           \
    int pow_no = 3;                            \
    while (pz / pow_no > _EPS(_EXT)) {         \
        lnz += pz / pow_no;                    \
        pz *= z2;                              \
        pow_no += 2;                           \
    }                                          \
    return lnz;                                \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* log10 */

#define func_ret(_FLT) _FLT
#define func_name log10
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    return log##_EXT(x) / LN10##_EXT;   \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* log2 */

#define func_ret(_FLT) _FLT
#define func_name log2
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    return log##_EXT(x) / LN2##_EXT;    \
}

decl

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* pow */

#define func_ret(_FLT) _FLT
#define func_name pow
#define func_args(_FLT) (_FLT base, _FLT exponent)
#define func_code(_FLT, _EXT) {                                                                \
    _FLT int_exponent = trunc##_EXT(exponent);                                                 \
    if (base == -1.0 && __isinf##_EXT(exponent))                                               \
        return 1.0;                                                                            \
    if (base == 1.0)                                                                           \
        return 1.0;                                                                            \
    if (exponent == 0.0)                                                                       \
        return 1.0;                                                                            \
    if (isfinite(base) && base < 0 && isfinite(exponent) && exponent != int_exponent) {        \
        __raise_domain_err();                                                                  \
        return NAN;                                                                            \
    }                                                                                          \
    if (base == 0.0) {                                                                         \
        if (exponent == 0.0) {                                                                 \
            __raise_domain_err();                                                              \
            return NAN;                                                                        \
        }                                                                                      \
        if (__signbit##_EXT(exponent) && isfinite(exponent)) {                                 \
            feraiseexcept(FE_DIVBYZERO);                                                       \
            if (int_exponent == exponent && fmod##_EXT(exponent, 2) != 0.0) {                  \
                return copysign##_EXT(INFINITY, base);                                         \
            }                                                                                  \
            else {                                                                             \
                return INFINITY;                                                               \
            }                                                                                  \
        }                                                                                      \
        if (!__signbit##_EXT(exponent) && isfinite(exponent)) {                                \
            if (int_exponent == exponent && fmod##_EXT(exponent, 2) != 0.0) {                  \
                return copysign##_EXT(0.0, base);                                              \
            }                                                                                  \
            else {                                                                             \
                return +0.0;                                                                   \
            }                                                                                  \
        }                                                                                      \
        if (__isinf##_EXT(exponent) && __signbit##_EXT(exponent)) {                            \
            return INFINITY;                                                                   \
        }                                                                                      \
    }                                                                                          \
    if (__isinf##_EXT(exponent)) {                                                             \
        if (fabs##_EXT(base) < 1) {                                                            \
            if (__signbit##_EXT(exponent)) {                                                   \
                return INFINITY;                                                               \
            } else {                                                                           \
                return 0.0;                                                                    \
            }                                                                                  \
        } else {                                                                               \
            if (__signbit##_EXT(exponent)) {                                                   \
                return 0.0;                                                                    \
            } else {                                                                           \
                return INFINITY;                                                               \
            }                                                                                  \
        }                                                                                      \
    }                                                                                          \
    if (__isinf##_EXT(base)) {                                                                 \
        if (!__signbit##_EXT(base)) {                                                          \
            if (__signbit##_EXT(exponent)) return 0.0;                                         \
            else return INFINITY;                                                              \
        } else {                                                                               \
            if (__signbit##_EXT(exponent)) {                                                   \
                if (int_exponent == exponent && fmod##_EXT(exponent, 2) != 0.0)                \
                    return -0.0;                                                               \
                else return +0.0;                                                              \
            } else {                                                                           \
                if (int_exponent == exponent && fmod##_EXT(exponent, 2) != 0.0)                \
                    return -INFINITY;                                                          \
                else return INFINITY;                                                          \
            }                                                                                  \
        }                                                                                      \
    }                                                                                          \
    if (__isnan##_EXT(base) || __isnan##_EXT(exponent)) return NAN;                            \
    if (__signbit##_EXT(base))                                                                 \
        return __int_pow##_EXT(base, int_exponent);                                            \
    _FLT frac_y = exponent - int_exponent;                                                     \
    return __int_pow##_EXT(base, int_exponent) * __flt_exp##_EXT(log##_EXT(base) * frac_y);    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* sqrt */

#define func_ret(_FLT) _FLT
#define func_name sqrt
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                    \
    if (__isnan##_EXT(x))                          \
        return x;                                  \
    if (__signbit##_EXT(x) && x != -0.0) {         \
        feraiseexcept(FE_INVALID);                 \
        return NAN;                                \
    }                                              \
    if (__isinf##_EXT(x) || x == 0.0)              \
        return x;                                  \
    _FLT l = 0.0;                                  \
    _FLT r = fmax##_EXT(1.0, x);                   \
    int steps = 0;                                 \
    while (r - l > _EPS(_EXT) && steps < 1000) {   \
        _FLT m = (l + r) / 2;                      \
        if (m * m == x)                            \
            return m;                              \
        if (m * m < x)                             \
            l = m;                                 \
        else                                       \
            r = m;                                 \
        steps ++;                                  \
    }                                              \
    return l;                                      \
}

/* cbrt */

#define func_ret(_FLT) _FLT
#define func_name cbrt
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                    \
    if (__isnan##_EXT(x))                          \
        return x;                                  \
    if (__isinf##_EXT(x) || x == 0.0)              \
        return x;                                  \
    _FLT l = 0.0;                                  \
    _FLT r = fmax##_EXT(1.0, x);                   \
    int steps = 0;                                 \
    while (r - l > _EPS(_EXT) && steps < 1000) {   \
        _FLT m = (l + r) / 2;                      \
        if (m * m * m == x)                        \
            return m;                              \
        if (m * m * m < x)                         \
            l = m;                                 \
        else                                       \
            r = m;                                 \
        steps ++;                                  \
    }                                              \
    return l;                                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* hypot */

#define func_ret(_FLT) _FLT
#define func_name hypot
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) {                \
    if (__isinf##_EXT(x) || __isinf##_EXT(y))  \
        return INFINITY;                       \
    if (__isnan##_EXT(x) || __isnan##_EXT(y))  \
        return NAN;                            \
    return sqrt##_EXT(x * x + y * y);          \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* __sincos */

#define func_ret(_FLT) void
#define func_name __sincos
#define func_args(_FLT) (_FLT x, _FLT *s, _FLT *c)
#define func_code(_FLT, _EXT) {                                                                    \
    if (x == 0.0) {                                                                                \
        *s = x;                                                                                    \
        *c = 1.0;                                                                                  \
        return;                                                                                    \
    }                                                                                              \
    if (__isinf##_EXT(x)) {                                                                        \
        feraiseexcept(FE_DIVBYZERO);                                                               \
        *s = NAN;                                                                                  \
        *c = NAN;                                                                                  \
        return;                                                                                    \
    }                                                                                              \
    if (__isnan##_EXT(x)) {                                                                        \
        *s = NAN;                                                                                  \
        *c = NAN;                                                                                  \
        return;                                                                                    \
    }                                                                                              \
    if (x >= 2 * PI##_EXT) {                                                                      \
        __sincos##_EXT(fmod##_EXT(x, 2 * PI##_EXT), s, c);                                        \
        return;                                                                                    \
    }                                                                                              \
    if (__signbit##_EXT(x)) {                                                                      \
        __sincos##_EXT(-x, s, c);                                                                  \
        *s = -*s;                                                                                  \
        return;                                                                                    \
    }                                                                                              \
    const int resolution = sizeof##_EXT(approx_sines) / sizeof##_EXT(long _FLT) - 1;               \
    int n = 0;                                                                                     \
    while (x - n * PI##_EXT / resolution >= 0 && n < resolution)                                  \
        n++;                                                                                       \
    n--;                                                                                           \
    _FLT sin_npi = approx_sines[n];                                                                \
    _FLT cos_npi = approx_cosines[n];                                                              \
    _FLT r = x - n * (PI##_EXT / resolution);                                                     \
    if (r == 0.0) {                                                                                \
        *s = sin_npi;                                                                              \
        *c = cos_npi;                                                                              \
        return;                                                                                    \
    }                                                                                              \
    _FLT sin_r = r;                                                                                \
    _FLT cos_r = 1;                                                                                \
    _FLT r2 = r * r;                                                                               \
    _FLT sin_fac = 6;                                                                              \
    _FLT cos_fac = 2;                                                                              \
    _FLT px_sin = -r * r * r;                                                                      \
    _FLT px_cos = -r * r;                                                                          \
    int fac_no = 4;                                                                                \
    while (fabs##_EXT(px_sin / sin_fac) > _EPS(_EXT)&&fabs##_EXT(px_cos / cos_fac) > _EPS(_EXT)) { \
        sin_r += px_sin / sin_fac;                                                                 \
        cos_r += px_cos / cos_fac;                                                                 \
        sin_fac *= fac_no * (fac_no + 1);                                                          \
        cos_fac *= (fac_no - 1) * fac_no;                                                          \
        px_sin *= -r2;                                                                             \
        px_cos *= -r2;                                                                             \
        fac_no += 2;                                                                               \
    }                                                                                              \
    *s = sin_npi * cos_r + sin_r * cos_npi;                                                        \
    *c = cos_r * cos_npi - sin_npi * sin_r;                                                        \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* sin */

#define func_ret(_FLT) _FLT
#define func_name sin
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {    \
    if (x == 0.0)                  \
        return x;                  \
    if (__isinf##_EXT(x)) {        \
        feraiseexcept(FE_INVALID); \
        return NAN;                \
    }                              \
    if (__isnan##_EXT(x))          \
        return NAN;                \
    _FLT s, c;                     \
    __sincos##_EXT(x, &s, &c);     \
    return c;                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cos */

#define func_ret(_FLT) _FLT
#define func_name cos
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {    \
    if (x == 0.0)                  \
        return 1.0;                \
    if (__isinf##_EXT(x)) {        \
        feraiseexcept(FE_INVALID); \
        return NAN;                \
    }                              \
    if (__isnan##_EXT(x))          \
        return NAN;                \
    _FLT s, c;                     \
    __sincos##_EXT(x, &s, &c);     \
    return c;                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cos */

#define func_ret(_FLT) _FLT
#define func_name cos
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {    \
    if (x == 0.0)                  \
        return 1.0;                \
    if (__isinf##_EXT(x)) {        \
        feraiseexcept(FE_INVALID); \
        return NAN;                \
    }                              \
    if (__isnan##_EXT(x))          \
        return NAN;                \
    _FLT s, c;                     \
    __sincos##_EXT(x, &s, &c);     \
    return c;                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* tan */

#define func_ret(_FLT) _FLT
#define func_name tan
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {    \
    if (x == 0.0)                  \
        return x;                  \
    if (__isinf##_EXT(x)) {        \
        feraiseexcept(FE_INVALID); \
        return NAN;                \
    }                              \
    if (__isnan##_EXT(x))          \
        return NAN;                \
    _FLT s, c;                     \
    __sincos##_EXT(x, &s, &c);     \
    return s / c;                  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* asin */

#define func_ret(_FLT) _FLT
#define func_name asin
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {        \
    if (__isnan##_EXT(x)) return NAN;  \
    if (x == 0.0) return x;            \
    if (x > 1.0 || x < -1.0) {         \
        __raise_domain_err();          \
        return NAN;                    \
    }                                  \
    _FLT xim1, xi;                     \
    xim1 = 0.0;                        \
    for (int i = 0; i < 100; i++) {    \
        _FLT s, c;                     \
        __sincos##_EXT(xim1, &s, &c);  \
        xi = xim1 - (s - x) / c;       \
        if (xi == xim1) break;         \
        xim1 = xi;                     \
    }                                  \
    return xi;                         \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* acos */

#define func_ret(_FLT) _FLT
#define func_name acos
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {        \
    if (__isnan##_EXT(x)) return NAN;  \
    if (x == 1.0) return +0.0;         \
    if (x > 1.0 || x < -1.0) {         \
        __raise_domain_err();          \
        return NAN;                    \
    }                                  \
    if (x == -1.0) return PI##_EXT;    \
    _FLT xim1, xi;                     \
    xim1 = PI##_EXT / 2;               \
    for (int i = 0; i < 100; i++) {    \
        _FLT s, c;                     \
        __sincos##_EXT(xim1, &s, &c);  \
        xi = xim1 + (c - x) / s;       \
        if (xi == xim1) break;         \
        xim1 = xi;                     \
    }                                  \
    return xi;                         \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* atan */

#define func_ret(_FLT) _FLT
#define func_name atan
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                    \
    if (__isnan##_EXT(x)) return NAN;              \
    if (x == 0.0) return x;                        \
    if (__isinf##_EXT(x))                          \
        return copysign##_EXT(PI##_EXT / 2, x);    \
    _FLT xim1, xi;                                 \
    xim1 = 0;                                      \
    for (int i = 0; i < 100; i++) {                \
        _FLT s, c;                                 \
        __sincos##_EXT(xim1, &s, &c);              \
        xi = xim1 - ((s / c) - x) * c * c;         \
        if (xi == xim1) break;                     \
        xim1 = xi;                                 \
    }                                              \
    return xi;                                     \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* atan2 */

#define func_ret(_FLT) _FLT
#define func_name atan2
#define func_args(_FLT) (_FLT y, _FLT x)
#define func_code(_FLT, _EXT) {                                    \
    if (__isnan##_EXT(x) || __isnan##_EXT(y))                      \
        return NAN;                                                \
    if (y == 0.0) {                                                \
        if (__signbit##_EXT(x))                                    \
            return copysign##_EXT(PI##_EXT, y);                    \
        else                                                       \
            return copysign##_EXT(0.0, y);                         \
    }                                                              \
    if (__isinf##_EXT(y)) {                                        \
        if (__isinf##_EXT(x)) {                                    \
            if (__signbit##_EXT(x))                                \
                return copysign##_EXT(3 * PI##_EXT / 4, y);        \
            else                                                   \
                return copysign##_EXT(PI##_EXT / 4, y);            \
        } else                                                     \
            return copysign##_EXT(PI##_EXT / 2, y);                \
    }                                                              \
    if (x == 0.0) {                                                \
        return copysign##_EXT(PI##_EXT / 2, y);                    \
    }                                                              \
    if (__isinf##_EXT(x)) {                                        \
        if (__signbit##_EXT(x))                                    \
            return copysign##_EXT(PI##_EXT, y);                    \
        else                                                       \
            return copysign##_EXT(0.0, y);                         \
    }                                                              \
    _FLT base_atan = atan##_EXT(fabs##_EXT(y / x));                \
    int quadrant;                                                  \
    if (!__signbit##_EXT(x) && !__signbit##_EXT(y))                \
        quadrant = 1;                                              \
    else if (__signbit##_EXT(x) && !__signbit##_EXT(y))            \
        quadrant = 2;                                              \
    else if (__signbit##_EXT(x) && __signbit##_EXT(y))             \
        quadrant = 3;                                              \
    else                                                           \
        quadrant = 4;                                              \
    _FLT result = base_atan + (PI##_EXT / 2) * (quadrant - 1);     \
    if (result > PI##_EXT)                                         \
        result -= 2 * PI##_EXT;                                    \
    return result;                                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* sinh */

#define func_ret(_FLT) _FLT
#define func_name sinh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                \
    if (x == 0.0 || __isinf##_EXT(x))          \
        return x;                              \
    if (__isnan##_EXT(x))                      \
        return x;                              \
    return (exp##_EXT(x) - exp##_EXT(-x)) / 2; \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cosh */

#define func_ret(_FLT) _FLT
#define func_name cosh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                \
    if (x == 0.0 || __isinf##_EXT(x))          \
        return x;                              \
    if (__isnan##_EXT(x))                      \
        return x;                              \
    return (exp##_EXT(x) + exp##_EXT(-x)) / 2; \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* tanh */

#define func_ret(_FLT) _FLT
#define func_name tanh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {        \
    if (x == 0.0 || __isinf##_EXT(x))  \
        return x;                      \
    if (__isnan##_EXT(x))              \
        return x;                      \
    _FLT epx = exp##_EXT(x);           \
    _FLT emx = exp##_EXT(-x);          \
    return (epx - emx) / (epx + emx);  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* asinh */

#define func_ret(_FLT) _FLT
#define func_name asinh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                \
    if (x == 0.0 || __isinf##_EXT(x) || __isnan##_EXT(x))      \
        return x;                                              \
    _FLT xim1, xi;                                             \
    xim1 = 0.0;                                                \
    for (int i = 0; i < 100; i++) {                            \
        xi = xim1 - (sinh##_EXT(xim1) - x) / cosh##_EXT(xim1); \
        if (xi == xim1) break;                                 \
        xim1 = xi;                                             \
    }                                                          \
    return xi;                                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* acosh */

#define func_ret(_FLT) _FLT
#define func_name acosh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                \
    if (x == 0.0 || __isinf##_EXT(x) || __isnan##_EXT(x))      \
        return x;                                              \
    _FLT xim1, xi;                                             \
    xim1 = log##_EXT(x);                                       \
    for (int i = 0; i < 200; i++) {                            \
        xi = xim1 - (cosh##_EXT(xim1) - x) / sinh##_EXT(xim1); \
        if (xi == xim1) break;                                 \
        xim1 = xi;                                             \
    }                                                          \
    return xi;                                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* atanh */

#define func_ret(_FLT) _FLT
#define func_name atanh
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                            \
    if (x == 0.0 || __isnan##_EXT(x))                      \
        return x;                                          \
    if (x == 1.0 || x == -1.0) {                           \
        feraiseexcept(FE_DIVBYZERO);                       \
        return copysign##_EXT(INFINITY, x);                \
    }                                                      \
    if (x > 1.0 || x < -1.0) {                             \
        feraiseexcept(FE_INVALID);                         \
        return NAN;                                        \
    }                                                      \
    _FLT xim1, xi;                                         \
    xim1 = 0;                                              \
    for (int i = 0; i < 200; i++) {                        \
        _FLT chx = cosh##_EXT(xim1);                       \
        xi = xim1 - (tanh##_EXT(xim1) - x) * chx * chx;    \
        if (xi == xim1) break;                             \
        xim1 = xi;                                         \
    }                                                      \
    return xi;                                             \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* erf */

#define func_ret(_FLT) _FLT
#define func_name erf
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                                                                        \
    if (x == 0.0 || __isnan##_EXT(x))                                                                                  \
        return x;                                                                                                      \
    if (__isinf##_EXT(x))                                                                                              \
        return copysign##_EXT(1.0, x);                                                                                 \
    if (__signbit##_EXT(x))                                                                                            \
        return -erf##_EXT(-x);                                                                                         \
    const _FLT coeffs[] = {3.0, 10.0, 42.0, 216.0, 1320.0, 9360.0, 75600.0, 685440.0, 6894720.0,                       \
        76204800.0, 918086400.0, 11975040000.0, 168129561600.0, 2528170444800.0, 40537905408000.0, 690452066304000.0,  \
        12449059983360000.0, 236887827111936000.0, 4744158915944448000.0, 99748982335242240000.0};                     \
    int len = 20;                                                                                                      \
    _FLT sum = x;                                                                                                      \
    _FLT px = -x * x * x;                                                                                              \
    _FLT x2 = -x * x;                                                                                                  \
    for (int i = 0; i < 20; i++) {                                                                                     \
        sum += px / coeffs[i];                                                                                         \
        if (px / coeffs[i] == 0.0) break;                                                                              \
        px *= x2;                                                                                                      \
    }                                                                                                                  \
    return sum * 2 / sqrt##_EXT(PI##_EXT);                                                                             \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* erfc */

#define func_ret(_FLT) _FLT
#define func_name erfc
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {    \
    if (__isnan##_EXT(x))          \
        return x;                  \
    if (__isinf##_EXT(x)) {        \
        if (__signbit##_EXT(x))    \
            return 2.0;            \
        else                       \
            return 0.0;            \
    }                              \
    return 1 - erf##_EXT(x);       \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* tgamma */

#define func_ret(_FLT) _FLT
#define func_name tgamma
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                            \
    if (x == 0.0) {                                                        \
        feraiseexcept(FE_DIVBYZERO);                                       \
        return NAN;                                                        \
    }                                                                      \
    if (__signbit##_EXT(x) && (trunc##_EXT(x) == x || __isinf##_EXT(x))) { \
        feraiseexcept(FE_INVALID);                                         \
        return NAN;                                                        \
    }                                                                      \
    if (__isinf##_EXT(x))                                                  \
        return INFINITY;                                                   \
    if (__isnan##_EXT(x))                                                  \
        return NAN;                                                        \
    if (x > 1.0) {                                                         \
        return (x - 1) * tgamma##_EXT(x - 1);                              \
    }                                                                      \
    if (__signbit##_EXT(x)) {                                              \
        return PI##_EXT / (sin##_EXT(PI##_EXT * x) * tgamma##_EXT(1 - x)); \
    }                                                                      \
    _FLT sum = 0.0;                                                        \
    const int len = 1000;                                                  \
    for (int i = 1; i < len; i++) {                                        \
        _FLT y = (_FLT)i / len;                                            \
        sum += pow##_EXT(-log##_EXT(y), x - 1);                            \
    }                                                                      \
    return sum / len;                                                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* lgamma */

#define func_ret(_FLT) _FLT
#define func_name lgamma
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                    \
    if (__isnan##_EXT(x))                                          \
        return x;                                                  \
    if (x == 1.0 || x == 2.0)                                      \
        return 0.0;                                                \
    if (x == 0.0 || (__signbit##_EXT(x) && trunc##_EXT(x) == x)) { \
        feraiseexcept(FE_DIVBYZERO);                               \
        return INFINITY;                                           \
    }                                                              \
    if (__isinf##_EXT(x))                                          \
        return INFINITY;                                           \
    return log##_EXT(fabs##_EXT(tgamma##_EXT(x)));                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code


/* trunc */

#define func_ret(_FLT) _FLT
#define func_name trunc
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                                         \
    _UNI(_EXT) *uni = (_UNI(_EXT)*)&x;                                  \
    if (isnan(x))                                                       \
        return x;                                                       \
    if (isinf(x))                                                       \
        return uni->bits.sign ? -INFINITY : INFINITY;                   \
    long point = _L(_EXT) - (uni->bits.exponent - _B(_EXT));            \
    if (point >= _L(_EXT))                                              \
        return x;                                                       \
    if (point <= 0)                                                     \
        return 0.0##_EXT;                                               \
    unsigned long long mask = ((unsigned long long)1 << point) - 1;     \
    _UNI(_EXT) result = *uni;                                           \
    result.bits.mantissa &= ~mask;                                      \
    return result.number;                                               \
}

decl_math_funcf(func_ret, trunc, func_args, func_code)
decl_math_funcd(func_ret, trunc, func_args, func_code)

#undef func_code
#define func_code(_FLT, _EXT) {                                         \
    _UNI(_EXT) *uni = (_UNI(_EXT)*)&x;                                  \
    if (isnan(x))                                                       \
        return NAN;                                                     \
    if (isinf(x))                                                       \
        return uni->bits.sign ? -INFINITY : INFINITY;                   \
    int point = _L(_EXT) - (uni->bits.exponent - _B(_EXT));             \
    if (point >= _L(_EXT))                                              \
        return x;                                                       \
    if (point <= 0)                                                     \
        return 0.0##_EXT;                                               \
    unsigned long long mask = ((unsigned long long)1 << point) - 1;     \
    _UNI(_EXT) result = *uni;                                           \
    result.bits.mantissa2 &= ~mask;                                     \
    if (mask >= ((unsigned long long)1 << 48))                          \
        result.bits.mantissa1 &= ~(mask >> 48);                         \
    return result.number;                                               \
}

decl_math_funcl(func_ret, trunc, func_args, func_code)

#undef func_code
#undef func_ret
#undef func_name
#undef func_args

/* floor */

#define func_ret(_FLT) _FLT
#define func_name floor
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    _FLT tx = trunc(x);         \
    if (x == tx)                \
        return x;               \
    if (x < 0)                  \
        return tx - 1;          \
    return tx;                  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* ceil */

#define func_ret(_FLT) _FLT
#define func_name ceil
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    _FLT tx = trunc(x);         \
    if (x == tx)                \
        return tx;              \
    if (x > 0)                  \
        return tx + 1;          \
    return tx;                  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* round, lround, llround */

#define func_ret(_FLT) _FLT
#define func_name round
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                 \
    if (__signbit##_EXT(x))                     \
        return ceil(x + 0.5);                   \
    else                                        \
        return floor(x - 0.5);                  \
}

decl

#undef func_ret
#undef func_name
#define func_ret(_FLT) long
#define func_name lround

#undef func_code
#define func_code(_FLT, _EXT) {                 \
    if (x > (_FLT)LONG_MAX)                     \
        return LONG_MAX;                        \
    if (x < (_FLT)LONG_MIN)                     \
        return LONG_MIN;                        \
    if (__signbit##_EXT(x))                     \
        return (long)ceil(x + 0.5);             \
    else                                        \
        return (long)floor(x - 0.5);            \
}

decl

#undef func_ret
#undef func_name
#define func_ret(_FLT) long long
#define func_name llround

#undef func_code
#define func_code(_FLT, _EXT) {                 \
    if (x > (_FLT)LLONG_MAX)                    \
        return LLONG_MAX;                       \
    if (x < (_FLT)LLONG_MIN)                    \
        return LLONG_MIN;                       \
    if (__signbit##_EXT(x))                     \
        return (long long)ceil(x + 0.5);        \
    else                                        \
        return (long long)floor(x - 0.5);       \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* nearbyint */

#define func_ret(_FLT) _FLT
#define func_name nearbyint
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) { \
    int mode = fegetround();    \
    if (mode == FE_DOWNWARD)    \
        return floor##_EXT(x);  \
    if (mode == FE_TONEAREST)   \
        return round##_EXT(x);  \
    if (mode == FE_UPWARD)      \
        return ceil##_EXT(x);   \
    if (mode == FE_TOWARDZERO)  \
        return trunc##_EXT(x);  \
    return NAN;                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* rint, lrint, llrint */

#define func_ret(_FLT) _FLT
#define func_name  rint
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {                         \
    if (__isinf##_EXT(x) || __isnan##_EXT(x) || x == 0) \
        return x;                                       \
    int mode = fegetround();                            \
    if (mode == FE_DOWNWARD)                            \
        return floor##_EXT(x);                          \
    if (mode == FE_TONEAREST)                           \
        return round##_EXT(x);                          \
    if (mode == FE_UPWARD)                              \
        return ceil##_EXT(x);                           \
    if (mode == FE_TOWARDZERO)                          \
        return trunc##_EXT(x);                          \
    return NAN;                                         \
}

decl

#undef func_ret
#undef func_name
#undef func_code

#define func_ret(_FLT) long
#define func_name lrint
#define func_code(_FLT, _EXT) {                 \
    if (x > (_FLT)LONG_MAX)                     \
        return LONG_MAX;                        \
    if (x < (_FLT)LONG_MIN)                     \
        return LONG_MIN;                        \
    int mode = fegetround();                    \
    if (mode == FE_DOWNWARD)                    \
        return (long)floor##_EXT(x);            \
    if (mode == FE_TONEAREST)                   \
        return (long)round##_EXT(x);            \
    if (mode == FE_UPWARD)                      \
        return (long)ceil##_EXT(x);             \
    if (mode == FE_TOWARDZERO)                  \
        return (long)trunc##_EXT(x);            \
    return NAN;                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_code

#define func_ret(_FLT) long long
#define func_name llrint
#define func_code(_FLT, _EXT) {                 \
    if (x > (_FLT)LLONG_MAX)                    \
        return LLONG_MAX;                       \
    if (x < (_FLT)LLONG_MIN)                    \
        return LLONG_MIN;                       \
    int mode = fegetround();                    \
    if (mode == FE_DOWNWARD)                    \
        return (long long)floor##_EXT(x);       \
    if (mode == FE_TONEAREST)                   \
        return (long long)round##_EXT(x);       \
    if (mode == FE_UPWARD)                      \
        return (long long)ceil##_EXT(x);        \
    if (mode == FE_TOWARDZERO)                  \
        return (long long)trunc##_EXT(x);       \
    return NAN;                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* frexp */

#define func_ret(_FLT) _FLT
#define func_name frexp
#define func_args(_FLT) (_FLT x, int *exp)
#define func_code(_FLT, _EXT) {                             \
    if (x == 0 || __isnan##_EXT(x) || __isinf##_EXT(x)) {   \
        *exp = 0;                                           \
        return x;                                           \
    }                                                       \
    if (x < 0)                                              \
        return -frexp(-x, exp);                             \
    int exponent = 0;                                       \
    while (x > 1) {                                         \
        x /= 2;                                             \
        exponent++;                                         \
    }                                                       \
    while (x < 0.5) {                                       \
        x *= 2;                                             \
        exponent --;                                        \
    }                                                       \
    *exp = exponent;                                        \
    return x;                                               \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* ldexp */

#define func_ret(_FLT) _FLT
#define func_name ldexp
#define func_args(_FLT) (_FLT x, int exp)
#define func_code(_FLT, _EXT) { \
    if (__isnan##_EXT(x))       \
        return x;               \
    if (__isinf##_EXT(x))       \
        return x;               \
    if (exp == 0)               \
        return x;               \
    if (x == 0.0)               \
        return x;               \
    _UNI(_EXT) mult;            \
    mult.bits.exponent = exp;   \
    return x * mult.number;     \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* modf */

#define func_ret(_FLT) _FLT
#define func_name modf
#define func_args(_FLT) (_FLT x, _FLT *ix)
#define func_code(_FLT, _EXT) { \
    if (x == 0) {               \
        *ix = 0;                \
        return x;               \
    }                           \
    if (__isinf(x)) {           \
        *ix = x;                \
        return x;               \
    }                           \
    if (__isnan(x)) {           \
        *ix = NAN;              \
        return NAN;             \
    }                           \
    _FLT tx = trunc(x);         \
    *ix = tx;                   \
    return x - tx;              \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* scalbn, scalbln */

#define func_ret(_FLT) _FLT
#define func_name scalbn
#define func_args(_FLT) (_FLT x, int exp)
#define func_code(_FLT, _EXT) { \
    return ldexp##_EXT(x, exp); \
}

decl

#undef func_name
#undef func_args
#undef func_code

#define func_name scalbln
#define func_args(_FLT) (_FLT x, long exp)
#define func_code(_FLT, _EXT) {         \
    return ldexp##_EXT(x, (int)exp));   \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* ilogb */

#define func_ret(_FLT) int
#define func_name ilogb
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    if (x == 0.0)                       \
        return FP_ILOGB0;               \
    if (__isinf##_EXT(x))               \
        return INT_MAX;                 \
    if (__isnan##_EXT(x))               \
        return FP_ILOGBNAN;             \
    _UNI(_EXT) *ptr = (_UNI(_EXT)*)&x;  \
    return ptr->bits.exponent;          \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* logb */

#define func_ret(_FLT) _FLT
#define func_name logb
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    if (x == 0.0) {                     \
        feraiseexcept(FE_DIVBYZERO);    \
        return -INFINITY;               \
    }                                   \
    if (__isinf##_EXT(x))               \
        return x;                       \
    if (__isnan##_EXT(x))               \
        return NAN;                     \
    _UNI(_EXT) *ptr = (_UNI(_EXT)*)&x;  \
    return ptr->bits.exponent;          \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* nextafter */

#define func_ret(_FLT) _FLT
#define func_name nextafter
#define func_args(_FLT) (_FLT from, _FLT to)
#define func_code(_FLT, _EXT) {                     \
    if (from == to)                                 \
        return to;                                  \
    if (__isnan##_EXT(from) || __isnan##_EXT(to))   \
        return NAN;                                 \
    _FLT eps = _Generic((to),                       \
        float: FLT_EPSILON,                         \
        double: DBL_EPSILON,                        \
        long double: LDBL_EPSILON                   \
    );                                              \
    _FLT nxt;                                       \
    if (from > to)                                  \
        nxt = from - eps;                           \
    else                                            \
        nxt = from + eps;                           \
    if (__isinf##_EXT(nxt))                         \
        feraiseexcept(FE_INEXACT | FE_OVERFLOW);    \
    if (__issubnormal##_EXT(nxt)) || nxt == 0.0)    \
        feraiseexcept(FE_INEXACT | FE_UNDERFLOW);   \
    return nxt;                                     \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* nexttoward */

#define func_ret(_FLT) _FLT
#define func_name nexttoward
#define func_args(_FLT) (_FLT from, long double to)
#define func_code(_FLT, _EXT) {                     \
    if (from == to)                                 \
        return (_FLT)to;                            \
    if (__isnan##_EXT(from) || __isnan##_EXT(to))   \
        return NAN;                                 \
    _FLT eps = _Generic((to),                       \
        float: FLT_EPSILON,                         \
        double: DBL_EPSILON,                        \
        long double: LDBL_EPSILON                   \
    );                                              \
    _FLT nxt;                                       \
    if (from > to)                                  \
        nxt = from - eps;                           \
    else                                            \
        nxt = from + eps;                           \
    if (__isinf##_EXT(nxt))                         \
        feraiseexcept(FE_INEXACT | FE_OVERFLOW);    \
    if (__issubnormal##_EXT(nxt)) || nxt == 0.0)    \
        feraiseexcept(FE_INEXACT | FE_UNDERFLOW);   \
    return nxt;                                     \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* copysign */

#define func_ret(_FLT) _FLT
#define func_name copysign
#define func_args(_FLT) (_FLT x, _FLT y)
#define func_code(_FLT, _EXT) { \
    _FLT ret = fabs##_EXT(x);   \
    if (__signbit##_EXT(y))     \
        return -ret;            \
    else                        \
        return ret;             \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* __isinf */

#define func_ret(_FLT) int
#define func_name __isinf
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) uni;                     \
    uni.number = x;                     \
    if (uni.exponent == _MAX_EXP(_EXT) && uni.mantissa == 0) \
        return 1;                       \
    else                                \
        return 0;                       \
}

decl_math_funcf(func_ret, __isinf, func_args, func_code)
decl_math_funcd(func_ret, __isinf, func_args, func_code)

#undef func_code
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) uni;                     \
    uni.number = x;                     \
    if (uni.exponent == _MAX_EXP(_EXT) && uni.mantissa1 == 0 && uni.mantissa2 == 0) \
        return 1;                       \
    else                                \
        return 0;                       \
}

decl_math_funcl(func_ret, __isinf, func_args, func_code)

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* __isnan */

#define func_ret(_FLT) int
#define func_name __isnan
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) uni;                     \
    uni.number = x;                     \
    if (uni.exponent == _MAX_EXP(_EXT) && uni.mantissa >= (unsigned long long)1 << (_L(_EXT) - 1)) \
        return 1;                       \
    else                                \
        return 0;                       \
}

decl_math_funcf(func_ret, __isnan, func_args, func_code)
decl_math_funcd(func_ret, __isnan, func_args, func_code)

#undef func_code
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) uni;                     \
    uni.number = x;                     \
    if (uni.exponent == _MAX_EXP(_EXT) && uni.mantissa2 >= (unsigned long long)1 << (_L(_EXT) - 65)) \
        return 1;                       \
    else                                \
        return 0;                       \
}

decl_math_funcl(func_ret, __isinf, func_args, func_code)

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* __signbit */

#define func_ret(_FLT) int
#define func_name __signbit
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) *ptr = (_UNI(_EXT)*)&x;  \
    return ptr->bits.sign;              \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* __issubnormal */

#define func_ret(_FLT) int
#define func_name _issubnormal
#define func_args(_FLT) (_FLT x)
#define func_code(_FLT, _EXT) {         \
    _UNI(_EXT) *ptr = (_UNI(_EXT)*)&x;  \
    return ptr->bits.exponent == 0;     \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code