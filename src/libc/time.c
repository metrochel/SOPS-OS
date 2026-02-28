/*
 * time.c
 *
 *
 */

#include "include/time.h"
#include "etc/syscalls.h"
#include "include/limits.h"
#include "locales/ext_lconv.h"
#include "include/stdio.h"
#include "include/etc/timezone.h"

/* Макросы и постоянные */

#define YEAR_SECS   31536000
#define DAY_SECS    86400
#define HOUR_SECS   3600
#define MIN_SECS    60

const int month_days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

timezone_t timezone;

/* Вспомогательные функции */

#define min(a, b) ((a) < (b) ? (a) : (b))

#define is_leap_year(year) (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 400 == 0)

int compute_weekday(int year, int month, int day) {
    int calc1 = (year % 100) / 12;
    int calc2 = (year % 100) % 12;
    int calc3 = calc2 / 4;

    int anchor;
    while (year < 1800) {
        year += 400;
    }
    while (year >= 2200) {
        year -= 400;
    }
    switch (year / 100) {
        case 18: anchor = 5; break;
        case 19: anchor = 3; break;
        case 20: anchor = 2; break;
        case 21: anchor = 6; break;
        default: return -1;
    }

    int doomsday = (calc1 + calc2 + calc3 + anchor) % 7;

    int doomsdate, weekday;
    switch (month) {
        case 1:
            if (is_leap_year(year)) doomsdate = 4;
            else doomsdate = 3;
            break;
        case 2:
            if (is_leap_year(year)) doomsdate = 29;
            else doomsdate = 28;
            break;
        case 3: doomsdate = 7; break;
        case 4: doomsdate = 4; break;
        case 5: doomsdate = 9; break;
        case 6: doomsdate = 6; break;
        case 7: doomsdate = 11; break;
        case 8: doomsdate = 8; break;
        case 9: doomsdate = 5; break;
        case 10: doomsdate = 10; break;
        case 11: doomsdate = 7; break;
        case 12: doomsdate = 12; break;
        default: return -1;
    }

    if (doomsdate == day)
        return doomsday;

    while (doomsdate - day >= 7)
        day += 7;
    while (day - doomsdate >= 7)
        day -= 7;

    weekday = doomsday + day - doomsdate;
    if (weekday < 0) weekday += 7;
    return weekday;
}

int compute_yearday(const struct tm *time) {
    int day = time->tm_mday;
    if (is_leap_year(time->tm_year + 1900) && time->tm_mon >= 2)
        day++;
    switch (time->tm_mon) {
        case 0: return day;
        case 1: return 31 + day;
        case 2: return 59 + day;
        case 3: return 90 + day;
        case 4: return 120 + day;
        case 5: return 151 + day;
        case 6: return 181 + day;
        case 7: return 212 + day;
        case 8: return 243 + day;
        case 9: return 273 + day;
        case 10: return 304 + day;
        case 11: return 334 + day;
    }
}

int get_iso8601_year(const struct tm *time) {
    int year = time->tm_year + 1900;
    int month = time->tm_mon;
    int day = time->tm_mday;
    if (month > 1 && month < 12)
        return year;
    if ((day > 7 && day < 29) || (month == 1 && day >= 4))
        return year;

    int weekday4 = compute_weekday(year, 1, 4);     // 4 января
    if (weekday4 == 0) weekday4 = 7;
    int weekday = time->tm_wday;
    if (weekday == 0) weekday = 7;
    if (weekday < weekday4)
        return year - 1;
    else
        return year;
}

/* Функции */

char __asctime_str[64];
struct tm __localtime_buf, __ctime_buf;

clock_t clock() {
    clock_t time = syscall0(syscall_get_proc_time);
    return time;
}

time_t time(time_t *arg) {
    time_t time = syscall0(syscall_get_unix_time);
    if (arg) *arg = time;
    return time;
}

double difftime(time_t end, time_t start) {
    return (double)(end - start);
}

char *ctime(const time_t *time) {
    return asctime(localtime(time));
}

char *asctime(const struct tm *time) {
    const char *weekday, *month;
    switch (time->tm_wday) {
        case 0: weekday = "Sun"; break;
        case 1: weekday = "Mon"; break;
        case 2: weekday = "Tue"; break;
        case 3: weekday = "Wed"; break;
        case 4: weekday = "Thu"; break;
        case 5: weekday = "Fri"; break;
        case 6: weekday = "Sat"; break;
        default: weekday = "Inv"; break;
    }

    switch (time->tm_mon) {
        case 0:  month = "Jan"; break;
        case 1:  month = "Feb"; break;
        case 2:  month = "Mar"; break;
        case 3:  month = "Apr"; break;
        case 4:  month = "May"; break;
        case 5:  month = "Jun"; break;
        case 6:  month = "Jul"; break;
        case 7:  month = "Aug"; break;
        case 8:  month = "Sep"; break;
        case 9:  month = "Oct"; break;
        case 10: month = "Nov"; break;
        case 11: month = "Dec"; break;
        default: month = "Inv"; break;
    }

    int result = sprintf(__asctime_str, "%s %s %2d %.2d:%.2d:%.2d %4d\n",
            weekday, month, time->tm_mday,
            time->tm_hour, time->tm_min, time->tm_sec,
            time->tm_year + 1900);

    if (result < 0)
        return NULL;
    return __asctime_str;
}

/* gmtime, localtime */

#define cvt_ut_to_tm() \
    int year = 70;                                                          \
    time_t time = *timer;                                                   \
    while (time > YEAR_SECS) {                                              \
        if (is_leap_year(year))                                             \
            time -= DAY_SECS;                                               \
        time -= YEAR_SECS;                                                  \
        year++;                                                             \
    }                                                                       \
    int month = 0;                                                          \
    int year_day = 0;                                                       \
    while (time > month_days[month] * DAY_SECS && month < 12) {             \
        year_day += month_days[month];                                      \
        if (!is_leap_year(year) && month == 1)                              \
            year_day--;                                                     \
        month++;                                                            \
        time -= month_days[month] * DAY_SECS;                               \
    }                                                                       \
    year_day--;                                                             \
    int day = 1;                                                            \
    while (time > DAY_SECS && day <= month_days[month]) {                   \
        day++;                                                              \
        time -= DAY_SECS;                                                   \
    }                                                                       \
    year_day += day;                                                        \
    int weekday = compute_weekday(year + 1900, month + 1, day);             \
    int sec = time % MIN_SECS;                                              \
    time /= MIN_SECS;                                                       \
    int min = time % MIN_SECS;                                              \
    time /= MIN_SECS;                                                       \
    int hour = time % 24;


struct tm* localtime(const time_t *timer) {
    cvt_ut_to_tm()

    __localtime_buf.tm_year = year;
    __localtime_buf.tm_mon = month;
    __localtime_buf.tm_mday = day;
    __localtime_buf.tm_yday = year_day;
    __localtime_buf.tm_wday = weekday;
    __localtime_buf.tm_hour = hour + (timezone.utc_offset / 100);
    __localtime_buf.tm_min = min;
    __localtime_buf.tm_sec = sec;
    __localtime_buf.tm_isdst = 0;
    return &__localtime_buf;
}

struct tm* gmtime(const time_t *timer) {
    cvt_ut_to_tm()

    __ctime_buf.tm_year = year;
    __ctime_buf.tm_mon = month;
    __ctime_buf.tm_mday = day;
    __ctime_buf.tm_yday = year_day;
    __ctime_buf.tm_wday = weekday;
    __ctime_buf.tm_hour = hour;
    __ctime_buf.tm_min = min;
    __ctime_buf.tm_sec = sec;
    __ctime_buf.tm_isdst = 0;
    return &__ctime_buf;
}

/* mktime */

time_t mktime(struct tm *arg) {
    struct tm time = *arg;

    while (time.tm_sec < 0) {
        time.tm_sec += 60;
        time.tm_min ++;
    }
    while (time.tm_sec > 60) {
        time.tm_sec -= 60;
        time.tm_min --;
    }

    while (time.tm_min < 0) {
        time.tm_min += 60;
        time.tm_hour ++;
    }
    while (time.tm_min >= 60) {
        time.tm_min -= 60;
        time.tm_hour --;
    }

    while (time.tm_hour < 0) {
        time.tm_hour += 24;
        time.tm_mday ++;
    }
    while (time.tm_hour >= 24) {
        time.tm_hour -= 24;
        time.tm_mday --;
    }

    while (time.tm_mon < 0) {
        time.tm_mon += 12;
        time.tm_year ++;
    }
    while (time.tm_mon >= 12) {
        time.tm_mon -= 12;
        time.tm_year --;
    }

    while (time.tm_mday < 0) {
        int mdays = month_days[time.tm_mon];
        if (time.tm_mon == 1 && !is_leap_year(time.tm_year)) {
            mdays --;
        }
        time.tm_mday += mdays;
        time.tm_mon ++;
        if (time.tm_mon >= 12) {
            time.tm_mon = 0;
            time.tm_year ++;
        }
    }
    while (time.tm_mday > month_days[time.tm_mon]) {
        if (!is_leap_year(time.tm_year) && time.tm_mon == 1 && time.tm_mday == 28)
            break;
        int mdays = month_days[time.tm_mon];
        if (time.tm_mon == 1 && !is_leap_year(time.tm_year)) {
            mdays --;
        }
        time.tm_mday -= mdays;
        time.tm_mon --;
        if (time.tm_mon < 0) {
            time.tm_mon = 11;
            time.tm_year --;
        }
    }

    time.tm_yday = compute_yearday(&time);
    time.tm_wday = compute_weekday(time.tm_year + 1900, time.tm_mon, time.tm_mday);

    time_t ut_time = 0;

    int yr_offset = time.tm_year - 70;
    if (yr_offset < 0) {
        return -1;
    }

    for (int i = 0; i < yr_offset; i++) {
        int year = 1970 + i;
        if (is_leap_year(year))
            ut_time += DAY_SECS;
        ut_time += YEAR_SECS;
    }

    ut_time += time.tm_yday * DAY_SECS;
    ut_time += time.tm_hour * HOUR_SECS;
    ut_time += time.tm_min * MIN_SECS;
    ut_time += time.tm_sec;

    return ut_time;
}

/* timespec_getX */

int timespec_get(struct timespec *ts, int base) {
    time_t time_s = time(NULL) + base * HOUR_SECS;
    long time_ns = syscall0(syscall_get_nanosec_time);
    struct timespec spec = {.tv_nsec = time_ns, .tv_sec = time_s};
    *ts = spec;
    return base;
}

int timespec_getres(struct timespec *ts, int base) {
    struct timespec res = {.tv_sec = 0, .tv_nsec = 100};
    *ts = res;
    return 0;
}