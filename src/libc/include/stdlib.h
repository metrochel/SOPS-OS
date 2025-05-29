//
//	stdlib.h - Стандартные функции
//
//	В этом заголовке описаны общие часто используемые функции.
//

#if !defined(_STDLIB_INCL)
#define _STDLIB_INCL 1

#include <etc/decl.h>

BEGIN_DECLS

// Из stddef.h нам нужен size_t.
#define __need_size_t
#define __need_NULL
#include <stddef.h>

// `div_t` - это тип-результат целочисленного деления для `int`.
typedef struct {
    int quot;   // Частное
    int rem;    // Остаток
} div_t;

// `div_t` - это тип-результат целочисленного деления для `long int`.
typedef struct {
    long int quot;   // Частное
    long int rem;    // Остаток
} ldiv_t;

// `div_t` - это тип-результат целочисленного деления для `long long int`.
typedef struct {
    long long int quot;   // Частное
    long long int rem;    // Остаток
} lldiv_t;

// `compare_func_t` - это тип-функция для сравнения двух элементов.
// Используется в функциях `bsearch` и `qsort`.
typedef int (compare_func_t)(void*, void*);

// Экстренно завершает исполнение программы.
void _Noreturn abort(void);

// Завершает исполнение программы с кодом выхода `code`.
void _Noreturn exit(int code);

// Добавляет функцию `func` в список функций, вызываемых
// после выхода из программы
int atexit(void (*func)(void));

// Вызывает команду в строке `str` и возвращает её код выхода.
int system(const char *str);

// Получает значение переменной окружения по имени `name`.
char *getenv(const char *name);

// Выделяет `size` байтов памяти из кучи.
void *malloc(size_t size);

// Выделяет `count` объектов размером `sz` Б из кучи и обнуляет их.
void *calloc(size_t count, size_t sz);

// Освобождает память для `ptr` и выделяет её снова, в этот раз в размере `newSz` Б.
void *realloc(void *ptr, size_t newSz);

// Освобождает память, занятую по адресу `ptr`.
void free(void *ptr);

// Переводит строку `str` в число с плавающей точкой.
float atof(const char *str);

// Переводит строку `str` в целое число.
int atoi(const char *str);

// Переводит строку `str` в целое число.
long int atol(const char *str);

// Переводит строку `str` в целое число.
long long int atoll(const char *str);

// Переводит строку `str` в целое число по основанию `base`.
// Если `base` равна 0, то основание определяется автоматически.
// После выполнения `*str_end` будет указывать на последний символ строки.
long strtol(const char *str, char **str_end, int base);

// Переводит строку `str` в целое число по основанию `base`.
// Если `base` равна 0, то основание определяется автоматически.
// После выполнения `*str_end` будет указывать на последний символ строки.
long long strtoll(const char *str, char **str_end, int base);

// Переводит строку `str` в беззнаковое целое число по основанию `base`.
// Если `base` равна 0, то основание определяется автоматически.
// После выполнения `*str_end` будет указывать на последний символ строки.
unsigned long strtoul(const char *str, char **str_end, int base);

// Переводит строку `str` в беззнаковое целое число по основанию `base`.
// Если `base` равна 0, то основание определяется автоматически.
// После выполнения `*str_end` будет указывать на последний символ строки.
unsigned long long strtoull(const char *str, char **str_end, int base);

// Переводит строку `str` в число с плавающей точкой.
// После выполнения `*str_end` будет указывать на последний символ строки.
float strtof(const char *str, char **str_end);

// Переводит строку `str` в число с плавающей точкой.
// После выполнения `*str_end` будет указывать на последний символ строки.
double strtod(const char *str, char **str_end);

// Переводит строку `str` в число с плавающей точкой.
// После выполнения `*str_end` будет указывать на последний символ строки.
long double strtold(const char *str, char **str_end);

// Смотрит размер символа в многобайтовой строке `s`, причём он не может превышать `n`.
int mblen(const char *s, size_t n);

// Переводит многобайтовый символ в `s` в широкий символ и записывает его в `wc`.
int mbtowc(wchar_t *wc, const char *s, size_t n);

// Переводит широкий символ в `wc` в многобайтовый символ и записывает его в `s`.
int wctomb(char *s, wchar_t wc);

// Переводит многобайтовую строку `mbs` в широкосимвольную строку `wcs`.
// Размер символа в `mbs` не превышает `n`.
size_t mbstowcs(wchar_t *wcs, const char *mbs, size_t n);

// Переводит широкосимвольную строку `wcs` в многобайтовую строку `mbs`.
// Размер символа в `mbs` не превышает `n`.
size_t wcstombs(char *mbs, const wchar_t *wcs, size_t n);

// Генерирует случайное число.
int rand();

// Устанавливает семя для генератора случайных чисел.
void srand(unsigned int seed);

// Ищет `key` в массиве `base` длиной `nmemb` элементов, каждый размером `sz`.
// Сравнение производится в соответствии с функцией `compare`.
void *bsearch(const void *key, const void *base, size_t nmemb, size_t sz, compare_func_t *compare);

// Сортирует массив `base` длиной `nmemb` элементов, каждый размером `sz`.
// Сравнение производится в соответствии с функцией `compare`.
void *qsort(void *base, size_t nmemb, size_t sz, compare_func_t *compare);

// Операция модуля (y = |x|).
int abs(int n);

// Операция модуля (y = |x|).
long int labs(long int n);

// Операция модуля (y = |x|).
long long int llabs(long long int n);

// Делит число `num` на число `div`.
div_t div(int num, int div);

// Делит число `num` на число `div`.
ldiv_t ldiv(long int num, long int div);

// Делит число `num` на число `div`.
lldiv_t lldiv(long int num, long int div);


END_DECLS


#endif