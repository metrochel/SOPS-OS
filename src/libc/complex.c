/*
 * complex.c
 *
 *
 */

#include "include/complex.h"
#include "include/math.h"
#include "include/fenv.h"

// =============================================== Макросы ==================================================

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

// ============================================ Постоянные ==================================================

extern const float PIf;
extern const double PI;
extern const long double PIl;

// =============================================== Функции ==================================================

/* creal */

#define func_ret(_FLT) _FLT
#define func_name creal
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {\
    return (_FLT)z;    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cimag */

#define func_ret(_FLT) _FLT
#define func_name cimag
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {        \
    _FLT _Complex iz = z - (_FLT)z;    \
    _FLT *ptr = (_FLT*)&iz;            \
    _FLT _;                            \
    _ = ptr[1];                        \
    ptr[1] = ptr[0];                   \
    ptr[0] = _;                        \
    return iz;                         \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cabs */

#define func_ret(_FLT) _FLT
#define func_name cabs
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                    \
    _FLT real = creal##_EXT(z);                    \
    _FLT imag = cimag##_EXT(z);                    \
    return sqrt##_EXT(real * real + imag * imag);  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* carg */

#define func_ret(_FLT) _FLT
#define func_name carg
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                            \
    return atan2##_EXT(cimag##_EXT(z), creal##_EXT(z));    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* conj */

#define func_ret(_FLT) _FLT _Complex
#define func_name conj
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {\
    _FLT *ptr = (_FLT*)&z;     \
    ptr[1] = -ptr[1];          \
    return z;                  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cproj */

#define func_ret(_FLT) _FLT _Complex
#define func_name cproj
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                            \
    _FLT real = creal##_EXT(z);                            \
    _FLT imag = cimag##_EXT(z);                            \
    if (__isinf##_EXT(real) || __isinf##_EXT(imag)) {      \
        return INFINITY + copysign##_EXT(0.0, imag) * I;   \
    }                                                      \
    return z;                                              \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cexp */

#define func_ret(_FLT) _FLT _Complex
#define func_name cexp
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                            \
    _FLT real = creal##_EXT(z);                                            \
    _FLT imag = cimag##_EXT(z);                                            \
    if (real == 0.0 && imag == 0.0)                                        \
        return 1.0;                                                        \
    if ((__isinf##_EXT(imag) || __isnan##_EXT(imag)) && isfinite(real)) {  \
        feraiseexcept(FE_INVALID);                                         \
        return NAN + NAN * I;                                              \
    }                                                                      \
    if (__isinf##_EXT(real)) {                                             \
        if (__isnan##_EXT(imag)) {                                         \
            if (__signbit##_EXT(real))                                     \
                return 0;                                                  \
            else                                                           \
                return INFINITY + NAN * I;                                 \
        }                                                                  \
        if (__isinf##_EXT(imag)) {                                         \
            if (__signbit##_EXT(real))                                     \
                return 0;                                                  \
            else {                                                         \
                feraiseexcept(FE_INVALID);                                 \
                return INFINITY + NAN * I;                                 \
            }                                                              \
        }                                                                  \
        if (!__signbit##_EXT(real)) {                                      \
            if (imag == 0.0)                                               \
                return INFINITY;                                           \
            else                                                           \
                return INFINITY * (cos##_EXT(imag) + I * sin##_EXT(imag)); \
        } else {                                                           \
            return +0 * (cos##_EXT(imag) + I * sin##_EXT(imag));           \
        }                                                                  \
    }                                                                      \
    if (__isnan##_EXT(real)) {                                             \
        if (__isnan##_EXT(imag)) {                                         \
            return NAN + NAN * I;                                          \
        }                                                                  \
        if (imag == 0.0) {                                                 \
            return NAN;                                                    \
        }                                                                  \
        feraiseexcept(FE_INVALID);                                         \
        return NAN + NAN * I;                                              \
    }                                                                      \
    return exp##_EXT(real) * (cos##_EXT(imag) + I * sin##_EXT(imag));      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* clog */

#define func_ret(_FLT) _FLT _Complex
#define func_name clog
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                            \
    _FLT real = creal##_EXT(z);                            \
    _FLT imag = cimag##_EXT(z);                            \
    if (real == 0.0 || imag == 0.0) {                      \
        feraiseexcept(FE_DIVBYZERO);                       \
        if (__signbit##_EXT(real))                         \
            return -INFINITY + PI##_EXT * I;               \
        else                                               \
            return -INFINITY;                              \
    }                                                      \
    if (__isinf##_EXT(real)) {                             \
        if (__isnan##_EXT(imag)) {                         \
            return INFINITY + NAN * I;                     \
        }                                                  \
        if (__signbit##_EXT(real)) {                       \
            if (isfinite(imag) && imag > 0)                \
                return INFINITY + PI##_EXT * I;            \
            else if (__isinf##_EXT(imag))                  \
                return INFINITY + (3 * PI##_EXT / 4) * I;  \
        } else {                                           \
            if (isfinite(imag) && imag > 0)                \
                return INFINITY;                           \
            else                                           \
                return INFINITY + (PI##_EXT / 4) * I;      \
        }                                                  \
    }                                                      \
    if (__isnan##_EXT(real)) {                             \
        if (__isnan##_EXT(imag))                           \
            return NAN + NAN * I;                          \
        if (__isinf##_EXT(imag))                           \
            return INFINITY + NAN * I;                     \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    if (__isnan##_EXT(imag)) {                             \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    if (__isinf##_EXT(imag)) {                             \
        return INFINITY + (PI##_EXT / 2) * I;              \
    }                                                      \
    _FLT abs = cabs##_EXT(z);                              \
    _FLT arg = carg##_EXT(z);                              \
    return clog##_EXT(abs) + I * arg;                      \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cpow */

#define func_ret(_FLT) _FLT _Complex
#define func_name cpow
#define func_args(_FLT) (_FLT _Complex x, _FLT _Complex y)
#define func_code(_FLT, _EXT) {            \
    return cexp##_EXT(x * clog##_EXT(y));  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* csqrt */

#define func_ret(_FLT) _FLT _Complex
#define func_name csqrt
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                        \
    _FLT real = creal##_EXT(z);                                        \
    _FLT imag = cimag##_EXT(z);                                        \
    if (real == 0.0 && imag == 0.0)                                    \
        return 0.0;                                                    \
    if (__isinf##_EXT(imag))                                           \
        return INFINITY + INFINITY * I;                                \
    if (__isnan##_EXT(imag)) {                                         \
        if (__isnan##_EXT(real))                                       \
            return NAN + NAN * I;                                      \
        if (!__isinf##_EXT(real)) {                                    \
            feraiseexcept(FE_INVALID);                                 \
            return NAN + NAN * I;                                      \
        } else {                                                       \
            if (__signbit##_EXT(real)) {                               \
                return NAN + INFINITY * I;                             \
            } else {                                                   \
                return INFINITY + NAN * I;                             \
            }                                                          \
        }                                                              \
    }                                                                  \
    if (__isnan##_EXT(real)) {                                         \
        feraiseexcept(FE_INVALID);                                     \
        return NAN + NAN * I;                                          \
    }                                                                  \
    if (__isinf##_EXT(real) && imag > 0) {                             \
        if (__signbit##_EXT(real)) {                                   \
            return 0 + INFINITY * I;                                   \
        } else {                                                       \
            return INFINITY + 0 * I;                                   \
        }                                                              \
    }                                                                  \
    _FLT sqrt_real, sqrt_imag;                                         \
    _FLT mag = cabs##_EXT(z);                                          \
    sqrt_real = sqrt##_EXT((mag + real) / 2);                          \
    sqrt_imag = copysign##_EXT(sqrt##_EXT((mag - real) / 2), imag);    \
    return sqrt_real + I * sqrt_imag;                                  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* csin */

#define func_ret(_FLT) _FLT _Complex
#define func_name csin
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {        \
    return -I * csinh##_EXT(I * z);    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* ccos */

#define func_ret(_FLT) _FLT _Complex
#define func_name ccos
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {\
    return ccosh##_EXT(I * z); \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* ctan */

#define func_ret(_FLT) _FLT _Complex
#define func_name ctan
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {        \
    return -I * ctanh##_EXT(I * z);    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* casin */

#define func_ret(_FLT) _FLT _Complex
#define func_name casin
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {        \
    return -I * casinh##_EXT(I * z);   \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cacos */

#define func_ret(_FLT) _FLT _Complex
#define func_name cacos
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                            \
    _FLT real = creal##_EXT(z);                                            \
    _FLT imag = cimag##_EXT(z);                                            \
    if (real == 0.0 && __isnan##_EXT(imag)) {                              \
        return PI##_EXT / 2 + NAN * I;                                     \
    }                                                                      \
    if (real == 0.0 && imag == 0.0) {                                      \
        return PI##_EXT / 2 + -0 * I;                                      \
    }                                                                      \
    if (__isinf##_EXT(imag) && isfinite(real)) {                           \
        return PI##_EXT / 2 + -INFINITY * I;                               \
    }                                                                      \
    if (__isnan##_EXT(imag) && isfinite(real)) {                           \
        feraiseexcept(FE_INVALID);                                         \
        return NAN + NAN * I;                                              \
    }                                                                      \
    if (__isinf##_EXT(real)) {                                             \
        if (__isnan##_EXT(imag)) {                                         \
            return NAN + INFINITY * I;                                     \
        }                                                                  \
        if (__signbit##_EXT(real)) {                                       \
            if (__isinf##_EXT(imag)) {                                     \
                return 3 * PI##_EXT / 4 + -INFINITY * I;                   \
            } else if (imag > 0) {                                         \
                return PI##_EXT + -INFINITY * I;                           \
            }                                                              \
        } else {                                                           \
            if (__isinf##_EXT(imag)) {                                     \
                return PI##_EXT / 4 + -INFINITY * I;                       \
            } else if (imag > 0) {                                         \
                return 0 + -INFINITY * I;                                  \
            }                                                              \
        }                                                                  \
    }                                                                      \
    if (__isnan##_EXT(real)) {                                             \
        if (__isnan##_EXT(imag))                                           \
            return NAN + NAN * I;                                          \
        if (__isinf##_EXT(imag))                                           \
            return NAN + INFINITY * I;                                     \
        feraiseexcept(FE_INVALID);                                         \
        return NAN + NAN * I;                                              \
    }                                                                      \
    return PI##_EXT / 2 + I * clog##_EXT(I * z + csqrt##_EXT(1 - z * z));  \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* catan */

#define func_ret(_FLT) _FLT _Complex
#define func_name catan
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {        \
    return -I * catanh##_EXT(I * z);   \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* csinh */

#define func_ret(_FLT) _FLT _Complex
#define func_name csinh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                    \
    _FLT real = creal##_EXT(z);                                    \
    _FLT imag = cimag##_EXT(z);                                    \
    if (real < 0.0)                                                \
        return -csinh##_EXT(-z);                                   \
    if (imag < 0.0)                                                \
        return conj##_EXT(csinh##_EXT(conj##_EXT(z)));             \
    if (real == 0.0) {                                             \
        if (imag == 0.0)                                           \
            return 0.0;                                            \
        if (__isinf##_EXT(imag)) {                                 \
            feraiseexcept(FE_INVALID);                             \
            return 0 + NAN * I;                                    \
        }                                                          \
        if (__isnan##_EXT(imag)) {                                 \
            return 0 + NAN * I;                                    \
        }                                                          \
    }                                                              \
    if (__isinf##_EXT(real)) {                                     \
        if (__isinf##_EXT(imag)) {                                 \
            feraiseexcept(FE_INVALID);                             \
            return INFINITY + NAN * I;                             \
        }                                                          \
        if (__isnan##_EXT(imag))                                   \
            return INFINITY + NAN * I;                             \
        if (imag == 0.0)                                           \
            return INFINITY;                                       \
        return INFINITY * (cos##_EXT(imag) + I * sin##_EXT(imag)); \
    }                                                              \
    if (__isnan##_EXT(real)) {                                     \
        if (__isnan##_EXT(imag))                                   \
            return NAN + NAN * I;                                  \
        if (imag == 0.0)                                           \
            return NAN;                                            \
        feraiseexcept(FE_INVALID);                                 \
        return NAN + NAN * I;                                      \
    }                                                              \
    if (__isinf##_EXT(imag) || __isnan##_EXT(imag)) {              \
        feraiseexcept(FE_INVALID);                                 \
        return NAN + NAN * I;                                      \
    }                                                              \
    _FLT _Complex pez = cexp##_EXT(z);                             \
    _FLT _Complex nez = cexp##_EXT(-z);                            \
    return (pez - nez) / 2;                                        \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code


/* ccosh */

#define func_ret(_FLT) _FLT _Complex
#define func_name ccosh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                    \
    _FLT real = creal##_EXT(z);                                    \
    _FLT imag = cimag##_EXT(z);                                    \
    if (real < 0.0)                                                \
        return ccosh##_EXT(-z);                                    \
    if (imag < 0.0)                                                \
        return conj##_EXT(ccosh##_EXT(conj##_EXT(z)));             \
    if (real == 0.0) {                                             \
        if (imag == 0.0)                                           \
            return 1.0;                                            \
        if (__isinf##_EXT(imag)) {                                 \
            feraiseexcept(FE_INVALID);                             \
            return NAN;                                            \
        }                                                          \
        if (__isnan##_EXT(imag)) {                                 \
            return NAN;                                            \
        }                                                          \
    }                                                              \
    if (__isinf##_EXT(real)) {                                     \
        if (__isinf##_EXT(imag)) {                                 \
            feraiseexcept(FE_INVALID);                             \
            return INFINITY + NAN * I;                             \
        }                                                          \
        if (__isnan##_EXT(imag))                                   \
            return INFINITY + NAN * I;                             \
        if (imag == 0.0)                                           \
            return INFINITY;                                       \
        return INFINITY * (cos##_EXT(imag) + I * sin##_EXT(imag)); \
    }                                                              \
    if (__isnan##_EXT(real)) {                                     \
        if (__isnan##_EXT(imag))                                   \
            return NAN + NAN * I;                                  \
        if (imag == 0.0)                                           \
            return NAN;                                            \
        feraiseexcept(FE_INVALID);                                 \
        return NAN + NAN * I;                                      \
    }                                                              \
    if (__isinf##_EXT(imag) || __isnan##_EXT(imag)) {              \
        feraiseexcept(FE_INVALID);                                 \
        return NAN + NAN * I;                                      \
    }                                                              \
    _FLT _Complex pez = cexp##_EXT(z);                             \
    _FLT _Complex nez = cexp##_EXT(-z);                            \
    return (pez + nez) / 2;                                        \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code


/* ctanh */

#define func_ret(_FLT) _FLT _Complex
#define func_name ctanh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                                \
    _FLT real = creal##_EXT(z);                                                \
    _FLT imag = cimag##_EXT(z);                                                \
    if (real < 0.0)                                                            \
        return -ctanh##_EXT(-z);                                               \
    if (imag < 0.0)                                                            \
        return conj##_EXT(ctanh##_EXT(conj##_EXT(z)));                         \
    if (real == 0.0) {                                                         \
        if (imag == 0.0)                                                       \
            return 0.0;                                                        \
        if (__isinf##_EXT(imag)) {                                             \
            feraiseexcept(FE_INVALID);                                         \
            return NAN;                                                        \
        }                                                                      \
        if (__isnan##_EXT(imag)) {                                             \
            return NAN;                                                        \
        }                                                                      \
    }                                                                          \
    if (__isinf##_EXT(real)) {                                                 \
        return 1.0;                                                            \
    }                                                                          \
    if (__isnan##_EXT(real)) {                                                 \
        if (__isnan##_EXT(imag))                                               \
            return NAN + NAN * I;                                              \
        if (imag == 0.0)                                                       \
            return NAN;                                                        \
        feraiseexcept(FE_INVALID);                                             \
        return NAN + NAN * I;                                                  \
    }                                                                          \
    if (__isinf##_EXT(imag) || __isnan##_EXT(imag)) {                          \
        feraiseexcept(FE_INVALID);                                             \
        return NAN + NAN * I;                                                  \
    }                                                                          \
    _FLT _Complex pez = cexp##_EXT(z);                                         \
    _FLT _Complex nez = cexp##_EXT(-z);                                        \
    if (__isinf##_EXT(creal##_EXT(pez)) || __isinf##_EXT(creal##_EXT(nez)))    \
        return 1.0;                                                            \
    return (pez - nez) / (pez + nez);                                          \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* casinh */

#define func_ret(_FLT) _FLT _Complex
#define func_name casinh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                            \
    _FLT real = creal##_EXT(z);                            \
    _FLT imag = cimag##_EXT(z);                            \
    if (real < 0.0)                                        \
        return -casinh##_EXT(-z);                          \
    if (imag < 0.0)                                        \
        return conj##_EXT(casinh##_EXT(conj##_EXT(z)));    \
    if (real == 0.0 && imag == 0.0) {                      \
    }                                                      \
    if (__isinf##_EXT(real)) {                             \
        if (__isinf##_EXT(imag)) {                         \
            return INFINITY + (PI##_EXT / 4) * I;          \
        }                                                  \
        if (__isnan##_EXT(imag))                           \
            return INFINITY + NAN * I;                     \
        return INFINITY;                                   \
    }                                                      \
    if (__isnan##_EXT(real)) {                             \
        if (__isnan##_EXT(imag))                           \
            return NAN + NAN * I;                          \
        if (__isinf##_EXT(imag))                           \
            return INFINITY + NAN * I;                     \
        if (imag == 0.0)                                   \
            return NAN;                                    \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    if (__isinf##_EXT(imag)) {                             \
        return INFINITY + (PI##_EXT / 2) * I;              \
    }                                                      \
    if (__isnan##_EXT(imag)) {                             \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    return clog##_EXT(z + csqrt##_EXT(1 + z * z));         \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* cacosh */

#define func_ret(_FLT) _FLT _Complex
#define func_name cacosh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                                                    \
    _FLT real = creal##_EXT(z);                                                    \
    _FLT imag = cimag##_EXT(z);                                                    \
    if (imag < 0.0)                                                                \
        return conj##_EXT(cacosh##_EXT(conj##_EXT(z)));                            \
    if (real == 0.0 && imag == 0.0) {                                              \
        return 0.0 + (PI##_EXT / 2) * I;                                           \
    }                                                                              \
    if (__isinf##_EXT(real)) {                                                     \
        if (__isinf##_EXT(imag)) {                                                 \
            if (__signbit##_EXT(real)) return INFINITY + (3 * PI##_EXT / 4) * I;   \
            else return INFINITY + (PI##_EXT / 4) * I;                             \
        }                                                                          \
        if (__isnan##_EXT(imag))                                                   \
            return INFINITY + NAN * I;                                             \
        if (__signbit##_EXT(real))                                                 \
            return INFINITY + PI##_EXT * I;                                        \
        else                                                                       \
            return INFINITY;                                                       \
    }                                                                              \
    if (__isnan##_EXT(real)) {                                                     \
        if (__isnan##_EXT(imag))                                                   \
            return NAN + NAN * I;                                                  \
        if (__isinf##_EXT(imag))                                                   \
            return INFINITY + NAN * I;                                             \
        if (imag == 0.0)                                                           \
            return NAN;                                                            \
        feraiseexcept(FE_INVALID);                                                 \
        return NAN + NAN * I;                                                      \
    }                                                                              \
    if (__isinf##_EXT(imag)) {                                                     \
        return INFINITY + (PI##_EXT / 2) * I;                                      \
    }                                                                              \
    if (__isnan##_EXT(imag)) {                                                     \
        feraiseexcept(FE_INVALID);                                                 \
        return NAN + NAN * I;                                                      \
    }                                                                              \
    return clog##_EXT(z + csqrt##_EXT(z * z - 1));                                 \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code

/* catanh */

#define func_ret(_FLT) _FLT _Complex
#define func_name catanh
#define func_args(_FLT) (_FLT _Complex z)
#define func_code(_FLT, _EXT) {                            \
    _FLT real = creal##_EXT(z);                            \
    _FLT imag = cimag##_EXT(z);                            \
    if (imag < 0.0)                                        \
        return conj##_EXT(catanh##_EXT(conj##_EXT(z)));    \
    if (real < 0.0)                                        \
        return -catanh##_EXT(-z);                          \
    if (real == 0.0 && imag == 0.0) {                      \
        return 0.0;                                        \
    }                                                      \
    if (real == 1.0 && imag == 0.0) {                      \
        feraiseexcept(FE_DIVBYZERO);                       \
        return INFINITY;                                   \
    }                                                      \
    if (__isinf##_EXT(real)) {                             \
        if (__isinf##_EXT(imag)) {                         \
            return 0.0 + (PI##_EXT / 2) * I;               \
        }                                                  \
        if (__isnan##_EXT(imag))                           \
            return 0.0 + NAN * I;                          \
        return 0.0 + (PI##_EXT / 2) * I;                   \
    }                                                      \
    if (__isnan##_EXT(real)) {                             \
        if (__isnan##_EXT(imag))                           \
            return NAN + NAN * I;                          \
        if (__isinf##_EXT(imag))                           \
            return 0.0 + (PI##_EXT / 2) * I;               \
        if (imag == 0.0)                                   \
            return NAN;                                    \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    if (__isinf##_EXT(imag)) {                             \
        return INFINITY + (PI##_EXT / 2) * I;              \
    }                                                      \
    if (__isnan##_EXT(imag)) {                             \
        if (real == 0.0)                                   \
            return 0.0 + NAN * I;                          \
        feraiseexcept(FE_INVALID);                         \
        return NAN + NAN * I;                              \
    }                                                      \
    return (clog##_EXT(1 + z) - clog##_EXT(1 - z)) / 2;    \
}

decl

#undef func_ret
#undef func_name
#undef func_args
#undef func_code
