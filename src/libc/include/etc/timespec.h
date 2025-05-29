//
//  timespec.h - Структура timespec
//
//  Определяет структуру timespec для использования в time.h и threads.h.
//

#ifndef _TIMESPEC
#define _TIMESPEC

// `timespec` - структура, содержащая время в секундах и время после него в наносекундах.
struct timespec {
    unsigned long tv_sec;
    unsigned long tv_nsec;
};

#endif