/*
 *  time_c_strftime.c
 *
 *  Функция strftime
 *
 */

#include "../include/time.h"
#include "../include/etc/timezone.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../locales/ext_lconv.h"

extern const int month_days[];

extern int get_iso8601_year(const struct tm *time);

#define is_leap_year(year) (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 400 == 0)

#define min(a, b) ((a) < (b) ? (a) : (b))

#define handle_func(name) \
    inline                                                                 \
    int handle_##name##_spec(char **str, size_t *chars_count, size_t max,  \
    int zero_flag, int e_flag, const struct tm *time)

#define put_alt(var_name)                               \
    len = locale.dates.conv_to_alt(buf, var_name);      \
    if (len > max) return -1;                           \
    char *ptr = buf;                                    \
    while (*ptr && *chars_count < max) {                \
        *(*str)++ = *ptr++;                             \
        (*chars_count)++;                               \
    }                                                   \

#define put_var(var_name)                               \
    char *ptr = buf;                                    \
    len = 0;                                            \
    while (year) {                                      \
        *ptr++ = '0' + (var_name % 10);                 \
        len++;                                          \
        var_name /= 10;                                 \
    }                                                   \
    for (size_t i = 0; i < len; i++) {                  \
        if (*chars_count >= max) break;                 \
        *(*str)++ = *ptr--;                             \
        (*chars_count)++;                               \
    }

#define put_fixed(var_name, var_size)                   \
    for (int i = 0; i < var_size; i++) {                \
        buf[i] = '0' + (var_name % 10);                 \
        var_name /= 10;                                 \
    }                                                   \
    len = var_size;                                     \
    char *ptr = buf + var_size - 1;                     \
    for (size_t i = 0; i < len; i++) {                  \
        if (*chars_count >= max) break;                 \
        *(*str)++ = *ptr--;                             \
        (*chars_count)++;                               \
    }

#define put_w_alt_var(flag, var_name)                   \
    char buf[16];                                       \
    size_t len;                                         \
    if (flag && locale.dates.has_alt_digits) {          \
        put_alt(var_name)                               \
    } else {                                            \
        put_var(var_name)                               \
    }                                                   \
    return 0;


#define put_w_alt_fixed(flag, var_name, var_size)       \
    char buf[16];                                       \
    size_t len;                                         \
    if (flag && locale.dates.has_alt_digits) {          \
        put_alt(var_name)                               \
    } else {                                            \
        put_fixed(var_name, var_size)                   \
    }                                                   \
    return 0;

#define strftime_strcpy(formstr) \
    strncpy(*str, formstr, max - *chars_count);     \
    size_t add = strlen(formstr);                   \
    *chars_count = min(max, *chars_count + add);    \
    if (*chars_count == max) return 0;              \
    *str += add;                                    \
    return 0;

#define strftime_rec_str(formstr) \
    size_t add = strftime(*str, max - *chars_count, formstr, time); \
    if (add == 0)                                                   \
        return -1;                                                  \
    *str += add;                                                    \
    *chars_count += add;                                            \
    return 0;

handle_func(full_year) {
    int year = time->tm_year;

    put_w_alt_var(e_flag, year)
}

handle_func(shrt_year) {
    int year = time->tm_year % 100;

    put_w_alt_fixed(zero_flag, year, 2)

}

handle_func(misc_year) {
    char buf[16];
    size_t len;
    if (e_flag && locale.dates.has_alt_digits) {
        strftime_strcpy(locale.dates.alt_base_year)
    } else {
        int year = time->tm_year / 100;
        if (year >= 100)
            return -1;
        put_fixed(year, 2)
    }
    return 0;
}

handle_func(iso8601_full_year) {
    int year = get_iso8601_year(time);
    char buf[16];
    size_t len;
    put_var(year)
    return 0;
}

handle_func(iso8601_shrt_year) {
    int year = get_iso8601_year(time) % 100;
    char buf[16];
    size_t len;
    put_fixed(year, 2)
    return 0;
}

handle_func(abbr_month) {
    int month = time->tm_mon;

    if (month < 0 || month >= 12)
        return -1;

    const char *month_str;
    if (e_flag && locale.dates.has_alt_months)
        month_str = locale.dates.alt_abbr_months[month];
    else
        month_str = locale.dates.abbr_months[month];

    strftime_strcpy(month_str)
}

handle_func(full_month) {
    int month = time->tm_mon;

    if (month < 0 || month >= 12)
        return -1;

    const char *month_str;
    if (e_flag && locale.dates.has_alt_months)
        month_str = locale.dates.alt_full_months[month];
    else
        month_str = locale.dates.full_months[month];

    strftime_strcpy(month_str)
}

handle_func(num_month) {
    int month = time->tm_mon;

    put_w_alt_fixed(zero_flag, month, 2)
}

handle_func(yrweek_mon) {
    int year_day = time->tm_yday;
    int week_no = (year_day / 7) + 1;

    put_w_alt_fixed(zero_flag, week_no, 2)
}

handle_func(yrweek_sun) {
    int year_day = time->tm_yday;
    int week_no = (year_day / 7);
    if (time->tm_wday == 0)
        week_no ++;
    if (week_no >= 54)
        week_no = 0;

    put_w_alt_fixed(zero_flag, week_no, 2)
}

handle_func(iso8601_week) {
    int year = time->tm_year;
    int iso8601_year = get_iso8601_year(time);
    int week_no;

    if (iso8601_year != year)
        week_no = 53;
    else {
        int year_day = time->tm_yday;
        week_no = (year_day / 7) + 1;
    }

    put_w_alt_fixed(zero_flag, week_no, 2)
}

handle_func(year_day) {
    int year_day = time->tm_yday;

    if (year_day > 366 || (!is_leap_year(time->tm_year + 1900) && year_day > 365))
        return -1;

    char buf[3];
    size_t len;
    put_fixed(year_day, 3)
    return 0;
}

handle_func(month_day) {
    int month_day = time->tm_mday;
    int month = time->tm_mon;

    if (month >= 12 || month < 0 || month_day > month_days[month] || month_day < 0)
        return -1;
    if (month == 1 && !is_leap_year(time->tm_year + 1900) && month_day > 28)
        return -1;

    char buf[16];
    size_t len;
    if (zero_flag && locale.dates.has_alt_digits) {
        month_day--;
        put_alt(month_day)
    } else {
        put_fixed(month_day, 2)
    }
    return 0;
}

handle_func(sing_day) {
    int month_day = time->tm_mday;
    int month = time->tm_mon;

    if (month >= 12 || month < 0 || month_day > month_days[month] || month_day < 0)
        return -1;

    char buf[16];
    size_t len;
    if (zero_flag && locale.dates.has_alt_digits) {
        put_alt(month_day)
        if (len == 1 && *chars_count < max) {
            buf[1] = buf[0];
            buf[0] = ' ';
            (*str)--;
            (*chars_count)--;
            for (int i = 0; i < 2; i++) {
                if (*chars_count >= max) break;
                *(*str)++ = buf[i];
                (*chars_count)++;
            }
        }
    } else {
        buf[0] = month_day >= 10 ? '0' + month_day / 10 : ' ';
        buf[1] = '0' + month % 10;
        for (int i = 0; i < 2; i++) {
            if (*chars_count >= max) break;
            *(*str)++ = buf[i];
            (*chars_count)++;
        }
    }

    return 0;
}

handle_func(abbr_weekday) {
    int weekday = time->tm_wday;
    if (weekday < 0 || weekday > 6)
        return -1;

    weekday--;
    if (weekday < 0) weekday = 6;

    const char *wday_str = locale.dates.abbr_weekdays[weekday];
    strftime_strcpy(wday_str)
}

handle_func(full_weekday) {
    int weekday = time->tm_wday;
    if (weekday < 0 || weekday > 6)
        return -1;

    weekday--;
    if (weekday < 0) weekday = 6;

    const char *wday_str = locale.dates.full_weekdays[weekday];
    strftime_strcpy(wday_str)
}

handle_func(num_weekday_sun) {
    int weekday = time->tm_wday;
    if (weekday < 0 || weekday > 6)
        return -1;

    char buf[16];
    size_t len;
    if (zero_flag && locale.dates.has_alt_digits) {
        put_alt(weekday)
    } else if (*chars_count < max) {
        *(*str)++ = '0' + weekday;
        (*chars_count)++;
    }

    return 0;
}

handle_func(num_weekday_mon) {
    int weekday = time->tm_wday;
    if (weekday < 0 || weekday > 6)
        return -1;

    if (weekday == 0) weekday = 7;

    char buf[16];
    size_t len;
    if (zero_flag && locale.dates.has_alt_digits) {
        put_alt(weekday)
    } else if (*chars_count < max) {
        *(*str)++ = '0' + weekday;
        (*chars_count)++;
    }

    return 0;
}

handle_func(hour) {
    int hour = time->tm_hour;
    if (hour < 0 || hour >= 24)
        return -1;

    put_w_alt_fixed(zero_flag, hour, 2)
}

handle_func(hour12) {
    int hour = time->tm_hour;
    if (hour < 0 || hour >= 24)
        return -1;

    if (hour > 12)
        hour -= 12;
    if (hour == 0)
        hour = 12;

    put_w_alt_fixed(zero_flag, hour, 2)
}

handle_func(min) {
    int minute = time->tm_min;
    if (minute < 0 || minute >= 60)
        return -1;

    put_w_alt_fixed(zero_flag, minute, 2)
}

handle_func(sec) {
    int second = time->tm_sec;
    if (second < 0 || second > 60)
        return -1;

    put_w_alt_fixed(zero_flag, second, 2)
}

handle_func(std_datestr) {
    const char *stdstr = locale.dates.std_datestr;
    if (e_flag && locale.dates.alt_std_datestr)
        stdstr = locale.dates.alt_std_datestr;

    strftime_rec_str(stdstr)
}

handle_func(loc_datestr) {
    const char *locstr = locale.dates.loc_datestr;
    if (!locstr)
        locstr = locale.dates.std_datestr;
    if (e_flag && locale.dates.alt_loc_datestr)
        locstr = locale.dates.alt_loc_datestr;

    strftime_rec_str(locstr)
}

handle_func(loc_timestr) {
    const char *locstr = locale.dates.loc_timestr;
    if (!locstr)
        locstr = "%H:%M:%S";
    if (e_flag && locale.dates.alt_loc_timestr)
        locstr = locale.dates.alt_loc_timestr;

    strftime_rec_str(locstr)
}

handle_func(date) {
    strftime_rec_str("%m/%d/%y")
}

handle_func(iso8601_date) {
    strftime_rec_str("%Y-%m-%d")
}

handle_func(12hr_time) {
    const char *timestr = locale.dates.loc_timestr12;
    if (!timestr)
        timestr = "%I:%M:%S %p";

    strftime_rec_str(timestr)
}

handle_func(hm_time) {
    strftime_rec_str("%H:%M")
}

handle_func(hms_time) {
    strftime_rec_str("%H:%M:%S")
}

handle_func(am_pm) {
    int is_pm = time->tm_hour >= 12;
    const char *am_pm_str;
    if (is_pm) {
        am_pm_str = locale.dates.pm_str;
        if (!am_pm_str)
            am_pm_str = "p.m.";
    } else {
        am_pm_str = locale.dates.am_str;
        if (!am_pm_str)
            am_pm_str = "a.m.";
    }

    strftime_rec_str(am_pm_str)
}

handle_func(tz_off) {
    if (!timezone.name)
        return 0;
    size_t add = snprintf(*str, max - *chars_count, "%+4d", timezone.utc_offset);
    *chars_count = min(max, *chars_count + add);
    if (*chars_count == max) return 0;
    *str += add;
    return 0;
}

handle_func(tz_name) {
    if (!timezone.name) {
        return 0;
    }
    strftime_strcpy(timezone.name)
}

#define handle_macro(type, func) \
    case type: result = func(&str, &chars_count, count, zero_flag, e_flag, time); break;

size_t strftime(char *str, size_t count, const char *format, const struct tm *time) {
    int zero_flag, e_flag;
    size_t chars_count = 0;
    if (!count)
        count = (size_t)-1;
    while (*format && chars_count < count) {
        zero_flag = 0;
        e_flag = 0;
        char c = *format++;
        if (c != '%') {
            *str++ = c;
            chars_count++;
            continue;
        }

        c = *format++;
        if (c == '%') {
            *str++ = '%';
            chars_count++;
            continue;
        }
        if (c == 'n') {
            *str++ = '\n';
            chars_count++;
            continue;
        }
        if (c == 't') {
            *str++ = '\t';
            chars_count++;
            continue;
        }

        if (c == '0') {
            str++;
            zero_flag = 1;
        }
        if (c == 'E') {
            str++;
            e_flag = 1;
        }

        int type = *format++;
        int result;
        switch (type) {
            handle_macro('Y', handle_full_year_spec)
            handle_macro('y', handle_shrt_year_spec)
            handle_macro('C', handle_misc_year_spec)
            handle_macro('G', handle_iso8601_full_year_spec)
            handle_macro('g', handle_iso8601_shrt_year_spec)

            handle_macro('b', handle_abbr_month_spec)
            handle_macro('h', handle_abbr_month_spec)
            handle_macro('B', handle_full_month_spec)
            handle_macro('m', handle_num_month_spec)

            handle_macro('U', handle_yrweek_sun_spec)
            handle_macro('W', handle_yrweek_mon_spec)
            handle_macro('V', handle_iso8601_week_spec)

            handle_macro('j', handle_year_day_spec)
            handle_macro('d', handle_month_day_spec)
            handle_macro('e', handle_sing_day_spec)

            handle_macro('a', handle_abbr_weekday_spec)
            handle_macro('A', handle_full_weekday_spec)
            handle_macro('w', handle_num_weekday_sun_spec)
            handle_macro('u', handle_num_weekday_mon_spec)

            handle_macro('H', handle_hour_spec)
            handle_macro('I', handle_hour12_spec)
            handle_macro('M', handle_min_spec)
            handle_macro('S', handle_sec_spec)

            handle_macro('c', handle_std_datestr_spec)
            handle_macro('x', handle_loc_datestr_spec)
            handle_macro('X', handle_loc_timestr_spec)
            handle_macro('D', handle_date_spec)
            handle_macro('F', handle_iso8601_date_spec)
            handle_macro('r', handle_12hr_time_spec)
            handle_macro('R', handle_hm_time_spec)
            handle_macro('T', handle_hms_time_spec)
            handle_macro('p', handle_am_pm_spec)
            handle_macro('z', handle_tz_off_spec)
            handle_macro('Z', handle_tz_name_spec)

            default: return -1;
        }

        if (result < 0) {
            if (chars_count == count && *format) {
                *(--str) = 0;
                return 0;
            }
            return -1;
        }
    }
    if (chars_count == count && *format) {
        *(--str) = 0;
        return 0;
    }
    return chars_count;
}