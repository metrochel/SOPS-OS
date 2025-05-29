//
//  complex.h - Комплексные числа
//
//  Предоставляет функции для управления комплексными числами.
//

#if !defined(_COMPLEX_INCL) && !defined(__STDC_NO_COMPLEX__)
#define _COMPLEX_INCL 1

#include <etc/decl.h>

#define _Complex __complex__
#define _Imaginary __imaginary__

BEGIN_DECLS

// Макрос для упрощённого определения мнимых чисел.
#define imaginary _Imaginary
// Макрос для упрощённого определения комплексных чисел.
#define complex _Complex

// Мнимая единица
#define _Imaginary_I (__extension__ 1.0iF)
// Комплексная единица
#define _Complex_I (__extension__ 0.0iF + 1.0iF)
// i
#define I _Imaginary_I

// Создание комплексного числа из действительной и мнимой частей.
#define CMPLX(re, im) (double complex)((double)(re) + I * (double)(im))
// Создание комплексного числа из действительной и мнимой частей.
#define CMPLXF(re, im) (float complex)((float)(re) + I * (float)(im))
// Создание комплексного числа из действительной и мнимой частей.
#define CMPLXL(re, im) (long double complex)((long double)(re) + I * (long double)(im))

// Вычисляет действительную часть числа `z`.
double creal(double complex z);
// Вычисляет действительную часть числа `z`.
float crealf(float complex z);
// Вычисляет действительную часть числа `z`.
long double creall(long double complex z);

// Вычисляет мнимую часть числа `z`.
double cimag(double complex z);
// Вычисляет мнимую часть числа `z`.
float cimagf(float complex z);
// Вычисляет мнимую часть числа `z`.
long double cimagl(long double complex z);

// Вычисляет модуль числа `z`.
double cabs(double complex z);
// Вычисляет модуль числа `z`.
float cabsf(float complex z);
// Вычисляет модуль числа `z`.
long double cabsl(long double complex z);

// Вычисляет аргумент числа `z`.
double carg(double complex z);
// Вычисляет аргумент числа `z`.
float cargf(float complex z);
// Вычисляет аргумент числа `z`.
long double cargl(long double complex z);

// Вычисляет сопряжённое для числа `z`.
double conj(double complex z);
// Вычисляет сопряжённое для числа `z`.
float conjf(float complex z);
// Вычисляет сопряжённое числа `z`.
long double conjl(long double complex z);

/* Пока не знаю, что это такое, но потом дойдём. */

// Вычисляет проекцию числа `z` на сферу Риманна.
double cproj(double complex z);
// Вычисляет проекцию числа `z` на сферу Риманна.
float cprojf(float complex z);
// Вычисляет проекцию числа `z` на сферу Риманна.
long double cprojl(long double complex z);

// Вычисляет `e^z`.
double complex cexp(double complex z);
// Вычисляет `e^z`.
float complex cexpf(float complex z);
// Вычисляет `e^z`.
long double complex cexpl(long double complex z);

// Вычисляет `ln z`.
double complex clog(double complex z);
// Вычисляет `ln z`.
float complex clogf(float complex z);
// Вычисляет `ln z`.
long double complex clogl(long double complex z);

// Вычисляет `a^b`.
double complex cpow(double complex a, double complex b);
// Вычисляет `a^b`.
float complex cpowf(float complex a, float complex b);
// Вычисляет `a^b`.
long double complex cpowl(long double complex a, long double complex b);

// Вычисляет квадратный корень из `z`.
double complex csqrt(double complex z);
// Вычисляет квадратный корень из `z`.
float complex csqrtf(float complex z);
// Вычисляет квадратный корень из `z`.
long double complex csqrtl(long double complex z);

// Вычисляет синус `z`.
double complex csin(double complex z);
// Вычисляет синус `z`.
float complex csinf(float complex z);
// Вычисляет синус `z`.
long double complex csinl(long double complex z);

// Вычисляет косинус `z`.
double complex ccos(double complex z);
// Вычисляет косинус `z`.
float complex ccosf(float complex z);
// Вычисляет косинус `z`.
long double complex ccosl(long double complex z);

// Вычисляет тангенс `z`.
double complex ctan(double complex z);
// Вычисляет тангенс `z`.
float complex ctanf(float complex z);
// Вычисляет тангенс `z`.
long double complex ctanl(long double complex z);

// Вычисляет арксинус `z`.
double complex casin(double complex z);
// Вычисляет арксинус `z`.
float complex casinf(float complex z);
// Вычисляет арксинус `z`.
long double complex casinl(long double complex z);

// Вычисляет арккосинус `z`.
double complex cacos(double complex z);
// Вычисляет арккосинус `z`.
float complex cacosf(float complex z);
// Вычисляет арккосинус `z`.
long double complex cacosl(long double complex z);

// Вычисляет арктангенс `z`.
double complex catan(double complex z);
// Вычисляет арктангенс `z`.
float complex catanf(float complex z);
// Вычисляет арктангенс `z`.
long double complex catanl(long double complex z);

// Вычисляет гиперболический синус `z`.
double complex csinh(double complex z);
// Вычисляет гиперболический синус `z`.
float complex csinhf(float complex z);
// Вычисляет гиперболический синус `z`.
long double complex csinhl(long double complex z);

// Вычисляет гиперболический косинус `z`.
double complex ccosh(double complex z);
// Вычисляет гиперболический косинус `z`.
float complex ccoshf(float complex z);
// Вычисляет гиперболический косинус `z`.
long double complex ccoshl(long double complex z);

// Вычисляет гиперболический тангенс `z`.
double complex ctanh(double complex z);
// Вычисляет гиперболический тангенс `z`.
float complex ctanhf(float complex z);
// Вычисляет гиперболический тангенс `z`.
long double complex ctanhl(long double complex z);

// Вычисляет гиперболический арксинус `z`.
double complex casinh(double complex z);
// Вычисляет гиперболический арксинус `z`.
float complex casinhf(float complex z);
// Вычисляет гиперболический арксинус `z`.
long double complex casinhl(long double complex z);

// Вычисляет гиперболический арккосинус `z`.
double complex cacosh(double complex z);
// Вычисляет гиперболический арккосинус `z`.
float complex cacoshf(float complex z);
// Вычисляет гиперболический арккосинус `z`.
long double complex cacoshl(long double complex z);

// Вычисляет гиперболический арктангенс `z`.
double complex catanh(double complex z);
// Вычисляет гиперболический арктангенс `z`.
float complex catanhf(float complex z);
// Вычисляет гиперболический арктангенс `z`.
long double complex catanhl(long double complex z);

END_DECLS

#endif