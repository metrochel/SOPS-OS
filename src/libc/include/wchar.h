//
//  wchar.h - Работа с широкосимвольными строками
//
//  Позволяет манипулировать строки с широкими символами.
//

#if !defined _WCHAR_INCL && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199500L
#define _WCHAR_INCL 1

#include <etc/decl.h>

BEGIN_DECLS

// Из stddef.h нам нужен wchar_t и size_t.
#define __need_wchar_t
#define __need_size_t
#include <stddef.h>

// Символ, указывающий на ошибку
#undef WEOF
#define WEOF (wint_t)(-1)
// Наименьшее возможное значение для `wchar_t`
#define WCHAR_MIN 0
// Наибольшее возможное значение для `wchar_t`
#define WCHAR_MAX __WCHAR_MAX__

#include <etc/wint_t.h>

// Переводит широкосимвольную строку `str` в число по основанию `base`.
// По окончании перевода устанавливает `strEnd` на последний символ.
long wcstol(const wchar_t *str, wchar_t **strEnd, int base);

// Переводит широкосимвольную строку `str` в число по основанию `base`.
// По окончании перевода устанавливает `strEnd` на последний символ.
unsigned long wcstoul(const wchar_t *str, wchar_t **strEnd, int base);

// Переводит широкосимвольную строку `str` в число с плавающей точкой.
// По окончании перевода устанавливает `strEnd` на последний символ.
double wcstod(const wchar_t *str, wchar_t **strEnd);

// Копирует широкосимвольную строку `src` в широкосимвольную строку `dest`.
wchar_t* wcscpy(wchar_t *dest, wchar_t *src);

// Копирует `count` широких символов с `src` на `dest`.
wchar_t* wcsncpy(wchar_t *dest, wchar_t *src, size_t count);

// Записывает широкосимвольную строку `src` на конец строки `dest`.
wchar_t* wcscat(wchar_t *dest, const wchar_t *src);

// Копирует `count` широких символов с `src` на конец `dest`.
wchar_t* wcsncat(wchar_t *dest, const wchar_t *src, size_t count);

// Преобразовывает `count` широких символов строки `src` и записывает их в `dest` 
// так, что результаты `strcmp` и `strcoll` совпадали.
size_t wcsxfrm(wchar_t *dest, const wchar_t *src, size_t count);

// Вычисляет длину широкосимвольной строки `str`.
size_t wcslen(const wchar_t *str);

// Сравнивает широкосимвольные строки `str1` и `str2`.
int wcscmp(const wchar_t *str1, const wchar_t *str2);

// Сравнивает `count` широких символов строк `str1` и `str2`.
int wcsncmp(const wchar_t *str1, const wchar_t *str2, size_t count);

// Сравнивает широкосимвольные строки `str1` и `str2` в соответствии
// с текущей локалью.
int wcscoll(const wchar_t *str1, const wchar_t *str2);

// Ищет первое вхождение широкого символа `c` в строку `str`.
wchar_t* wcschr(const wchar_t *str, wchar_t c);

// Ищет первое справа вхождение широкого символа `c` в строку `str`.
wchar_t* wcsrchr(const wchar_t *str, wchar_t c);

// Ищет фрагмент от начала широкосимвольной строки `str` наибольшей длины,
// состоящий только из символов, входящих в `chars`. 
wchar_t* wcsspn(const wchar_t *str, const wchar_t *chars);

// Ищет фрагмент от начала широкосимвольной строки `str` наибольшей длины,
// состоящий только из символов, не входящих в `chars`. 
size_t wcscspn(const wchar_t *str, const wchar_t *chars);

// Возвращает указатель на первый символ в широкосимвольной строке `str`, 
// входящий также в `chars`. 
wchar_t* wcspbrk(const wchar_t *chars, const wchar_t *str);

// Ищет первое вхождение широкосимвольной строки `str` в строку `dest`.
wchar_t* wcsstr(const wchar_t *dest, const wchar_t *str);

// Токенизирует широкосимвольную строку `str` в соответствии с делителями `stops`.
wchar_t* wcstok(wchar_t *str, const wchar_t *stops, wchar_t **str_end);

// Копирует `count` широких символов с `src` в `dest`.
wchar_t* wmemcpy(wchar_t *dest, const wchar_t *src, size_t count);

// Перемещает `count` широких символов с `src` на `dest`.
wchar_t* wmemmove(wchar_t *dest, const wchar_t *src, size_t count);

// Сравнивает `count` широких символов в буферах `str1` и `str2`.
int wmemcmp(const wchar_t *str1, const wchar_t *str2, size_t count);

// Ищет широкий символ в буфере `buf` длиной `count`.
wchar_t* wmemchr(const wchar_t *buf, wchar_t c, size_t count);

// Заменяет `count` символов в буфере `dest` на `c`.
wchar_t* wmemset(wchar_t *dest, wchar_t c, size_t count);

END_DECLS

#endif