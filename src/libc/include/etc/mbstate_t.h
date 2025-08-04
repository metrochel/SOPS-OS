//
//  mbstate_t.h - mbstate_t
//
//  Определяет тип mbstate_t.
//

#ifndef _MBSTATE_T
#define _MBSTATE_T
#include <etc/wint_t.h>
// `mbstate_t` - это структура, описывающая состояние обработки многобайтового символа.
typedef struct {
    wint_t symbol;          // Символ
    short shift;            // Сдвиг
    int utf16_is_surrogate;    // Флаг суррогатной пары (UTF-16)
    int utf8_followers;     // Количество оставшихся байтов после первого (UTF-8)
} mbstate_t;
#endif