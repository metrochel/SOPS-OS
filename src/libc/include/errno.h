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

// Код последней возникшей ошибки.
// Если 0, то всё хорошо.
#define errno __errnos[thrd_current().id]

BEGIN_DECLS

// Чшшш! Это моё! Не подсматривать!
extern int __errnos[];

END_DECLS

#endif