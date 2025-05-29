//
//	errno.h - Заголовок про ошибки
//
//	В этом заголовке можно определить тип ошибки,
//  которую сгенерировала функция.
//

#if !defined(_ERRNO_INCLUDE)
#define _ERRNO_INCLUDE 1
#include <etc/decl.h>

/* Коды ошибок */
// Ошибка области определения
#define EDOM 1
// Ошибка неверной последовательности
#define EILSEQ 2
// Ошибка пределов (обычно целочисленное переполнение)
#define ERANGE 3

BEGIN_DECLS

// Код последней возникшей ошибки.
// Если ошибки нет, значение `errno` равно нулю.
extern int errno;

END_DECLS

#endif