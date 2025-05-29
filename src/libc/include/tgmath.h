//
//  tgmath.h - Математика без типов
//
//  Определяет независимые от типов макросы, которые вызывают математические функции.
//

#if !defined _TGMATH_INCL && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199900L
#define _TGMATH_INCL 1

#include <math.h>
#include <etc/mathfunc.h>

// В C++ всё, что тут делается, достигается перегружением функций, 
// поэтому нет смысла объявлять все эти макросы.
#if !defined(__cplusplus)

#ifndef __STDC_NO_COMPLEX__
#include <complex.h>
#endif

// Описание у каждой функции должно быть в заголовке math.h.
// Здесь я их приводить не буду.

#define sin(x)          __MATH_FUNC1C(sin, x)
#define cos(x)          __MATH_FUNC1C(cos, x)
#define tan(x)          __MATH_FUNC1C(tan, x)
#define sinh(x)         __MATH_FUNC1C(sinh, x)
#define cosh(x)         __MATH_FUNC1C(cosh, x)
#define tanh(x)         __MATH_FUNC1C(tanh, x)

#define asin(x)         __MATH_FUNC1C(asin, x)
#define asinh(x)        __MATH_FUNC1C(asinh, x)
#define acos(x)         __MATH_FUNC1C(acos, x)
#define acosh(x)        __MATH_FUNC1C(acosh, x)
#define atan(x)         __MATH_FUNC1C(atan, x)
#define atanh(x)        __MATH_FUNC1C(atanh, x)
#define asinpi(x)       __MATH_FUNC1C(asinpi, x)
#define acospi(x)       __MATH_FUNC1C(acospi, x)
#define atanpi(x)       __MATH_FUNC1C(atanpi, x)
#define atan2(x)        __MATH_FUNC2(atan2, x, y)
#define atan2pi(x)      __MATH_FUNC2(atan2pi, x, y)

#define sqrt(x)         __MATH_FUNC1C(sqrt, x)
#define pow(x, y)       __MATH_FUNC2C(pow, x, y)
#define exp(x)          __MATH_FUNC1C(exp, x)
#define cbrt(x)         __MATH_FUNC1(cbrt, x)
#define exp2(x)         __MATH_FUNC1(exp2, x)

#define fabs(x)         __MATH_FUNC1C_CFUNC(fabs, cabs, x)
#define ceil(x)         __MATH_FUNC1(ceil, x)
#define floor(x)        __MATH_FUNC1(floor, x)
#define copysign(x, y)  __MATH_FUNC2(copysign, x, y)
#define erf(x)          __MATH_FUNC1(erf, x)
#define erfc(x)         __MATH_FUNC1(erfc, x)
#define expm1(x)        __MATH_FUNC1(expm1, x)
#define fdim(x)         __MATH_FUNC1(fdim, x)
#define fma(x, y, z)    __MATH_FUNC3(fma, x, y, z)
#define fmax(x, y)      __MATH_FUNC2(fmax, x, y)
#define fmin(x, y)      __MATH_FUNC2(fmin, x, y)
#define fmod(x, y)      __MATH_FUNC2(fmod, x, y)
#define frexp(x, xp)    __MATH_FUNC2_PRESERVE(frexp, x, xp)
#define hypot(x, y)     __MATH_FUNC2(hypot, x, y)
#define ilogb(x)        __MATH_FUNC1(ilogb, x, y)
#define ldexp(x, xp)    __MATH_FUNC2(ldexp, x, xp)
#define lgamma(x)       __MATH_FUNC1(lgamma, x)
#define rint(x)         __MATH_FUNC1(rint, x)
#define lrint(x)        __MATH_FUNC1(lrint, x)
#define llrint(x)       __MATH_FUNC1(llrint, x)
#define round(x)        __MATH_FUNC1(round, x)
#define lround(x)       __MATH_FUNC1(lround, x)
#define llround(x)      __MATH_FUNC1(llround, x)
#define log10(x)        __MATH_FUNC1(log10, x)
#define log1p(x)        __MATH_FUNC1(log1p, x)
#define log2(x)         __MATH_FUNC1(log2, x)
#define logb(x)         __MATH_FUNC1(logb, x)
#define nearbyint(x)    __MATH_FUNC1(nearbyint, x)
#define nextafter(x, y) __MATH_FUNC2(nextafter, x, y)
#define nexttoward(x, y) __MATH_FUNC2(nexttoward, x, y)
#define remainder(x, y) __MATH_FUNC2(remainder, x, y)
#define remquo(x, y, q) __MATH_FUNC3_PRESERVE(remquo, x, y, q)
#define scalbn(x, xp)   __MATH_FUNC2_PRESERVE(scalbn, x, xp)
#define scalbln(x, xp)  __MATH_FUNC2_PRESERVE(scalbln, x, xp)
#define tgamma(x)       __MATH_FUNC1(tgamma, x)
#define trunc(x)        __MATH_FUNC1(trunc, x)

#ifndef __STDC_NO_COMPLEX__

#define carg(x)         __MATH_FUNC1_COMPLEX(carg, x)
#define conj(x)         __MATH_FUNC1_COMPLEX(conj, x)
#define creal(x)        __MATH_FUNC1_COMPLEX(creal, x)
#define cimag(x)        __MATH_FUNC1_COMPLEX(cimag, x)
#define cproj(x)        __MATH_FUNC1_COMPLEX(cproj, x)

#endif

#endif

#endif