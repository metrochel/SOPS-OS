//
//	stdlib.h - Стандартные функции
//
//	В этом заголовке описаны общие часто используемые функции.
//

#if !defined(_STDLIB_INCL)
#define _STDLIB_INCL 1
#include <etc/decl.h>

// Из stddef.h нам нужен size_t.
#define __need_size_t
#include <stddef.h>

BEGIN_DECLS

// Экстренно завершает исполнение программы.
void abort(void);

// Освобождает память, занятую по адресу `ptr`.
void free(void *ptr);

// Выделяет `size` байтов памяти из кучи.
void *malloc(size_t size);

// Выделяет `count` объектов размером `sz` Б из кучи и обнуляет их.
void *calloc(size_t count, size_t sz);

// Добавляет функцию `func` в список функций, вызываемых
// после выхода из программы
int atexit(void (*func)(void));

// Переводит строку `str` в целое число.
int atoi(const char *str);

// Извлекает значение переменной окружения `name`.
char *getenv(const char *name);

// Операция модуля (y = |x|).
int abs(int n);

END_DECLS

#endif