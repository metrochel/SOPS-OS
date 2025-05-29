//
//  mathcall.h - Макрос MATH_FUNC
//
//  Определяет макросы MATH_FUNC для заголовка tgmath.h.
//

// Проверяет, что число имеет тип целого числа (char, short, int, long, long long).
#define __MATH_IS_INT(x) __extension__({                                      \
    __typeof__((x) + (long long)0) __x = x;                                   \
    (*(long long*)&(__x) == __x) || (*(unsigned long long*)&(__x) == __x);    \
})

// Вызывает математическую функцию с 1 аргументом.
#define __MATH_FUNC1(function, arg1)  (                                             \
    sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__  ? function##l((long double)(arg1))  : \
    sizeof((arg1)) == __SIZEOF_DOUBLE__       ? function((double)(arg1))          : \
    __MATH_IS_INT((arg1))                     ? function((double)(arg1))          : \
    function##f((long double)(arg1))                                                \
)

// Вызывает математическую функцию с 2 аргументами. Учитывает тип только 1 аргумента.
#define __MATH_FUNC2_PRESERVE(function, arg1, arg2)  (                                      \
    sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__  ? function##l((long double)(arg1), (arg2))  : \
    sizeof((arg1)) == __SIZEOF_DOUBLE__       ? function((double)(arg1), (arg2))          : \
    __MATH_IS_INT((arg1))                     ? function((double)(arg1), (arg2))          : \
    function##f((long double)(arg1), (arg2))                                                \
)

// Вызывает математическую функцию с 2 аргументами.
#define __MATH_FUNC2(function, arg1, arg2) ( \
    (sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__  || sizeof((arg2)) == __SIZEOF_LONG_DOUBLE__) ? \
        function##l((long double)(arg1), (long double)(arg2))                               : \
    (sizeof((arg1)) == __SIZEOF_DOUBLE__       || sizeof((arg2)) == __SIZEOF_DOUBLE__)      ? \
        function((double)(arg1), (double)(arg2))                                            : \
    (__MATH_IS_INT((arg1)) || __MATH_IS_INT((arg2)))                                        ? \
        function((double)(arg1), (double)(arg2))                                            : \
    function##f((float)(arg1), (float)(arg2))                                                 \
)

// Вызывает функцию с 3 аргументами. Учитывает тип только 1 и 2 аргумента.
#define __MATH_FUNC3_PRESERVE(function, arg1, arg2, arg3) (                                   \
    (sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__  || sizeof((arg2)) == __SIZEOF_LONG_DOUBLE__) ? \
        function##l((long double)(arg1), (long double)(arg2), (arg3))                       : \
    (sizeof((arg1)) == __SIZEOF_DOUBLE__       || sizeof((arg2)) == __SIZEOF_DOUBLE__)      ? \
        function((double)(arg1), (double)(arg2), (arg3))                                    : \
    (__MATH_IS_INT((arg1)) || __MATH_IS_INT((arg2)))                                        ? \
        function((double)(arg1), (double)(arg2), (arg3))                                    : \
    function##f((float)(arg1), (float)(arg2), (arg3))                                         \
)

// Вызывает математическую функцию с 3 аргументами.
#define __MATH_FUNC3(function, arg1, arg2, arg3) ( \
    (sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__  || sizeof((arg2)) == __SIZEOF_LONG_DOUBLE__ || \
        sizeof((arg3)) == __SIZEOF_LONG_DOUBLE__) ?                                           \
        function##l((long double)(arg1), (long double)(arg2), (long double)(arg3))          : \
    (sizeof((arg1)) == __SIZEOF_DOUBLE__       || sizeof((arg2)) == __SIZEOF_DOUBLE__ ||      \
        sizeof((arg3)) == __SIZEOF_DOUBLE__)                                                ? \
        function((double)(arg1), (double)(arg2))                                            : \
    (__MATH_IS_INT((arg1)) || __MATH_IS_INT((arg2)) || __MATH_IS_INT((arg3)))               ? \
        function((double)(arg1), (double)(arg2), (double)(arg3))                            : \
    function##f((float)(arg1), (float)(arg2), (float)(arg3))                                  \
)

#ifndef __STDC_NO_COMPLEX__

// Вызывает математическую функцию с 1 аргументом. Допускает комплексный аргумент.
#define __MATH_FUNC1C_CFUNC(function, cfunction, arg1)  \
    (((arg1) - (long double)(arg1)) ? ( \
        sizeof((arg1)) == 2 * __SIZEOF_LONG_DOUBLE__    ? c ## function ## l((long double complex)(arg1)) : \
        sizeof((arg1)) == 2 * __SIZEOF_DOUBLE__         ? c ## function((double complex)(arg1))         :   \
        c ## function ## f((float complex)(arg1))                                                           \
    ) : ( \
        sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__        ? function ## l((long double)(arg1))             : \
        sizeof((arg1)) == __SIZEOF_DOUBLE__             ? function((double)(arg1))                       : \
        __MATH_IS_INT((arg1))                           ? function((double)(arg1))                       : \
        function ## f((float)(arg1))                                                                       \
))

// Вызывает математическую функцию с 1 аргументом. Допускает комплексный аргумент.
#define __MATH_FUNC2C_CFUNC(function, cfunction, arg1, arg2)  \
    (((arg1) - (long double)(arg1) || (arg2) - (long double)(arg2)) ? ( \
        (sizeof((arg1)) == 2 * __SIZEOF_LONG_DOUBLE__ || sizeof((arg2)) == 2 * __SIZEOF_LONG_DOUBLE__)    ? \
            cfunction ## l((long double complex)(arg1), (long double complex)(arg2))                      : \
        (sizeof((arg1)) == 2 * __SIZEOF_DOUBLE__ || sizeof((arg2)) == 2 * __SIZEOF_DOUBLE__)              ? \
            cfunction((double complex)(arg1), (double complex)(arg2))                                     : \
        cfunction ## f((float complex)(arg1), (float complex)(arg2))                                        \
    ) : ( \
        (sizeof((arg1)) == __SIZEOF_LONG_DOUBLE__ || sizeof((arg2)) == SIZEOF_LONG_DOUBLE__)              ? \
            function ## l((long double)(arg1), (long double)(arg2))                                       : \
        (sizeof((arg1)) == __SIZEOF_DOUBLE__ || sizeof((arg1)) == __SIZEOF_DOUBLE__)                      ? \
            function((double)(arg1), (double)(arg2))                                                      : \
        (__MATH_IS_INT((arg1)) || __MATH_IS_INT((arg2)))                                                  ? \
            function((double)(arg1), (double)(arg2))                                                      : \
        function ## f((float)(arg1), (float)(arg2))                                                         \
))

// Вызывает математическую функцию с 1 комплексным аргументом.
#define __MATH_FUNC1_COMPLEX(function, arg1) ( \
    sizeof((arg1)) == 2 * __SIZEOF_LONG_DOUBLE__                                                          ? \
    function##l((arg1))                                                                                   : \
    sizeof((arg1)) == 2 * __SIZEOF_DOUBLE__                                                               ? \
    function((arg1))                                                                                      : \
    \
    function##f((arg1))                                                                                   \
)

#else

#define __MATH_FUNC1C_CFUNC(function, cfunction, arg1)       __MATH_FUNC1(function, (arg1))
#define __MATH_FUNC2C_CFUNC(function, cfunction, arg1, arg2) __MATH_FUNC2(function, (arg1), (arg2))

#endif

#define __MATH_FUNC_1C(function, arg1)       __MATH_FUNC1C_CFUNC(function, c ## function, (arg1))
#define __MATH_FUNC_2C(function, arg1, arg2) __MATH_FUNC2C_CFUNC(function, c ## function, (arg1), (arg2))