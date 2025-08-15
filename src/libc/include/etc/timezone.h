//
//  timezone.h - Часовые пояса
//
//  Управляет часовым поясом программы.
//

#ifndef _TIMEZONE_INCL
#define _TIMEZONE_INCL

// `timezone_t` - это тип, описывающий текущий часовой пояс.
typedef struct {
    const char *name;
    const char *localized_name;
    int utc_offset;
} timezone_t;

extern timezone_t timezone;

#endif
