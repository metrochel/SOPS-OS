//
//	time.h - Библиотека с временем
//
//	Помогает программе контроллировать время.
//

#if !defined(_TIME_INCL)
#define _TIME_INCL 1

// Из stddef.h нам нужен size_t и NULL.
#define __need_NULL
#define __need_size_t
#include <stddef.h>

#include <etc/decl.h>
#include <etc/timespec.h>

BEGIN_DECLS

// `clock_t` - тип, способный описать количество тактов с момента запуска программы.
typedef unsigned long clock_t;

// `clock_t` - тип, способный описать время в формате UNIX Timestamp (число секунд после 1 января 1970 года, 00:00:00).
typedef unsigned long time_t;

typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} tm;

// Возвращает время в тактах процессора после запуска программы.
clock_t clock();

// Возвращает текущее время в формате UNIX Timestamp.
time_t time();

// Вычисляет разницу между временем `time1` и временем `time2`.
double difftime(time_t time1, time_t time2);

// Переводит время `time` в текстовый формат.
char *ctime(const time_t* time);

// Переводит время `time` в текстовый формат.
char *asctime(const tm* time);

// Переводит время `time` в текстовый формат в соответствии с `format`.
// Размер финальной строки не должен превышать `maxsize`.
char *strftime(char *s, size_t maxsize, const char *format, const tm *time);

// Переводит время `time` из UNIX Timestamp в `tm` по UTC.
tm *gmtime(const time_t* time);

// Переводит время `time` из UNIX Timestamp в `tm` по местному времени.
tm *localtime(const time_t* time);

// Переводит время `time` из календарного в UNIX Timestamp.
time_t mktime(tm *time);

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201100L

// Переводит время часового пояса `base` в формат `timespec` и записывает его в `ts`.
int timespec_get(struct timespec *ts, int base);

#if __STDC_VERSION__ >= 202300L

// Изменяет `ts` на разрешение времени для часового пояса `base`.
int timespec_getres(struct timespec *ts, int base);

#endif

#endif

END_DECLS

// Число тактов процессора в секунду.
// Вообще должно вычисляться и получаться от ОС, но 
// POSIX определяет этот макрос как 1 000 000 вне зависимости
// от процессора.
#define CLOCKS_PER_SEC ((clock_t)1000000)

#endif