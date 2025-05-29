//
//  uchar.h - Работа с многобайтовыми строками
//
//  Переводит многобайтовые строки в однобайтовые строки и обратно
//

#if !defined _UCHAR_INCL && defined __STDC_VERSION__ && __STDC_VERSION__ >= 201100L
#include <etc/decl.h>

#define __need_size_t
#include <stddef.h>

BEGIN_DECLS

// `mbstate_t` - это структура, описывающая состояние обработки многобайтового символа.
typedef struct {
    wint_t symbol;          // Символ
    short shift;            // Сдвиг
} mbstate_t;

// `char16_t` - это тип, описывающий один 16-битный символ.
typedef unsigned short char16_t;

// `char32_t` - это тип, описывающий один 32-битный символ.
typedef unsigned int char32_t;

// Переводит узкую многобайтовую строку `str` в строку UTF-16.
size_t mbrtoc16(char8_t* restrict pc16, const char* restrict str, size_t count, mbstate_t* restrict state);

// Переводит UTF-16-символ `c16` в узкую многобайтовую кодировку и записывает его в `s`.
size_t c16rtomb(char* restrict s, char16_t c16, mbstate_t* restrict state);

// Переводит узкую многобaайтовую строку `str` в строку UTF-32 `pc32`.
size_t mbrtoc32(char32_t* restrict pc32, const char* restrict str, size_t count, mbstate_t* restrict state);

// Переводит UTF-32-символ `c32` в узкую многобайтовую кодировку и записывает его в `s`.
size_t c32rtomb(char* restrict s, char32_t c32, mbstate_t* restrict state);

#if __STDC_VERSION__ >= 202300L

// `char8_t` - это тип, описывающий один 8-битный символ.
typedef signed char char8_t;

// Переводит узкую многобайтовую строку `str` в строку UTF-8.
size_t mbrtoc8(char8_t* restrict pc8, const char* restrict str, size_t count, mbstate_t* restrict state);

// Переводит UTF-8-символ `c8` в узкую многобайтовую кодировку и записывает его в `s`.
size_t c8rtomb(char* restrict s, char8_t c8, mbstate_t* restrict state);

#endif

END_DECLS

#endif