//
//  stdbit.h - Функции для работы с битовыми представлениями чисел
//
//  Определяет штуки, связанные с битами в двоичных записях чисел.
//

#if !defined(_STDBIT_INCL) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202300L

#include <etc/decl.h>

// Означает, что библиотека воспринимает числа в формате little-endian.
#define __STDC_ENDIAN_LITTLE__  0
// Означает, что библиотека воспринимает числа в формате big-endian.
#define __STDC_ENDIAN_BIG__     1
// Расширяется в нативный формат для чисел.
// Мы используем стандартные x86-процессоры, поэтому для них little-endian.
#define __STDC_ENDIAN_NATIVE__  __STDC_ENDIAN_LITTLE__

BEGIN_DECLS

// Определяет количество битов-ведущих нулей в числе x.
unsigned int stdc_leading_zeroes_uc(unsigned char x);

// Определяет количество битов-ведущих нулей в числе x.
unsigned int stdc_leading_zeroes_us(unsigned short x);

// Определяет количество битов-ведущих нулей в числе x.
unsigned int stdc_leading_zeroes_ui(unsigned int x);

// Определяет количество битов-ведущих нулей в числе x.
unsigned int stdc_leading_zeroes_ul(unsigned long x);

// Определяет количество битов-ведущих нулей в числе x.
unsigned int stdc_leading_zeroes_ull(unsigned long long x);


// Определяет количество битов-ведущих единиц в числе x.
unsigned int stdc_leading_ones_uc(unsigned char x);

// Определяет количество битов-ведущих единиц в числе x.
unsigned int stdc_leading_ones_us(unsigned short x);

// Определяет количество битов-ведущих единиц в числе x.
unsigned int stdc_leading_ones_ui(unsigned int x);

// Определяет количество битов-ведущих единиц в числе x.
unsigned int stdc_leading_ones_ul(unsigned long x);

// Определяет количество битов-ведущих единиц в числе x.
unsigned int stdc_leading_ones_ull(unsigned long long x);


// Определяет количество битов-нулей на конце в числе x.
unsigned int stdc_trailing_zeroes_uc(unsigned char x);

// Определяет количество битов-нулей на конце в числе x.
unsigned int stdc_trailing_zeroes_us(unsigned short x);

// Определяет количество битов-нулей на конце в числе x.
unsigned int stdc_trailing_zeroes_ui(unsigned int x);

// Определяет количество битов-нулей на конце в числе x.
unsigned int stdc_trailing_zeroes_ul(unsigned long x);

// Определяет количество битов-нулей на конце в числе x.
unsigned int stdc_trailing_zeroes_ull(unsigned long long x);


// Определяет количество битов-единиц на конце в числе x.
unsigned int stdc_trailing_ones_uc(unsigned char x);

// Определяет количество битов-единиц на конце в числе x.
unsigned int stdc_trailing_ones_us(unsigned short x);

// Определяет количество битов-единиц на конце в числе x.
unsigned int stdc_trailing_ones_ui(unsigned int x);

// Определяет количество битов-единиц на конце в числе x.
unsigned int stdc_trailing_ones_ul(unsigned long x);

// Определяет количество битов-единиц на конце в числе x.
unsigned int stdc_trailing_ones_ull(unsigned long long x);


// Определяет первый ведущий бит-ноль в числе x.
unsigned int stdc_first_leading_zero_uc(unsigned char x);

// Определяет первый ведущий бит-ноль в числе x.
unsigned int stdc_first_leading_zero_us(unsigned short x);

// Определяет первый ведущий бит-ноль в числе x.
unsigned int stdc_first_leading_zero_ui(unsigned int x);

// Определяет первый ведущий бит-ноль в числе x.
unsigned int stdc_first_leading_zero_ul(unsigned long x);

// Определяет первый ведущий бит-ноль в числе x.
unsigned int stdc_first_leading_zero_ull(unsigned long long x);


// Определяет первый ведущий бит-единицу в числе x.
unsigned int stdc_first_leading_one_uc(unsigned char x);

// Определяет первый ведущий бит-единицу в числе x.
unsigned int stdc_first_leading_one_us(unsigned short x);

// Определяет первый ведущий бит-единицу в числе x.
unsigned int stdc_first_leading_one_ui(unsigned int x);

// Определяет первый ведущий бит-единицу в числе x.
unsigned int stdc_first_leading_one_ul(unsigned long x);

// Определяет первый ведущий бит-единицу в числе x.
unsigned int stdc_first_leading_one_ull(unsigned long long x);


// Определяет первый бит-ноль на конце числа x.
unsigned int stdc_first_trailing_zero_uc(unsigned char x);

// Определяет первый бит-ноль на конце числа x.
unsigned int stdc_first_trailing_zero_us(unsigned short x);

// Определяет первый бит-ноль на конце числа x.
unsigned int stdc_first_trailing_zero_ui(unsigned int x);

// Определяет первый бит-ноль на конце числа x.
unsigned int stdc_first_trailing_zero_ul(unsigned long x);

// Определяет первый бит-ноль на конце числа x.
unsigned int stdc_first_trailing_zero_ull(unsigned long long x);


// Определяет первый бит-единицу на конце числа x.
unsigned int stdc_first_trailing_one_uc(unsigned char x);

// Определяет первый бит-единицу на конце числа x.
unsigned int stdc_first_trailing_one_us(unsigned short x);

// Определяет первый бит-единицу на конце числа x.
unsigned int stdc_first_trailing_one_ui(unsigned int x);

// Определяет первый бит-единицу на конце числа x.
unsigned int stdc_first_trailing_one_ul(unsigned long x);

// Определяет первый бит-единицу на конце числа x.
unsigned int stdc_first_trailing_one_ull(unsigned long long x);


// Определяет количество битов-нулей в числе x.
unsigned int stdc_count_zeroes_uc(unsigned char x);

// Определяет количество битов-нулей в числе x.
unsigned int stdc_count_zeroes_us(unsigned short x);

// Определяет количество битов-нулей в числе x.
unsigned int stdc_count_zeroes_ui(unsigned int x);

// Определяет количество битов-нулей в числе x.
unsigned int stdc_count_zeroes_ul(unsigned long x);

// Определяет количество битов-нулей в числе x.
unsigned int stdc_count_zeroes_ull(unsigned long long x);


// Определяет количество битов-единиц в числе x.
unsigned int stdc_count_ones_uc(unsigned char x);

// Определяет количество битов-единиц в числе x.
unsigned int stdc_count_ones_us(unsigned short x);

// Определяет количество битов-единиц в числе x.
unsigned int stdc_count_ones_ui(unsigned int x);

// Определяет количество битов-единиц в числе x.
unsigned int stdc_count_ones_ul(unsigned long x);

// Определяет количество битов-единиц в числе x.
unsigned int stdc_count_ones_ull(unsigned long long x);


// Проверяет, что в числе x ровно один бит-единица (число есть
// степень 2).
unsigned int stdc_has_single_bit_uc(unsigned char x);

// Проверяет, что в числе x ровно один бит-единица (число есть
// степень 2).
unsigned int stdc_has_single_bit_us(unsigned short x);

// Проверяет, что в числе x ровно один бит-единица (число есть
// степень 2).
unsigned int stdc_has_single_bit_ui(unsigned int x);

// Проверяет, что в числе x ровно один бит-единица (число есть
// степень 2).
unsigned int stdc_has_single_bit_ul(unsigned long x);

// Проверяет, что в числе x ровно один бит-единица (число есть
// степень 2).
unsigned int stdc_has_single_bit_ull(unsigned long long x);


// Определяет, сколько бит достаточно, чтобы можно было
// записать число x.
unsigned int stdc_bit_width_uc(unsigned char x);

// Определяет, сколько бит достаточно, чтобы можно было
// записать число x.
unsigned int stdc_bit_width_us(unsigned short x);

// Определяет, сколько бит достаточно, чтобы можно было
// записать число x.
unsigned int stdc_bit_width_ui(unsigned int x);

// Определяет, сколько бит достаточно, чтобы можно было
// записать число x.
unsigned int stdc_bit_width_ul(unsigned long x);

// Определяет, сколько бит достаточно, чтобы можно было
// записать число x.
unsigned int stdc_bit_width_ull(unsigned long long x);


// Вычисляет наибольшую степень числа 2, не превышающую x.
unsigned int stdc_bit_floor_uc(unsigned char x);

// Вычисляет наибольшую степень числа 2, не превышающую x.
unsigned int stdc_bit_floor_us(unsigned short x);

// Вычисляет наибольшую степень числа 2, не превышающую x.
unsigned int stdc_bit_floor_ui(unsigned int x);

// Вычисляет наибольшую степень числа 2, не превышающую x.
unsigned int stdc_bit_floor_ul(unsigned long x);

// Вычисляет наибольшую степень числа 2, не превышающую x.
unsigned int stdc_bit_floor_ull(unsigned long long x);


// Вычисляет наименьшую степень числа 2, превышающую или равную x.
unsigned int stdc_bit_ceil_uc(unsigned char x);

// Вычисляет наименьшую степень числа 2, превышающую или равную x.
unsigned int stdc_bit_ceil_us(unsigned short x);

// Вычисляет наименьшую степень числа 2, превышающую или равную x.
unsigned int stdc_bit_ceil_ui(unsigned int x);

// Вычисляет наименьшую степень числа 2, превышающую или равную x.
unsigned int stdc_bit_ceil_ul(unsigned long x);

// Вычисляет наименьшую степень числа 2, превышающую или равную x.
unsigned int stdc_bit_ceil_ull(unsigned long long x);

END_DECLS

#endif