//
//	stdarg.h - Поддержка разного числа аргументов
//
//	Предоставляет тип va_list и макросы, связанные с ним.
//

#ifndef _STDARG_INCL
#define _STDARG_INCL
#include <etc/decl.h>

BEGIN_DECLS

typedef __builtin_va_list va_list;

// Начинает список `v`.
#define va_start(v, p) __builtin_va_start(v, p)

// Завершает список `v`.
#define va_end(v) __builtin_va_end(v)

// Берёт из списка `v` следующий аргумент типа `p`.
#define va_arg(v, p) __builtin_va_arg(v, p)

// Копирует список `src` в список `dst`.
#define va_copy(dst, src) __builtin_va_copy(dst, src)

END_DECLS

#endif