/*
 * stdio_c_xscanf.c
 *
 * Функции *scanf
 *
 */

#include "../include/stdio.h"
#include "../include/ctype.h"
#include "../include/stdlib.h"
#include "../include/math.h"
#include "../include/stddef.h"
#include "../include/wchar.h"
#include "../include/limits.h"
#include <stdint.h>

#define concat(a,b) a##b
#define concat3(a,b,c) a##b##c

// Макросо-поносо-метр™ показывает 999 999 999% поноса!

/* === Типы ===  */

#define __scanf_state_t(scanf) concat(scanf, _state_t)
#define __scanf_args_t(scanf) concat(scanf, _args_t)
#define __scanf_params_t(scanf) concat(scanf, _params_t)

#define scanf_state_t __scanf_state_t(SCANF)
#define scanf_args_t __scanf_args_t(SCANF)
#define scanf_params_t __scanf_params_t(SCANF)

#ifndef _SCANF_DECL
#define _SCANF_DECL

typedef enum {
    spec_hh,
    spec_h,
    spec_none,
    spec_l,
    spec_ll,
    spec_j,
    spec_z,
    spec_t,
    spec_L
} size_spec_t;

#endif

typedef struct {
    CHAR fmt_spec;
    short flags;
    int max_width;
    CHAR size_spec;
} scanf_state_t;

typedef struct {
    const CHAR *format;
    void *buffer;
    size_t chars_count;
    va_list args;
    scanf_state_t state;
} scanf_args_t;

#define __get_func_t(scanf) concat(scanf, _get_func_t)
#define __consume_func_t(scanf) concat(scanf, _consume_func_t)
#define __unget_func_t(scanf) concat(scanf, _unget_func_t)

#define get_func_t __get_func_t(SCANF)
#define consume_func_t __consume_func_t(SCANF)
#define unget_func_t __unget_func_t(SCANF)

typedef int (*get_func_t)(scanf_args_t*);

typedef int (*consume_func_t)(scanf_args_t*);

typedef int (*unget_func_t)(scanf_args_t*, int);

typedef struct {
    get_func_t get;
    consume_func_t consume_spaces;
    unget_func_t unget;
} scanf_params_t;

/* === Параметры для видов scanf === */

/* sscanf */

#define __sscanf_get(scanf) concat3(s, scanf, _get)
#define __sscanf_consume(scanf) concat3(s, scanf, _consume_spaces)
#define __sscanf_unget(scanf) concat3(s, scanf, _unget)
#define __SSCANF(scanf) concat(params_s, scanf)

#define sscanf_get __sscanf_get(SCANF)
#define sscanf_consume __sscanf_consume(SCANF)
#define sscanf_unget __sscanf_unget(SCANF)
#define SSCANF __SSCANF(SCANF)

int sscanf_get(scanf_args_t *scanf_args) {
    const CHAR *ptr = (const CHAR*)(scanf_args->buffer);

    int c = *ptr++;
    scanf_args->chars_count++;
    while (ISSPACE(*ptr)) {
        ptr++;
        (scanf_args->chars_count)++;
    }
    scanf_args->buffer = (void*)ptr;

    return c;
}

int sscanf_consume(scanf_args_t *scanf_args) {
    const CHAR *ptr = (const CHAR*)(scanf_args->buffer);
    
    while (ISSPACE(*ptr)) {
        ptr++;
        (scanf_args->chars_count)++;
    }
    scanf_args->buffer = (void*)ptr;

    return 0;
}

int sscanf_unget(scanf_args_t *scanf_args, int c) {
    const CHAR *ptr = (const CHAR*)(scanf_args->buffer);

    ptr--;
    scanf_args->buffer = (void*)ptr;
    (scanf_args->chars_count)--;

    return 0;
}

const scanf_params_t SSCANF = {sscanf_get, sscanf_consume, sscanf_unget};

/* fscanf */

#define __fscanf_get(scanf) concat3(f, scanf, _get)
#define __fscanf_consume(scanf) concat3(f, scanf, _consume_spaces)
#define __fscanf_unget(scanf) concat3(f, scanf, _unget)
#define __FSCANF(scanf) concat(params_f, scanf)

#define fscanf_get __fscanf_get(SCANF)
#define fscanf_consume __fscanf_consume(SCANF)
#define fscanf_unget __fscanf_unget(SCANF)
#define FSCANF __FSCANF(SCANF)

int fscanf_get(scanf_args_t *scanf_args) {
    FILE *file = (FILE*)(scanf_args->buffer);

    int c = fgetc(file);
    (scanf_args->chars_count)++;
    int _c = fgetc(file);
    (scanf_args->chars_count)++;
    while (ISSPACE(_c) && !feof(file)) {
        _c = fgetc(file);
        (scanf_args->chars_count)++;
    }
    ungetc(_c, file);
    (scanf_args->chars_count)--;

    return c;
}

int fscanf_consume(scanf_args_t *scanf_args) {
    FILE *file = (FILE*)(scanf_args->buffer);

    int c = fgetc(file);
    (scanf_args->chars_count)++;
    while (ISSPACE(c) && !feof(file)) {
        c = fgetc(file);
        (scanf_args->chars_count)++;
    }
    ungetc(c, file);
    (scanf_args->chars_count)--;

    return 0;
}

int fscanf_unget(scanf_args_t *scanf_args, int c) {
    FILE *file = (FILE*)(scanf_args->buffer);

    ungetc(c, file);
    (scanf_args->chars_count)--;

    return 0;
}

const scanf_params_t FSCANF = {fscanf_get, fscanf_consume, fscanf_unget};

/* === Вспомогательные макросы и функции === */

#define FMT_NO_WRITE    0b1

#define call_get params->get(scanf_args)
#define call_consume_spaces params->consume_spaces(scanf_args)
#define call_unget params->unget(scanf_args, c)

extern size_t get_utf8_char_len(const char *str);

#define write_arg(arg, val) if (!(scanf_args->state.flags & FMT_NO_WRITE)) arg = val;

#define __handle_decl(name, scanf) \
    inline int                     \
    concat3(handle_##name##_,scanf,_spec) (scanf_args_t *scanf_args, const scanf_params_t *params)

#define handle_decl(name) __handle_decl(name, SCANF)

#ifndef WCHAR

#define get_mchar                                   \
    char mchar = call_get;                          \
    write_arg(*ptr, mchar);                         \
    c = mchar;

#define get_wchar                                   \
    int c = call_get;                               \
    char _buf[] = {0, 0, 0, 0};                     \
    _buf[0] = c;                                    \
    size_t len = get_utf8_char_len(_buf);           \
    if (len == (size_t)-1) return -1;               \
    for (size_t _j = 1; _j < len; _j++) {           \
        c = call_get;                               \
        if (!c || c == EOF) return 1;               \
        _buf[_j] = c;                               \
    }                                               \
    wchar_t wchar;                                  \
    int result = mbtowc(&wchar, _buf, sizeof _buf); \
    if (result < 0) return -1;                      \
    write_arg(*ptr++, wchar);                       \
    c = wchar;


#else

#define get_wchar \
    wchar_t wchar = call_get;           \
    write_arg(*ptr++, wchar)            \
    c = wchar;

#define get_mchar \
    char buf[4] = {0, 0, 0, 0};         \
    wchar_t wc = call_get;              \
    int result = wctomb(buf, wc);       \
    if (result < 0) return -1;          \
    for (int i = 0; i < result; i++) {  \
        write_arg(*ptr++, buf[i]);      \
    }                                   \
    c = wc;

#endif

#define __write_int(number, success, no_write);                         \
    if (scanf_args->state.flags & FMT_NO_WRITE) return no_write;        \
    if (scanf_args->state.size_spec == spec_hh) {                       \
        CHAR *ptr = va_arg(scanf_args->args, CHAR*);                    \
        *ptr = (CHAR)(number);                                          \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_h) {                        \
        short *ptr = va_arg(scanf_args->args, short*);                  \
        *ptr = (short)(number);                                         \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_none) {                     \
        int *ptr = va_arg(scanf_args->args, int*);                      \
        *ptr = (int)(number);                                           \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_l) {                        \
        long *ptr = va_arg(scanf_args->args, long*);                    \
        *ptr = (long)(number);                                          \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_ll) {                       \
        long long *ptr = va_arg(scanf_args->args, long long*);          \
        *ptr = (long long)(number);                                     \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_j) {                        \
        intmax_t *ptr = va_arg(scanf_args->args, intmax_t*);            \
        *ptr = (intmax_t)(number);                                      \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_z) {                        \
        size_t *ptr = va_arg(scanf_args->args, size_t*);                \
        *ptr = (size_t)(number);                                        \
        return success;                                                 \
    }                                                                   \
    if (scanf_args->state.size_spec == spec_t) {                        \
        ptrdiff_t *ptr = va_arg(scanf_args->args, ptrdiff_t*);          \
        *ptr = (ptrdiff_t)(number);                                     \
        return success;                                                 \
    }                                                                   \
    return no_write;

#define write_int __write_int(number, 1, 0)

#define write_float \
if (scanf_args->state.flags & FMT_NO_WRITE)                             \
    return 0;                                                           \
if (scanf_args->state.size_spec == spec_none) {                         \
    float *ptr = va_arg(scanf_args->args, float*);                      \
    *ptr = (float)number;                                               \
    return 1;                                                           \
}                                                                       \
if (scanf_args->state.size_spec == spec_l) {                            \
    double *ptr = va_arg(scanf_args->args, double*);                    \
    *ptr = (double)number;                                              \
    return 1;                                                           \
}                                                                       \
if (scanf_args->state.size_spec == spec_L) {                            \
    long double *ptr = va_arg(scanf_args->args, long double*);          \
    *ptr = (long double)number;                                         \
    return 1;                                                           \
}

#define __get_charset_len(scanf)    concat3(__get_, scanf, _charset_len)
#define __read_charset(scanf)       concat3(__read_, scanf, _charset)
#define __charset_check(scanf)      concat3(__, scanf, _charset_check)

#define get_charset_len __get_charset_len(SCANF)
#define read_charset __read_charset(SCANF)
#define charset_check __charset_check(SCANF)

#define parse_charset                                                   \
    int inverse = 0;                                                    \
    (scanf_args->format)++;                                             \
    if ((*scanf_args->format) == '^') {                                 \
        (scanf_args->format)++;                                         \
        inverse = 1;                                                    \
    }                                                                   \
    size_t charset_len = get_charset_len(scanf_args);                   \
    CHAR charset[charset_len];                                          \
    read_charset(scanf_args, charset);


inline size_t get_charset_len(scanf_args_t *scanf_args) {
    size_t len = 0;

    if (*scanf_args->format == ']') {
        len++;
        scanf_args->format++;
    }
    while (*scanf_args->format && *scanf_args->format != ']') {
        if (*scanf_args->format == '-' && len > 0) {
            int start_digit = *(--(scanf_args->format)) - '0';
            scanf_args->format++;
            int end_digit = *(++(scanf_args->format)) - '0';
            int digits = abs(end_digit - start_digit);
            len += digits;
        }
        len++;
        scanf_args->format++;
    }

    return len;
}

inline void read_charset(scanf_args_t *scanf_args, CHAR *charset) {
    size_t len = get_charset_len(scanf_args);

    if (*scanf_args->format == '^')
        scanf_args->format++;
    if (*scanf_args->format == ']')
        scanf_args->format++;

    size_t i = 0;
    while (*scanf_args->format && *scanf_args->format != ']' && i < len) {
        if (*scanf_args->format == '-' && ISDIGIT(*(scanf_args->format - 1)) && ISDIGIT(*(scanf_args->format + 1))) {
            int start_digit = *(--scanf_args->format) + 1;
            scanf_args->format++;
            int end_digit = *(++scanf_args->format);
            if (end_digit < start_digit) {
                int _digit = start_digit - 2;
                end_digit = start_digit;
                start_digit = _digit;
            }

            for (int digit = start_digit; digit <= end_digit; digit++) {
                *charset++ = digit;
            }
            i += 2;
        } else {
            *charset++ = *(scanf_args->format++);
            i++;
        }
    }
}

inline int charset_check(const CHAR *charset, CHAR ch, int inverse) {
    while (*charset) {
        if (*charset == ch)
            return inverse ? 0 : 1;
        charset++;
    }
    return inverse ? 1 : 0;
}

#define pre_charset_check charset_check(charset, c, inverse)

/* === Обработчики форматов === */

handle_decl(char) {
    if (scanf_args->state.size_spec == spec_none) {
        CHAR *ptr = va_arg(scanf_args->args, CHAR*);
        int width = scanf_args->state.max_width ? scanf_args->state.max_width : 1;
        int c;
        for (int i = 0; i < width; i++) {
            get_mchar
        }
        call_consume_spaces;
        return 1;
    }

    if (scanf_args->state.size_spec == spec_l) {
        wchar_t *ptr = va_arg(scanf_args->args, wchar_t*);
        int width = scanf_args->state.max_width ? scanf_args->state.max_width : 1;
        int c;
        for (int i = 0; i < width; i++) {
            get_wchar
        }
        call_consume_spaces;
        return 1;
    }

    return -1;
}

handle_decl(str) {
    if (scanf_args->state.flags & FMT_NO_WRITE) return 0;

    if (scanf_args->state.size_spec == spec_none) {
        CHAR *ptr = va_arg(scanf_args->args, CHAR*);
        size_t width = (scanf_args->state.max_width) ? scanf_args->state.max_width : INT_MAX;
        size_t processed_chars = 0;
        CHAR c;
        do {
            get_mchar
            processed_chars ++;
        } while ((c || !ISSPACE(c)) && processed_chars < width);
        call_consume_spaces;
        write_arg(*ptr, 0)
        scanf_args->chars_count += processed_chars;
        return 1;
    }

    if (scanf_args->state.size_spec == spec_l) {
        wchar_t *ptr = va_arg(scanf_args->args, wchar_t*);
        size_t width = (scanf_args->state.max_width) ? scanf_args->state.max_width : INT_MAX;
        size_t processed_chars = 0;
        CHAR c = call_get;
        do {
            get_wchar
            processed_chars ++;
        } while ((c || !ISSPACE(c)) && processed_chars < width);
        write_arg(*ptr, 0)
        call_consume_spaces;
        scanf_args->chars_count += processed_chars;
        return 1;
    }

    return -1;
}

handle_decl(charset) {
    if (scanf_args->state.size_spec == spec_none) {
        CHAR *ptr = va_arg(scanf_args->args, CHAR*);
        size_t width = (scanf_args->state.max_width) ? scanf_args->state.max_width : INT_MAX;
        size_t processed_chars = 0;
        parse_charset
        int c = call_get;
        while (pre_charset_check && processed_chars < width) {
            get_mchar
            processed_chars++;
        }
        call_unget;
        write_arg(*ptr, 0)
        call_consume_spaces;
        return 1;
    }

    if (scanf_args->state.size_spec == spec_l) {
        wchar_t *ptr = va_arg(scanf_args->args, wchar_t*);
        size_t width = (scanf_args->state.max_width) ? scanf_args->state.max_width : INT_MAX;
        size_t processed_chars = 0;
        parse_charset
        int c = call_get;
        while (pre_charset_check && processed_chars < width) {
            get_wchar
            write_arg(*ptr++, wchar)
            processed_chars ++;
            c = wchar;
        }
        call_unget;
    }

    return -1;
}

handle_decl(dec_int) {
    long long number = 0;
    int negative = 0;

    int c = call_get;
    if (c == '+')
        c = call_get;
    if (c == '-') {
        negative = 1;
        c = call_get;
    }
    while (ISDIGIT(c)) {
        int digit = c - '0';
        if (digit < 0 || digit >= 10)
            break;
        number = 10 * number + digit;
        c = call_get;
    }

    if (negative) number = -number;

    write_int
}

handle_decl(int) {
    long long number = 0;
    int negative = 0;

    int c = call_get;
    if (c == '+') {
        c = call_get;
    } else if (c == '-') {
        negative = 1;
        c = call_get;
    }

    int base = 10;
    if (c == '0') {
        base = 8;
        c = call_get;
        if (c == 'x' || c == 'X') {
            base = 16;
            c = call_get;
        }
    }

    while (ISXDIGIT(c)) {
        int digit = c - '0';
        if (c >= 'a')
            digit = c - 'a' + 10;
        if (c >= 'A')
            digit = c - 'A' + 10;
        if (digit < 0 || digit >= base)
            break;
        number = base * number + digit;
        c = call_get;
    }

    if (negative) number = -number;

    write_int
}

handle_decl(uint) {
    unsigned long long number = 0;

    int c = call_get;

    while (ISDIGIT(c)) {
        int digit = c - '0';
        if (digit < 0 || digit >= 10)
            break;
        number = 10 * number + digit;
        c = call_get;
    }

    write_int
}

handle_decl(oct_int) {
    unsigned long long number = 0;

    int c = call_get;

    while (ISDIGIT(c)) {
        int digit = c - '0';
        if (digit < 0 || digit >= 8)
            break;
        number = 8 * number + digit;
        c = call_get;
    }

    write_int
}

handle_decl(hex_int) {
    unsigned long long number = 0;

    int c = call_get;

    while (ISXDIGIT(c)) {
        int digit = c - '0';
        if (c >= 'a')
            digit = c - 'a' + 10;
        if (c >= 'A')
            digit = c - 'A' + 10;
        if (digit < 0 || digit >= 16)
            break;
        number = 16 * number + digit;
        c = call_get;
    }

    write_int
}

handle_decl(float) {
    long double number = 0.0;
    int negative = 0;

    int c = call_get;
    if (c == '-') {
        negative = 1;
        c = call_get;
    }
    else if (c == '+') {
        c = call_get;
    }

    int base = 0;
    int is_past_point = 0;
    if (c == '0') {
        c = call_get;
        if (c == 'x' || c == 'X') {
            base = 16;
            c = call_get;
        }
        else if (ISDIGIT(c) || c == '.') {
            if (c == '.') is_past_point = 1;
            base = 10;
            c = call_get;
        }
        else {
            number = 0.0;
            write_float
        }
    }


    long double frac_exp = (base == 10) ? 0.1 : 0.0625;
    while (ISXDIGIT(c) || (c == '.' && !is_past_point)) {
        int digit = c - '0';
        if (c >= 'a')
            digit = c - 'a' + 10;
        if (c >= 'A')
            digit = c - 'A' + 10;
        if (digit < 0 || digit >= base) {
            break;
        }

        if (!is_past_point)
            number = base * number + digit;
        else {
            number += digit * frac_exp;
            frac_exp /= base;
        }
    }

    if (((c == 'e' || c == 'E') && base == 10) || ((c == 'p' || c == 'P') && base == 16)) {
        c = call_get;
        if (!ISDIGIT(c) && c != '-' && c != '+')
            call_unget;
        else {
            int exp_negative = 0;
            int exp_base = (base == 10) ? 10 : 2;
            int exp_pow = 0;

            if (c == '+')
                c = call_get;
            if (c == '-') {
                exp_negative = 1;
                c = call_get;
            }

            while (ISDIGIT(c)) {
                int digit = c - '0';
                exp_pow = 10 * exp_pow + digit;
                c = call_get;
            }
            exp_pow = exp_negative ? -exp_pow : exp_pow;
            long double exponent = powl(exp_base, exp_pow);
            number *= exponent;
        }
    }

    number = negative ? -number : number;

    write_float

    return 0;
}

handle_decl(num_chars) {
    __write_int(scanf_args->chars_count, 0, 0)
}

handle_decl(ptr) {
    ptrdiff_t ptrd = 0;

    int c = call_get;
    if (c != 0)
        return -1;
    c = call_get;
    if (c != 'x')
        return -1;

    c = call_get;
    while (ISXDIGIT(c)) {
        int digit = c - '0';
        if (c >= 'A')
            digit = c - 'A' + 10;
        if (c >= 'a')
            break;

        ptrd = (ptrd << 4) | digit;
        c = call_get;
    }

    if (scanf_args->state.fmt_spec & FMT_NO_WRITE) return 0;

    void **ptr = va_arg(scanf_args->args, void**);
    *ptr = (void*)ptrd;
    return 1;
}

/* === Вспомогательные функции для __nscanf === */

#define __get_size_spec(scanf) concat3(get_, scanf, _size_spec)
#define __handle_fmt(scanf) concat3(handle_, scanf, _format)

#define get_size_spec __get_size_spec(SCANF)
#define handle_format __handle_fmt(SCANF)

int get_size_spec(const CHAR **str) {
    CHAR spec = *(*str);
    switch (spec) {
        case 'h':
            spec = *(*++str);
            (*str)++;
            if (spec == 'h') return spec_hh;
            else return spec_h;
        case 'l':
            spec = *(*++str);
            (*str)++;
            if (spec == 'l') return spec_ll;
            else return spec_l;
        case 'j': (*str)++; return spec_j;
        case 'z': (*str)++; return spec_z;
        case 't': (*str)++; return spec_t;
        case 'L': (*str)++; return spec_L;
        default: return spec_none;
    }
}

#define __handle_case(fmt, func, scanf) \
    case fmt: return concat3(handle_##func##_, scanf, _spec)(scanf_args, scanf_params);

#define handle_case(fmt, func) __handle_case(fmt, func, SCANF)

int handle_format(scanf_args_t *scanf_args, const scanf_params_t *scanf_params) {
    switch (scanf_args->state.fmt_spec) {
        handle_case('c', char)
        handle_case('s', str)
        handle_case('[', charset)
        handle_case('d', dec_int)
        handle_case('i', int)
        handle_case('u', uint)
        handle_case('o', oct_int)
        handle_case('x', hex_int)
        handle_case('X', hex_int)
        handle_case('n', num_chars)
        handle_case('a', float)
        handle_case('A', float)
        handle_case('e', float)
        handle_case('E', float)
        handle_case('f', float)
        handle_case('F', float)
        handle_case('g', float)
        handle_case('G', float)
        handle_case('p', ptr)
        default: return -1;
    }
}

#define __nscanf_func(scanf)    concat(__n, scanf)

#define __nscanf                __nscanf_func(SCANF)

// __nscanf - это основа для всех *scanf-функций.
int __nscanf(scanf_args_t *scanf_args, const scanf_params_t *params) {
    int handled_args = 0;
    while (scanf_args->format) {
        CHAR fmt_char = *scanf_args->format++;
        if (fmt_char != '%') {
            CHAR buf_char = call_get;
            if (buf_char != fmt_char)
                return EOF;
            continue;
        }

        fmt_char = *scanf_args->format++;
        if (fmt_char == '%') {
            CHAR buf_char = call_get;
            if (buf_char != '%')
                return EOF;
            continue;
        }

        scanf_args->state = (scanf_state_t){};

        if (fmt_char == '*') {
            scanf_args->state.flags |= FMT_NO_WRITE;
            fmt_char = *scanf_args->format++;
        }

        if (ISDIGIT(fmt_char)) {
            int width = STRTOL(scanf_args->format, (CHAR**)&(scanf_args->format), 10);
            if (width <= 0)
                return -1;
            scanf_args->state.max_width = width;
        }

        int size_spec = get_size_spec(&scanf_args->format);
        scanf_args->state.size_spec = size_spec;

        int fmt_spec = *scanf_args->format++;
        scanf_args->state.fmt_spec = fmt_spec;

        int result = handle_format(scanf_args, params);
        if (result < 0)
            return -1;
        if (result == 1)
            handled_args++;
    }

    return handled_args;
}

/* === Основные функции === */

#define __vsscanf(scanf)    concat(vs, scanf)
#define __vfscanf(scanf)    concat(vf, scanf)
#define __vscanf(scanf)     concat(v, scanf)
#define __sscanf(scanf)     concat(s, scanf)
#define __fscanf(scanf)     concat(f, scanf)
#define __scanf(scanf)      scanf

int __vsscanf(SCANF)(const CHAR *restrict buffer, const CHAR *restrict format, va_list args) {
    scanf_args_t scanf_args;
    scanf_args.buffer = (void*)buffer;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    return __nscanf(&scanf_args, &SSCANF);
}

int __vfscanf(SCANF)(FILE *restrict buffer, const CHAR *restrict format, va_list args) {
    scanf_args_t scanf_args;
    scanf_args.buffer = buffer;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    return __nscanf(&scanf_args, &FSCANF);
}

int __vscanf(SCANF)(const CHAR *restrict format, va_list args) {
    scanf_args_t scanf_args;
    scanf_args.buffer = stdin;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    return __nscanf(&scanf_args, &FSCANF);
}

int __sscanf(SCANF)(const CHAR *restrict buffer, const CHAR *restrict format, ...) {
    va_list args;
    va_start(args, format);
    scanf_args_t scanf_args;
    scanf_args.buffer = (void*)buffer;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    int result = __nscanf(&scanf_args, &SSCANF);
    va_end(args);
    return result;
}

int __fscanf(SCANF)(FILE *restrict buffer, const CHAR *restrict format, ...) {
    va_list args;
    va_start(args, format);
    scanf_args_t scanf_args;
    scanf_args.buffer = buffer;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    int result = __nscanf(&scanf_args, &FSCANF);
    va_end(args);
    return result;
}

int __scanf(SCANF)(const CHAR *restrict format, ...) {
    va_list args;
    va_start(args, format);
    scanf_args_t scanf_args;
    scanf_args.buffer = stdin;
    scanf_args.format = format;
    scanf_args.chars_count = 0;
    scanf_args.args = args;
    scanf_args.state = (scanf_state_t){};
    int result = __nscanf(&scanf_args, &FSCANF);
    va_end(args);
    return result;
}

#undef get_mchar
#undef get_wchar