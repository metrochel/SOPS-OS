//
//	string.h - Заголовок строковых функций
//
//	Предоставляет функции для управления строками.
//

#ifndef _STRING_INCL
#define _STRING_INCL 1
#include <etc/decl.h>

// Из stddef.h нам нужен size_t и NULL.
#define __need_size_t
#define __need_NULL
#include <stddef.h>

BEGIN_DECLS

// Копирует строку `src` в строку `dst`.
char *strcpy(char *dest, const char *src);

// Копирует `n` символов строки `src` в строку `dest`.
char *strncpy(char *dest, const char *src, size_t n);

// Добавляет строку `src` в конец строки `dest`.
char *strcat(char *dest, const char *src);

// Добавляет `n` символов из строки `src` в конец строки `dest`.
char *strncat(char *dest, const char *src, size_t n);

// Преобразует строку `str1` так, что результат сравнения `strcmp` и `strcoll` со строкой `str2` совпадал.
size_t *strxfrm(char *str1, const char *str2, size_t n);

// Вычисляет длину строки `str`.
size_t strlen(const char *str);

// Сравнивает строки `str1` и `str2` и возвращает 1, если они совпадают, иначе 0.
int strcmp(const char *str1, const char *str2);

// Сравнивает `n` символов строк `str1` и `str2` и возвращает 1, если они совпадают,
// иначе 0.
int strncmp(const char *str1, const char *str2, size_t n);

// Сравнивает две строки в соответствии с текущей локалью (языком типа).
int strcoll(const char *str1, const char *str2);

// Ищет первое вхождение символа `ch` в строку `src`.
// Возвращает указатель на `ch` в `src`, если `ch` в ней есть, иначе возвращает нулевой указатель.
char *strchr(char *src, int ch);

// Ищет последнее вхождение символа `ch` в строку `src`.
// Возвращает указатель на `ch` в `src`, если `ch` в ней есть, иначе возвращает нулевой указатель.
char *strrchr(char *src, int ch);

// Ищет длину максимального сегмента в строке `str1`, состоящего только из символов строки `str2`.
size_t strspn(const char *str1, const char *str2);

// Ищет длину максимального сегмента в строке `str1`, состоящего из любых символов, кроме строки `str2`.
size_t strcspn(const char *str1, const char *str2);

// Ищет первый символ в строке `str`, входящий в `seps`.
char *strpbrk(char *str, char *seps);

// FIXME: Вообще говоря, должна быть ещё версия с const char * для strpbrk и strstr, но
// так как нельзя перегружать функции в C, у меня её сделать не выходит.
// При этом в glibc она работает без проблем.

// Ищет первый символ в строке `str`, входящий в `seps`.
const char *strpbrk(const char *str, const char *seps);

// Ищет первое вхождение строки `str2` в строку `str1`.
char *strstr(char *str1, char *str2);

// Ищет первое вхождение строки `str2` в строку `str1`.
const char *strstr(const char *str1, const char *str2);

// Токенизирует строку `str` в соответствии с токенами в `delim`.
char *strtok(char *str, char *delim);

// Ищет символ `ch` в буфере `buf` длиной `n` Б.
void *memchr(void *buf, int ch, size_t n);

// Сравнивает буферы `buf1` и `buf2` длиной `n` Б и возвращает 1, если они совпадают.
int memcmp(const void *buf1, const void *buf2, size_t n);

// Изменяет `count` символов в `dest` на `ch`.
void *memset(void *dest, int ch, size_t count);

// Копирует `count` символов с `src` на `dest`.
void *memcpy(void *dest, const void *src, size_t count);

// Перемещает буфер `src` длиной `count` Б в буфер `dest`.
void *memmove(void *dest, const void *src, size_t count);

// Возвращает строковое описание ошибки `err`.
char* strerror(int err);

// Следующие функции есть только в C11 и при наличии какого-то EXT1.
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L && \
    defined(__STDC_LIB_EXT1__) && defined(__STDC_LIB_WANT_EXT1__)

// Копирует строку `src` в строку `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t strcpy_s(char *restrict dest, rsize_t destSz, const char *restrict src);
    
// Копирует `count` символов из строки `src` в строку `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t strncpy_s(char *restrict dest, rsize_t destSz, const char *restrict src, rsize_t count);

// Присоединяет строку `src` к строке `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t strcat_s(char *restrict dest, rsize_t destSz, const char *restrict src);

// Присоединяет `count` символов строки `src` на конец строки `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t strncat_s(char *restrict dest, rsize_t destSz, const char *restrict src, rsize_t count);

// Вычисляет длину строки `str`, проверяя не больше `maxSz` символов.
size_t strnlen_s(const char *str, size_t maxSz);

// Токенизирует строку `str` в соответствии с разделителями `delim`.
char* strtok_s(char *restrict str, rsize_t *restrict strmax, const char *restrict delim, char **restrict state);

// Изменяет `count` символов в буфере `dest` на `c`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t memset_s(void *dest, rsize_t destSz, int c, rsize_t count);

// Копирует буфер `src` длиной `count` символов в `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t memcpy_s(void *restrict dest, rsize_t destSz, const void *restrict src, rsize_t count);

// Копирует буфер `src` длиной `count` символов сначала во временный буфер, а затем
// в буфер `dest`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t memmove_s(void *restrict dest, rsize_t destSz, const void *src, rsize_t count);

// Ищет строковое описание ошибки `err` и помещает его в `buf`.
// Может обработать ошибки во время работы, возвращает ненулевое значение при ошибке.
errno_t strerror_s(char *buf, rsize_t bufsz, errno_t err);

// Вычисляет длину строкового описания ошибки `err`.
size_t strerrorlen_s(errno_t err);

#endif

// Следующие функции определены после C23.
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202300L

// Клонирует строку `str` и возвращает указатель на неё.
char *strdup(const char *str);

// Клонирует `count` символов строки `str` и возвращает указатель на результат.
char *strndup(const char *str, size_t count);

// Копирует `count` символов из буфера `src` в буфер `dest`.
// Прерывается, если наткнётся на символ `c`.
void *memccpy(void *restrict dest, const void *restrict src, int c, size_t count);

// Изменяет `count` символов в буфере `dest` на `c`.
void *memset_explicit(void *dest, int c, size_t count);

#endif

END_DECLS

#endif