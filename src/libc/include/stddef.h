//
//  stddef.h - Общие функции, типы и прочее
//
//	В этом заголовке объявляются общие типы и структуры, необходимые программам.
//

// Тут начинается спагетти.
// (Вы, кстати, как варите макароны? Я с щипоткой __need_size_t в течение 15 минут.)

#if !defined(_STDDEF_INCL)

// Если вспомогательные макросы __need_X не были объявлены, то это значит,
// что заголовок включила программа, а значит, ей нужно всё.
#if !defined(__need_size_t) && !defined(__need_NULL) && \
    !defined(__need_wchar_t)
#define _STDDEF_INCL 1
#define __need_size_t
#define __need_NULL
#define __need_wchar_t
#endif

#include <etc/decl.h>

BEGIN_DECLS

#if defined(__need_size_t) && !defined(_SIZE_T)
#define _SIZE_T
// `size_t` - это тип, определяющий размер какой-либо переменной. По умолчанию `unsigned long`.
typedef __SIZE_TYPE__ size_t;
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L && \
    defined(__STDC_LIB_EXT1__) && defined(__STDC_LIB_WANT_EXT1__)
// `rsize_t` - это тип, определяющий размер ровно одного объекта какого-либо типа.
typedef size_t rsize_t;
#endif
#undef __need_size_t
#endif

#if defined(__need_wchar_t) && !defined(_WCHAR_T)
#define _WCHAR_T
// `wchar_t` - это тип, достаточный для одного широкого символа.
typedef __WCHAR_TYPE__ wchar_t;
#undef __need_wchar_t
#endif

#if defined(__need_NULL) && !defined(NULL)
// NULL - это макрос, определяющий нулевой указатель.
#define NULL ((void*)0)
#undef __need_NULL
#endif

// Если не определены макросы `__need_X`, то надо всё.
// FIXME?: Тут очень нечистая магия, надо будет к этому вернуться.
#ifdef _STDDEF_INCL

typedef __PTRDIFF_TYPE__ ptrdiff_t;

// Вычисляет сдвиг члена `D` в структуре `P`.
#define offsetof(P, D) __builtin_offsetof(P, D)

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202300L) || \
    (defined(__cplusplus) && __cplusplus >= 201700L)

// `nullptr_t` - это тип, которым обладает `nullptr`.
typedef void* nullptr_t;

#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L) || \
    (defined(__cplusplus) && __cplusplus >= 201700L) &&           \
    !defined(_GCC_MAX_ALIGN_T)
// Здесь происходит конфликт объявлений. Почему-то GCC не различает своё объявление
// max_align_t и моё, хотя я практически скопировал объявление glibc. GCC использует
// макрос _GCC_MAX_ALIGN_T как include-guard, поэтому мы повторим это здесь.
#define _GCC_MAX_ALIGN_T

// `max_align_t` - это тип, который имеет максимально возможное выравнивание.
typedef struct {
	// Судя по всему, такое определение имеет GCC.

	long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
	long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));

#ifdef __i386__
    __float128 __max_align_f128 __attribute__((__aligned__(__alignof(__float128))));
#endif
} max_align_t;

#endif

// Если версия C++ равна 17 или больше, то надо определить следующее:
#if defined(__cplusplus) && __cplusplus >= 201700L

// `byte` - это тип, размер которого равен 1 байту.
typedef unsigned char byte;

// Преобразовывает байт `b` в целое число.
int to_integer(byte b);

#endif

#endif

END_DECLS

#endif