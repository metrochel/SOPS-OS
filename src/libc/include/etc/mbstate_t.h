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
} mbstate_t;
#endif