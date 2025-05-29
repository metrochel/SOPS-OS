//
//  math.h - Библиотека математических функций
//
//  Предоставляет различные математические функции.
//

#ifndef _MATH_INCL
#define _MATH_INCL 1

#include <etc/decl.h>

// Здесь настолько много функций, что мне пришлось писать скрипт, который сгенерирует определения
// для огромного их пласта.
// Будьте готовы к тому, что этот заголовок будет абсолютно нечитаем.

// Очень большое число, гарантирующее переполнение (double)
#define HUGE_VAL 1e10000

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Очень большое число, гарантирующее переполнение (float)
#define HUGE_VALF 1e5000
// Очень большое число, гарантирующее переполнение (long double)
#define HUGE_VALL 1e15000

// Not a Number - постояннная, равнозначная тому, что
// возвращённый результат не имеет смысла.
#define NAN (0.0 / 0.0)

// Если функция fma работает быстрее, чем операции сложения и 
// умножения, то эти макросы должны быть определены.
// Так как я не уверен, что функция fma будет быстрее, пока что 
// я их закомментирую. (FIXME)
// #define FP_FAST_FMAF 1
// #define FP_FAST_FMA  1
// #define FP_FAST_FMAL 1

// Это значение возвращается функцией ilogb(x), если x = 0.
#define FP_ILOGB0       −0xFFFFFFFF
// Это значение возвращается функцией ilogb(x), если x - NaN.
#define FP_ILOGBNAN     -0xFFFFFFFE

// Если математические функции способны менять errno,
// то этот макрос определён как 1.
#define MATH_ERRNO      1
// Если математические функции способны создавать ошибки
// чисел с плавающей точкой, то этот макрос определён как 2.
#define MATH_ERREXCEPT  2
// Этот макрос описывает, как математические функции могут
// создавать ошибки.
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)

/* Категории чисел, возвращаемые fpclassify */

// Нормальное число
#define FP_NORMAL       0
// Бесконечность
#define FP_INFINITY     1
// Не имеет смысла
#define FP_NAN          2
// Субнормальное число
#define FP_SUBNORMAL    3
// Ноль
#define FP_ZERO         4

#endif

BEGIN_DECLS

// Вычисляет модуль числа x.
double fabs(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет модуль числа x.
float fabsf(float x);

// Вычисляет модуль числа x.
long double fabsl(long double x);

#endif

// Вычисляет остаток от деления числа x на число y.
double fmod(double x, double y);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет остаток от деления числа x на число y.
float fmodf(float x, float y);

// Вычисляет остаток от деления числа x на число y.
long double fmodl(long double x, long double y);

#endif

// Вычисляет (x * y) + z.
double fma(double x, double y, double z);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет (x * y) + z.
float fmaf(float x, float y, float z);

// Вычисляет (x * y) + z.
long double fmal(long double x, long double y, long double z);

#endif

// Возвращает наибольшее число из x и y.
double fmax(double x, double y);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Возвращает наибольшее число из x и y.
float fmaxf(float x, float y);

// Возвращает наибольшее число из x и y.
long double fmaxl(long double x, long double y);

#endif

// Вычисляет экспоненту для числа x (e^x).
double exp(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет экспоненту для числа x (e^x).
float expf(float x);

// Вычисляет экспоненту для числа x (e^x).
long double expl(long double x);

#endif

// Вычисляет натуральный логарифм числа x (ln x).
double log(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет натуральный логарифм числа x (ln x).
float logf(float x);

// Вычисляет натуральный логарифм числа x (ln x).
long double logl(long double x);

#endif

// Вычисляет логарифм числа x по основанию 10 (log_10 x).
double log10(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет логарифм числа x по основанию 10 (log_10 x).
float log10f(float x);

// Вычисляет логарифм числа x по основанию 10 (log_10 x).
long double log10l(long double x);

#endif

// Вычисляет число x в степени y (x^y).
double pow(double x, double y);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет число x в степени y (x^y).
float powf(float x, float y);

// Вычисляет число x в степени y (x^y).
long double powl(long double x, long double y);

#endif

// Вычисляет квадратный корень числа x.
double sqrt(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет квадратный корень числа x.
float sqrtf(float x);

// Вычисляет квадратный корень числа x.
long double sqrtl(long double x);

#endif

// Вычисляет синус угла x (в радианах).
double sin(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет синус угла x (в радианах).
float sinf(float x);

// Вычисляет синус угла x (в радианах).
long double sinl(long double x);

#endif

// Вычисляет косинус угла x (в радианах).
double cos(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет косинус угла x (в радианах).
float cosf(float x);

// Вычисляет косинус угла x (в радианах).
long double cosl(long double x);

#endif

// Вычисляет тангенс угла x (в радианах).
double tan(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет тангенс угла x (в радианах).
float tanf(float x);

// Вычисляет тангенс угла x (в радианах).
long double tanl(long double x);

#endif

// Вычисляет арксинус числа x.
double asin(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет арксинус числа x.
float asinf(float x);

// Вычисляет арксинус числа x.
long double asinl(long double x);

#endif

// Вычисляет арккосинус числа x.
double acos(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет арккосинус числа x.
float acosf(float x);

// Вычисляет арккосинус числа x.
long double acosl(long double x);

#endif

// Вычисляет арктангенс числа x.
double atan(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет арктангенс числа x.
float atanf(float x);

// Вычисляет арктангенс числа x.
long double atanl(long double x);

#endif

// Вычисляет арктангенс числа y/x, используя знаки y и x для определения четверти угла.
double atan2(double y, double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет арктангенс числа y/x, используя знаки y и x для определения четверти угла.
float atan2f(float y, float x);

// Вычисляет арктангенс числа y/x, используя знаки y и x для определения четверти угла.
long double atan2l(long double y, long double x);

#endif

// Вычисляет гиперболический синус угла x (в радианах).
double sinh(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет гиперболический синус угла x (в радианах).
float sinhf(float x);

// Вычисляет гиперболический синус угла x (в радианах).
long double sinhl(long double x);

#endif

// Вычисляет гиперболический косинус угла x (в радианах).
double cosh(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет гиперболический косинус угла x (в радианах).
float coshf(float x);

// Вычисляет гиперболический косинус угла x (в радианах).
long double coshl(long double x);

#endif

// Вычисляет гиперболический тангенс угла x (в радианах).
double tanh(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Вычисляет гиперболический тангенс угла x (в радианах).
float tanhf(float x);

// Вычисляет гиперболический тангенс угла x (в радианах).
long double tanhl(long double x);

#endif

// Находит наименьшее число y такое, что y >= x.
double ceil(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Находит наименьшее число y такое, что y >= x.
float ceilf(float x);

// Находит наименьшее число y такое, что y >= x.
long double ceill(long double x);

#endif

// Находит наибольшее число x такое, что x <= y.
double floor(double x);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Находит наибольшее число x такое, что x <= y.
float floorf(float x);

// Находит наибольшее число x такое, что x <= y.
long double floorl(long double x);

#endif

// Разбивает число на произведение числа и степени 2.
double frexp(double x, int *exp);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Разбивает число на произведение числа и степени 2.
float frexpf(float x, int *exp);

// Разбивает число на произведение числа и степени 2.
long double frexpl(long double x, int *exp);

#endif

// Умножает x на 2 в степени exp.
double ldexp(double x, int exp);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Умножает x на 2 в степени exp.
float ldexpf(float x, int exp);

// Умножает x на 2 в степени exp.
long double ldexpl(long double x, int exp);

#endif

// Разбивает число x на целую и дробную часть. Целая часть записывается
// в y, а дробная возвращается.
double modf(double x, double *y);

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L

// Разбивает число x на целую и дробную часть. Целая часть записывается
// в y, а дробная возвращается.
float modff(float x, float *y);

// Разбивает число x на целую и дробную часть. Целая часть записывается
// в y, а дробная возвращается.
long double modfl(long double x, long double *y);

#endif

#if defined(__cplusplus) && __cplusplus >= 201100L \
    || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L
    
// Вычисляет кубический корень числа x.
double cbrt(double x);

// Вычисляет кубический корень числа x.
float cbrtf(float x);

// Вычисляет кубический корень числа x.
long double cbrtl(long double x);

// Вычисляет гипотенузу прямоугольного 
// треугольника со сторонами x и y.
double hypot(double x, double y);

// Вычисляет гипотенузу прямоугольного 
// треугольника со сторонами x и y.
float hypotf(float x, float y);

// Вычисляет гипотенузу прямоугольного 
// треугольника со сторонами x и y.
long double hypotl(long double x, long double y);

// Вычисляет гиперболический арксинус числа x.
double asinh(double x);

// Вычисляет гиперболический арксинус числа x.
float asinhf(float x);

// Вычисляет гиперболический арксинус числа x.
long double asinhl(long double x);

// Вычисляет гиперболический арккосинус числа x.
double acosh(double x);

// Вычисляет гиперболический арккосинус числа x.
float acoshf(float x);

// Вычисляет гиперболический арккосинус числа x.
long double acoshl(long double x);

// Вычисляет гиперболический арктангенс числа x.
double atanh(double x);

// Вычисляет гиперболический арктангенс числа x.
float atanhf(float x);

// Вычисляет гиперболический арктангенс числа x.
long double atanhl(long double x);

// Вычисляет функцию ошибки от данного x.
double erf(double x);

// Вычисляет функцию ошибки от данного x.
float erff(float x);

// Вычисляет функцию ошибки от данного x.
long double erfl(long double x);

// Вычисляет комплементарную функцию ошибки от данного x (1 - erf(x)).
double erfc(double x);

// Вычисляет комплементарную функцию ошибки от данного x (1 - erf(x)).
float erfcf(float x);

// Вычисляет комплементарную функцию ошибки от данного x (1 - erf(x)).
long double erfcl(long double x);

// Вычисляет гамма-функцию от данного x (Г(x)).
double tgamma(double x);

// Вычисляет гамма-функцию от данного x (Г(x)).
float tgammaf(float x);

// Вычисляет гамма-функцию от данного x (Г(x)).
long double tgammal(long double x);

// Вычисляет натуральный логарифм гамма-функции от данного x (ln Г(x)).
double lgamma(double x);

// Вычисляет натуральный логарифм гамма-функции от данного x (ln Г(x)).
float lgammaf(float x);

// Вычисляет натуральный логарифм гамма-функции от данного x (ln Г(x)).
long double lgammal(long double x);

// Для данного числа x вычисляет целое число y, ближайшее по модулю к x.
double trunc(double x);

// Для данного числа x вычисляет целое число y, ближайшее по модулю к x.
float truncf(float x);

// Для данного числа x вычисляет целое число y, ближайшее по модулю к x.
long double truncl(long double x);

// Округляет число x.
double round(double x);

// Округляет число x.
float roundf(float x);

// Округляет число x.
long double roundl(long double x);

// Округляет число x.
long lround(double x);

// Округляет число x.
long lroundf(float x);

// Округляет число x.
long lroundl(long double x);

// Округляет число x.
long long llround(double x);

// Округляет число x.
long long llroundf(float x);

// Округляет число x.
long long llroundl(long double x);

// Округляет число x в соответствии с текущим режимом округления.
double nearbyint(double x);

// Округляет число x в соответствии с текущим режимом округления.
float nearbyintf(float x);

// Округляет число x в соответствии с текущим режимом округления.
long double nearbyintl(long double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
double rint(double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
float rintf(float x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long double rintl(long double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long lrint(double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long lrintf(float x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long lrintl(long double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long long llrint(double x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long long llrintf(float x);

// Округляет число x в соответствии с текущим режимом округления и создаёт
// ошибку при расхождении ответа.
long long llrintl(long double x);

// Возвращает произведение числа x и 2 в степени n.
double scalbn(double x, int n);

// Возвращает произведение числа x и 2 в степени n.
float scalbnf(float x, int n);

// Возвращает произведение числа x и 2 в степени n.
long double scalbnl(long double x, int n);

// Возвращает произведение числа x и 2 в степени n.
double scalbln(double x, long n);

// Возвращает произведение числа x и 2 в степени n.
float scalblnf(float x, long n);

// Возвращает произведение числа x и 2 в степени n.
long double scalblnl(long double x, long n);

// Вычисляет целый экспонент числа x.
int ilogb(double x);

// Вычисляет целый экспонент числа x.
int ilogbf(float x);

// Вычисляет целый экспонент числа x.
int ilogbl(long double x);

// Вычисляет экспонент числа x.
double logb(double x);

// Вычисляет экспонент числа x.
float logbf(float x);

// Вычисляет экспонент числа x.
long double logbl(long double x);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
double nextafter(double x, double y);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
float nextafterf(float x, float y);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
long double nextafterl(long double x, long double y);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
double nexttoward(double x, double y);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
float nexttowardf(float x, float y);

// Возвращает значение, следующее после числа x в направлении числа y,
// которое можно записать в данном типе.
long double nexttowardl(long double x, long double y);

// Возвращает число x со знаком числа y.
double copysign(double x, double y);

// Возвращает число x со знаком числа y.
float copysignf(float x, float y);

// Возвращает число x со знаком числа y.
long double copysignl(long double x, long double y);

// FIXME: Непохоже, что напрямую прописывать макросы в заголовке -
// хорошая идея. Перенести.

// Классифицирует число.
#define fpclassify(x) (\
    __isinf(x)                      ? FP_INFINITY   : \
    __isnan(x)                      ? FP_NAN        : \
    x == 0                          ? FP_ZERO       : \
    __is_subnormal(x)               ? FP_SUBNORMAL  : \
    FP_NORMAL \
    )

// Проверяет, является ли число конечным.
#define isfinite(x) (!isinf(x) && !isnan(x))

// Проверяет, является ли число бесконечностью.
#define isinf(x) __isinf(x)

// Проверяет, является ли число NaN (Not a Number).
#define isnan(x) __isnan(x)

// Проверяет знак числа.
#define signbit(x) __signbit(x)

// Проверяет для чисел x и y, что x > y.
#define isgreater(x, y) __greater(x, y)

// Проверяет для чисел x и y, что x >= y.
#define isgreaterequal(x, y) __greatereq(x, y)

// Проверяет для чисел x и y, что x < y.
#define isless(x, y) __less(x, y)

// Проверяет для чисел x и y, что x <= y.
#define islessequal(x, y) __lesseq(x, y)

// Проверяет для чисел x и y, что x > y или x < y.
#define islessgreater(x, y) __lessgreater(x, y)

// Проверяет для чисел x и y, что x и y невозможно сравнить (хотя бы одно из них NaN).
#define isunordered(x, y) __unordered(x, y)

#endif

END_DECLS

#endif