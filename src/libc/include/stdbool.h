//
//  stdbool.h - Заголовок с типом bool
//
//  Предоставляет функции для булевого типа данных.
//

#if defined __STDC_VERSION__ && __STDC_VERSION__ < 202300L && __STDC_VERSION__ >= 199900L

// `bool` - это тип данных, принимающий только два значения: `true` или `false`.
#define bool _Bool

// Значение логической ИСТИНЫ
#define true 1
// Значение логической ЛЖИ
#define false 0

#endif

#define __bool_true_false_are_defined 1