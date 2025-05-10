//
//  stddef.h - Общие функции, типы и прочее
//
//	В этом заголовке объявляются общие типы и структуры, необходимые программам.
//

// Тут начинается спагетти.
// (Вы, кстати, как варите макароны? Я с щипоткой __need_size_t в течение 15 минут.)

// Если вспомогательные макросы __need_X не были объявлены, то это значит,
// что заголовок включила программа, а значит, ей нужно всё.
#if !defined(_STDDEF_INCL) && !defined(__need_size_t)
#define _STDDEF_INCL 1
#define __need_size_t
#include <etc/decl.h>

BEGIN_DECLS

#if defined(__need_size_t) && !defined(_SIZE_T)
#define _SIZE_T
// `size_t` - это тип, определяющий размер какой-либо переменной. По умолчанию `unsigned long`.
typedef unsigned long size_t;
#undef __need_size_t
#endif

END_DECLS

#endif