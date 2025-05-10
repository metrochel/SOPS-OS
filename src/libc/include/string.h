//
//	string.h - Заголовок строковых функций
//
//	Предоставляет функции для управления строками.
//

#ifndef _STRING_INCL
#define _STRING_INCL 1
#include <etc/decl.h>

// Из stddef.h нам нужен size_t.
#define __need_size_t
#include <stddef.h>

BEGIN_DECLS

// Копирует `count` символов с `src` на `dest`.
void *memcpy(void *dest, const void *src, size_t count);

// Изменяет `count` символов в `dest` на `ch`.
void *memset(void *dest, int ch, size_t count);

// Копирует строку `src` в строку `dst`.
char *strcpy(char *dest, const char *src);

// Добавляет строку `src` в конец строкии `dest`.
char *strcat(char *dest, char *src);

// Ищет первое вхождение символа `ch` в строку `src`.
// Возвращает указатель на `ch` в `src`, если `ch` в ней есть, иначе возвращает нулевой указатель.
int *strchr(char *src, int ch);

// Вычисляет длину строки `str`.
size_t strlen(const char *str);

END_DECLS

#endif