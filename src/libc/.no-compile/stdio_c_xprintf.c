/*
 * stdio_c_sprintf.c
 * 
 * Функция sprintf из stdio.c
 * 
 */

#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/stddef.h"
#include "../include/ctype.h"
#include "../include/tgmath.h"
#include "../include/wchar.h"
#include "../include/string.h"
#include "../etc/intmanip.h"
#include <stdint.h>

#define concat(a, b) a##b
#define concat3(a, b, c) a##b##c
#define eval(a) a

#ifndef _PUT_FUNCS
#define _PUT_FUNCS

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

typedef struct {
    size_spec_t size_spec;
    short flags;
    int min_width;
    int precision;
    CHAR spec;
} printf_state_t;

#define put_args(CHAR) CHAR c, void **buf, size_t *chars_count, size_t maxlen

typedef int (*put_func_t)(put_args(CHAR));

#define signed_size_t long
#define uptrdiff_t unsigned long

#define FMT_LEFT_JUSTIFIED  1
#define FMT_ALWAYS_SIGN     2
#define FMT_PREPEND_SPACE   4
#define FMT_ALTERNATE       8
#define FMT_PAD_W_ZERO      16

/* === put-функции === */


CHAR f_put_buf[128];
int f_putbuf_index = 0;

void flush_f_put_buf(FILE *stream) {
    fwrite(f_put_buf, f_putbuf_index, 1, stream);
    f_putbuf_index = 0;
}

// put для строк
int str_put(put_args(CHAR)) {
    if (*chars_count >= maxlen - 1)
        return 0;
    CHAR *str = *(CHAR**)buf;
    if (str) {
        *str++ = c;
        *buf = (void*)str;
    }
    (*chars_count)++;
    return 1;
}

// put для широкосимвольных строк
int wide_str_put(put_args(CHAR)) {
    if (*chars_count >= maxlen - 1)
        return 0;

    static int charbuf = 0;
    charbuf = (charbuf << 8) | c;
    wchar_t wc;
    int sz = mbtowc(&wc, (CHAR*)&charbuf, 4);
    if (sz != MBC_INVALID) {
        wchar_t *str = *(wchar_t**)buf;
        if (str) {
            *str++ = wc;
            *buf = (void*)str;
        }
        charbuf = 0;
        (*chars_count)++;
        return 1;
    }

    return 0;
}

// put для потоков FILE
int f_put(put_args(CHAR)) {
    if (c == 0)
        return 0;
    FILE *stream = *(FILE**)buf;
    if (f_putbuf_index == sizeof f_put_buf) {
        flush_f_put_buf(stream);
    }
    f_put_buf[f_putbuf_index++] = c;
    (*chars_count)++;
}

// put для широкосимвольных потоков FILE
int wide_f_put(put_args(CHAR)) {
    if (c == 0)
        return 0;
    FILE *stream = *(FILE**)buf;

    static int charbuf = 0;
    charbuf = (charbuf << 8) | c;
    wchar_t wc;
    int sz = mbtowc(&wc, (CHAR*)&charbuf, 4);
    if (sz != MBC_INVALID) {
        if ((int)(f_putbuf_index - sizeof f_put_buf) < sz) {
            flush_f_put_buf(stream);
        }
        f_put_buf[f_putbuf_index++] = BYTE0(wc);
        f_put_buf[f_putbuf_index++] = BYTE1(wc);
        f_put_buf[f_putbuf_index++] = BYTE2(wc);
        f_put_buf[f_putbuf_index++] = BYTE3(wc);

    }
}

#endif

/* === Макросы для обработки форматов === */

#define is_int_spec(spec) (spec == 'd' || spec == 'i' || spec == 'u' || spec == 'o' || spec == 'x' || spec == 'X')

#define call_put(c) put(c, str, chars_count, maxlen)

// Отныне я - властелин препроцессора!

#define put_w_padding(len, put_func) \
    if (state->min_width == -1) {                                   \
        int min_width = va_arg(*args, int);                         \
        if (min_width < 0) {                                        \
            min_width = -min_width;                                 \
            state->flags |= FMT_LEFT_JUSTIFIED;                     \
        }                                                           \
        state->min_width = min_width;                               \
    }                                                               \
    if (state->min_width > len) {                                   \
        CHAR padding = state->flags & FMT_PAD_W_ZERO ? '0' : ' ';   \
        if (state->flags & FMT_LEFT_JUSTIFIED)                      \
            padding = ' ';                                          \
        if (is_int_spec(state->spec) && state->precision > 0)       \
            padding = ' ';                                          \
        if (state->flags & FMT_LEFT_JUSTIFIED) {                    \
            put_func;                                               \
            for (int i = 0; i < state->min_width - len; i++) {      \
                call_put(padding);                                  \
            }                                                       \
        } else {                                                    \
            for (int i = 0; i < state->min_width - len; i++) {      \
                call_put(padding);                                  \
            }                                                       \
            put_func;                                               \
        }                                                           \
    } else {                                                        \
        put_func;                                                   \
    }


#define put_int(number)                                             \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number)                                      \
        return 0;                                                   \
    if (state->flags & FMT_ALWAYS_SIGN && number > 0) {             \
        call_put('+');                                              \
    }                                                               \
    else if (state->flags & FMT_PREPEND_SPACE && number >= 0) {     \
        call_put(' ');                                              \
    }                                                               \
    CHAR buf[precision + 50] = {};                                  \
    CHAR *ptr = buf + precision + 49;                               \
    int sgn = number < 0 ? 1 : 0;                                   \
    int len = 0;                                                    \
    while (number) {                                                \
        *ptr-- = (number % 10) + '0';                               \
        len ++;                                                     \
        number /= 10;                                               \
    }                                                               \
    while (len < precision) {                                       \
        *ptr-- = '0';                                               \
        len ++;                                                     \
    }                                                               \
    if (sgn) {                                                      \
        *ptr-- = '-';                                               \
        len ++;                                                     \
    }                                                               \
    put_w_padding(len, for (int i = 0; i < len; i++) {              \
        call_put(*ptr++);                                           \
    });


#define put_uint(number)    \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number)                                      \
        return 0;                                                   \
    CHAR buf[precision + 50];                                       \
    CHAR *_buf = buf + precision + 49;                              \
    int len = 0;                                                    \
    while (number) {                                                \
        *_buf-- = '0' + number % 10;                                \
        len ++;                                                     \
        number /= 10;                                               \
    }                                                               \
    while (len < precision) {                                       \
        *_buf-- = '0';                                              \
        len ++;                                                     \
    }                                                               \
    _buf++;                                                         \
    put_w_padding(len, for (int i = 0; i < len; i++) {              \
        call_put(*_buf++);                                          \
    });


#define put_oct_uint(number)                                        \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number) {                                    \
        if (state->flags & FMT_ALTERNATE)                           \
            call_put('0');                                          \
        return 0;                                                   \
    }                                                               \
    CHAR _buf[precision + 50];                                      \
    CHAR *buf = _buf + precision + 49;                              \
    int len = 0;                                                    \
    while (number) {                                                \
        *buf-- = '0' + (number & 0x07);                             \
        number >>= 3;                                               \
        len ++;                                                     \
    }                                                               \
    if (state->flags & FMT_ALTERNATE) {                             \
        *buf-- = '0';                                               \
        len ++;                                                     \
    }                                                               \
    while (len < precision) {                                       \
        *buf-- = '0';                                               \
        len ++;                                                     \
    }                                                               \
    put_w_padding(len, for (int i = 0; i < len; i++) {              \
        call_put(*buf++);                                           \
    });


#define put_hex_uint(number)                                        \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number) {                                    \
        return 0;                                                   \
    }                                                               \
    CHAR _buf[precision + 50];                                      \
    CHAR *buf = _buf + precision + 49;                              \
    int len = 0;                                                    \
    int is_zero = number == 0;                                      \
    while (number) {                                                \
        CHAR digit = number & 0x0F;                                 \
        if (digit < 10) {                                           \
            if (caps) digit = 'A' + digit - 10;                     \
            else digit = 'a' + digit - 10;                          \
        } else {                                                    \
            digit = '0' + digit;                                    \
        }                                                           \
        *buf-- = digit;                                             \
        number >>= 4;                                               \
        len ++;                                                     \
    }                                                               \
    while (len < precision) {                                       \
        *buf-- = '0';                                               \
        len ++;                                                     \
    }                                                               \
    if (state->flags & FMT_ALTERNATE && !is_zero) {                 \
        if (caps) *buf-- = 'X';                                     \
        else *buf-- = 'x';                                          \
        len ++;                                                     \
        *buf-- = '0';                                               \
        len++;                                                      \
    }                                                               \
    put_w_padding(len, for (int i = 0; i < len; i++) {              \
        call_put(*buf++);                                           \
    });

#ifdef WCHAR
#define INFSTRL L"inf"
#define INFSTRU L"INF"
#define NANSTRL L"nan"
#define NANSTRU L"NAN"
#else
#define INFSTRL "inf"
#define INFSTRU "INF"
#define NANSTRL "nan"
#define NANSTRU "NAN"
#endif

#define put_float(number)                                           \
    if (isnan(number)) {                                            \
        const CHAR *nanstr = NANSTRL;                               \
        if (caps) {                                                 \
            nanstr = NANSTRU;                                       \
        }                                                           \
        while (*nanstr) {                                           \
            call_put(*nanstr++);                                    \
        }                                                           \
        return 0;                                                   \
    }                                                               \
    if (isinf(number)) {                                            \
        const CHAR *infstr = INFSTRL;                               \
        if (caps) {                                                 \
            infstr = INFSTRU;                                       \
        }                                                           \
        while (*infstr) {                                           \
            call_put(*infstr++);                                    \
        }                                                           \
        return 0;                                                   \
    }                                                               \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 6;                                              \
    __typeof__(number) max_pow = 1;                                 \
    while (max_pow < number)                                        \
        max_pow *= 10;                                              \
    max_pow /= 10;                                                  \
    if (number < 0) {                                               \
        call_put('-');                                              \
        number = -number;                                           \
    }                                                               \
    while (number > 1) {                                            \
        CHAR digit = '0' + (int)(number / max_pow);                 \
        call_put(digit);                                            \
        number = fmod(number, max_pow);                             \
        max_pow /= 10;                                              \
    }                                                               \
    if (precision == 0) {                                           \
        if (state->flags & FMT_ALTERNATE)                           \
            call_put('.');                                          \
    } else {                                                        \
        max_pow = pow(10, precision - 1);                           \
        number *= max_pow;                                          \
        number *= 10;                                               \
        number = round(number);                                     \
        for (int i = 0; i < precision; i++) {                       \
            CHAR digit = '0' + (int)(number / max_pow);             \
            call_put(digit);                                        \
            number = fmod(number, max_pow);                         \
            max_pow /= 10;                                          \
        }                                                           \
    }


// На кой чёрт выводить float в 16-ричной системе ?!?!?!?!??!?

#define put_hex_float(number)                                           \
    int precision = state->precision;                                   \
    if (precision == -1)                                                \
        precision = va_arg(*args, int);                                 \
    __typeof__(number) max_pow = 1;                                     \
    while (max_pow < number)                                            \
        max_pow *= 16;                                                  \
    max_pow /= 16;                                                      \
    if (number < 0) {                                                   \
        call_put('-');                                                  \
        number = -number;                                               \
    }                                                                   \
    call_put('0');                                                      \
    call_put(caps ? 'X' : 'x');                                         \
    while (number > 1) {                                                \
        CHAR digit = '0' + (int)(number / max_pow);                     \
        if (digit >= 0x3A) {                                            \
            digit += 7;                                                 \
            if (!caps) digit += 0x20;                                   \
        }                                                               \
        call_put(digit);                                                \
        number = fmod(number, max_pow);                                 \
        max_pow /= 16;                                                  \
    }                                                                   \
    if (precision == 0) {                                               \
        if (state->flags & FMT_ALTERNATE)                               \
            call_put('.');                                              \
    } else {                                                            \
        call_put('.');                                                  \
        if (precision > 0) {                                            \
            max_pow = pow(16, precision - 1);                           \
            number *= max_pow;                                          \
            number *= 16;                                               \
            number = round(number);                                     \
            for (int i = 0; i < precision; i++) {                       \
                CHAR digit = '0' + (int)(number / max_pow);             \
                if (digit >= 0x3A) {                                    \
                    digit += 7;                                         \
                    if (!caps) digit += 0x20;                           \
                }                                                       \
                call_put(digit);                                        \
                number = fmod(number, max_pow);                         \
                max_pow /= 16;                                          \
            }                                                           \
        } else {                                                        \
            while (number > 0) {                                        \
                number *= 16;                                           \
                CHAR digit = '0' + (int)(number / 1);                   \
                if (digit >= 0x3A) {                                    \
                    digit += 7;                                         \
                    if (!caps) digit += 0x20;                           \
                }                                                       \
                call_put(digit);                                        \
                number = fmod(number, 1);                               \
            }                                                           \
        }                                                               \
    }                                                                   \

#define put_exp_flt(number)                                         \
    int exponent = 0;                                               \
    int neg = number < 0;                                           \
    if (neg) number = -number;                                      \
    while (number > 10) {                                           \
        number /= 10;                                               \
        exponent ++;                                                \
    }                                                               \
    while (number < 0.1) {                                          \
        number *= 10;                                               \
        exponent --;                                                \
    }                                                               \
    if (neg) number = -number;                                      \
    put_float(number);                                              \
    call_put(caps ? 'E' : 'e');                                     \
    if (exponent > 0) {                                             \
        call_put('+');                                              \
    } else {                                                        \
        call_put('-');                                              \
        exponent = -exponent;                                       \
    }                                                               \
    if (exponent < 10 && exponent > -10)                            \
        call_put('0');                                              \
    int max_exp_pow = 1;                                            \
    while (max_exp_pow < exponent)                                  \
        max_exp_pow *= 10;                                          \
    max_exp_pow /= 10;                                              \
    while (exponent) {                                              \
        call_put('0' + (exponent / max_exp_pow) % 10);              \
        exponent /= 10;                                             \
    }

/* === Обработчики форматов === */

#define __handle_args (void **str, size_t maxlen, printf_state_t *state, va_list *args,   \
    size_t *chars_count, put_func_t put)
#define __handle_args_caps (void **str, size_t maxlen, printf_state_t *state, va_list *args,   \
    size_t *chars_count, put_func_t put, int caps)

#define __handle_decl(name, the_printf) \
    inline int concat3(handle_##name##_, the_printf,_fmtspec) __handle_args

#define __handle_decl_caps(name, the_printf)  \
    inline int concat3(handle_##name##_, the_printf,_fmtspec) __handle_args_caps

#define handle_decl(name)      __handle_decl(name, PRINTF)
#define handle_decl_caps(name) __handle_decl_caps(name, PRINTF)

handle_decl(char) {
#ifndef WCHAR
    if (state->size_spec == spec_none) {                                                // Нет уточнения размера
        int ch = va_arg(*args, int);
        unsigned char c = (unsigned char)ch;
        put_w_padding(1, call_put(c))
        return 0;
    }

    if (state->size_spec == spec_l) {                                                  // Уточнение "l"
        wint_t ch = va_arg(*args, wint_t);
        CHAR buf[4] = {0, 0, 0, 0};
        int char_sz = wctomb(buf, ch);
        CHAR *_buf = buf;
        put_w_padding(char_sz, while (*_buf) {
            call_put(*_buf++);
        })
        return 0;
    }

#else
    if (state->size_spec == spec_none) {
        int ch = va_arg(*args, int);
        char *ptr = (char*)&ch;
        wchar_t c;
        size_t cvt = mbtowc(&c, ptr, 4);
        if (cvt == MBC_INVALID) return -1;
        put_w_padding(1, call_put(c))
        return 0;
    }

    if (state->size_spec == spec_l) {
        wint_t ch = va_arg(*args, wint_t);
        wchar_t c = (wchar_t)ch;
        put_w_padding(1, call_put(c))
        return 0;
    }

#endif

    return -1;
}

handle_decl(str) {
#ifndef WCHAR
    if (state->size_spec == spec_none) {
        const char *string = va_arg(*args, const char*);
        size_t len = strlen(string);
        int stop = len;

        if (state->precision == -1) {
            stop = va_arg(*args, int);
        }
        else if (state->precision > 0)
            stop = state->precision;

        put_w_padding(stop, for (int i = 0; i < stop; i++) {
            call_put(string[i]);
        })

        return 0;
    }

    if (state->size_spec == spec_l) {
        const wchar_t *string = va_arg(*args, const wchar_t*);
        size_t len = wcslen(string);
        int stop = len;

        if (state->precision == -1) {
            stop = va_arg(*args, int);
        }
        else if (state->precision > 0)
            stop = state->precision;

        put_w_padding(stop, {for (int i = 0; i < stop; i++) {
                CHAR buf[4] = {};
                CHAR *_buf = buf;
                wctomb(buf, string[i]);
                while (*_buf) {
                    call_put(*_buf++);
                }
            }})

        return 0;
    }
#else
    if (state->size_spec == spec_none) {
        const char *string = va_arg(*args, const char*);
        size_t len = strlen(string);
        int stop = len;

        wchar_t wcs[len];
        size_t newlen = mbstowcs(wcs, string, 4);
        len = newlen;
        if (len == (size_t)-1)
            return -1;

        if (state->precision == -1) {
            stop = va_arg(*args, int);
        } else if (state->precision > 0)
            stop = state->precision;

        wchar_t *wcsptr = wcs;
        put_w_padding(stop, for (int i = 0; i < stop; i++) {
            call_put(*wcsptr++);
        })
    }

    if (state->size_spec == spec_l) {
        const wchar_t *string = va_arg(*args, const wchar_t*);
        size_t len = wcslen(string);
        int stop = len;

        if (state->precision == -1) {
            stop = va_arg(*args, int);
        } else if (state->precision > 0)
            stop = state->precision;

        put_w_padding(stop, for (int i = 0; i < stop; i++) {
            call_put(*string++);
        })
    }
#endif

    return -1;
}

handle_decl(sint) {
    if (state->size_spec == spec_hh || state->size_spec == spec_h || state->size_spec == spec_none) {
        int number = va_arg(*args, int);
        put_int(number)
        return 0;
    }

    if (state->size_spec == spec_l) {
        long number = va_arg(*args, long);
        put_int(number)
        return 0;
    }

    if (state->size_spec == spec_ll) {
        long long number = va_arg(*args, long long);
        put_int(number)
        return 0;
    }

    if (state->size_spec == spec_j) {
        intmax_t number = va_arg(*args, intmax_t);
        put_int(number)
        return 0;
    }

    if (state->size_spec == spec_z) {
        signed_size_t number = va_arg(*args, signed_size_t);
        put_int(number)
        return 0;
    }

    if (state->size_spec == spec_t) {
        ptrdiff_t number = va_arg(*args, ptrdiff_t);
        put_int(number)
        return 0;
    }

    return -1;
}

handle_decl(uint) {
    if (state->size_spec == spec_hh || state->size_spec == spec_h || state->size_spec == spec_none) {
        unsigned int number = va_arg(*args, unsigned int);
        put_uint(number)
        return 0;
    }

    if (state->size_spec == spec_l) {
        unsigned long number = va_arg(*args, unsigned long);
        put_uint(number)
        return 0;
    }

    if (state->size_spec == spec_ll) {
        unsigned long long number = va_arg(*args, unsigned long long);
        put_uint(number)
        return 0;
    }

    if (state->size_spec == spec_j) {
        uintmax_t number = va_arg(*args, uintmax_t);
        put_uint(number)
        return 0;
    }

    if (state->size_spec == spec_z) {
        size_t number = va_arg(*args, size_t);
        put_uint(number)
        return 0;
    }

    if (state->size_spec == spec_t) {
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_uint(number)
        return 0;
    }

    return -1;
}

handle_decl(oct_int) {
    if (state->size_spec == spec_hh || state->size_spec == spec_h || state->size_spec == spec_none) {
        unsigned int number = va_arg(*args, unsigned int);
        put_oct_uint(number)
        return 0;
    }

    if (state->size_spec == spec_l) {
        unsigned long number = va_arg(*args, unsigned long);
        put_oct_uint(number)
        return 0;
    }

    if (state->size_spec == spec_ll) {
        unsigned long long number = va_arg(*args, unsigned long long);
        put_oct_uint(number)
        return 0;
    }

    if (state->size_spec == spec_j) {
        uintmax_t number = va_arg(*args, uintmax_t);
        put_oct_uint(number)
        return 0;
    }

    if (state->size_spec == spec_z) {
        size_t number = va_arg(*args, size_t);
        put_oct_uint(number)
        return 0;
    }

    if (state->size_spec == spec_t) {
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_oct_uint(number)
        return 0;
    }

    return -1;
}

handle_decl_caps(hex_int) {
    if (state->size_spec == spec_hh || state->size_spec == spec_h || state->size_spec == spec_none) {
        unsigned int number = va_arg(*args, unsigned int);
        put_hex_uint(number)
        return 0;
    }

    if (state->size_spec == spec_l) {
        unsigned long number = va_arg(*args, unsigned long);
        put_hex_uint(number)
        return 0;
    }

    if (state->size_spec == spec_ll) {
        unsigned long long number = va_arg(*args, unsigned long long);
        put_hex_uint(number)
        return 0;
    }

    if (state->size_spec == spec_j) {
        uintmax_t number = va_arg(*args, uintmax_t);
        put_hex_uint(number)
        return 0;
    }

    if (state->size_spec == spec_z) {
        size_t number = va_arg(*args, size_t);
        put_hex_uint(number)
        return 0;
    }

    if (state->size_spec == spec_t) {
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_hex_uint(number)
        return 0;
    }

    return -1;
}

handle_decl_caps(float) {
    if (state->size_spec == spec_none || state->size_spec == spec_l) {
        double number = va_arg(*args, double);
        put_float(number)
        return 0;
    }

    if (state->size_spec == spec_L) {
        long double number = va_arg(*args, long double);
        put_float(number)
        return 0;
    }

    return -1;
}

handle_decl_caps(exp_flt) {
    if (state->size_spec == spec_none || state->size_spec == spec_l) {
        double number = va_arg(*args, double);

        put_exp_flt(number)

        return 0;
    }

    if (state->size_spec == spec_L) {
        long double number = va_arg(*args, long double);

        put_exp_flt(number)

        return 0;
    }
}

handle_decl_caps(exp16_flt) {
    if (state->size_spec == spec_none || state->size_spec == spec_l) {
        double number = va_arg(*args, double);

        int exponent = 0;
        int neg = number < 0;
        if (neg) number = -number;
        while (number > 16) {
            number /= 16;
            exponent ++;
        }
        while (number < 0.0625) {
            number *= 16;
            exponent --;
        }

        if (neg) number = -number;
        put_hex_float(number)
        call_put(caps ? 'P' : 'p');
        if (exponent > 0) {
            call_put('+');
        } else {
            call_put('-');
            exponent = -exponent;
        }

        if (exponent < 16 && exponent > -16)
        call_put('0');

        int max_exp_pow = 1;
        while (max_exp_pow < exponent)
            max_exp_pow <<= 4;
        max_exp_pow >>= 4;

        while (exponent) {
            CHAR digit = '0' + (exponent / max_exp_pow);
            if (digit >= 0x3A) {
                digit += 7;
                if (!caps) digit += 0x20;
            }
            call_put(digit);
            exponent >>= 4;
            max_exp_pow >>= 4;
        }

        return 0;
    }

    if (state->size_spec == spec_L) {                                                        // Уточнение размера "L"
        double number = va_arg(*args, long double);

        int exponent = 0;
        int neg = number < 0;
        if (neg) number = -number;
        while (number > 16) {
            number /= 16;
            exponent ++;
        }
        while (number < 0.0625) {
            number *= 16;
            exponent --;
        }

        if (neg) number = -number;
        put_hex_float(number)
        call_put(caps ? 'P' : 'p');
        if (exponent > 0) {
            call_put('+');
        } else {
            call_put('-');
            exponent = -exponent;
        }

        if (exponent < 16 && exponent > -16)
            call_put('0');

        int max_exp_pow = 1;
        while (max_exp_pow < exponent)
            max_exp_pow <<= 4;
        max_exp_pow >>= 4;

        while (exponent) {
            CHAR digit = '0' + (exponent / max_exp_pow);
            if (digit >= 0x3A) {
                digit += 7;
                if (!caps) digit += 0x20;
            }
            call_put(digit);
            exponent >>= 4;
            max_exp_pow >>= 4;
        }

        return 0;
    }

    return -1;
}

handle_decl_caps(flt) {
    if (state->size_spec == spec_none || state->size_spec == spec_l) {
        double number = va_arg(*args, double);

        if (state->precision == -1)
            state->precision = va_arg(*args, int);

        int p = 0;
        if (state->precision < 0)
            p = 6;
        else if (state->precision == 0)
            p = 1;
        else
            p = state->precision;

        double _number = number;
        int x = 0;
        while (_number > 10) {
            _number /= 10;
            x ++;
        }
        while (_number < 0.1) {
            _number *= 10;
            x --;
        }

        int _precision = state->precision;
        if (p > x && x >= -4) {
            state->precision = p - 1 - x;
            put_float(number)
        } else {
            state->precision = p - 1;
            put_exp_flt(number)
        }
        state->precision = _precision;
        return 0;
    }

    if (state->size_spec == spec_L) {
        long double number = va_arg(*args, long double);

        if (state->precision == -1)
            state->precision = va_arg(*args, int);

        int p = 0;
        if (state->precision < 0)
            p = 6;
        else if (state->precision == 0)
            p = 1;
        else
            p = state->precision;

        long double _number = number;
        int x = 0;
        while (_number > 10) {
            _number /= 10;
            x ++;
        }
        while (_number < 0.1) {
            _number *= 10;
            x --;
        }

        int _precision = state->precision;
        if (p > x && x >= -4) {
            state->precision = p - 1 - x;
            put_float(number)
        } else {
            state->precision = p - 1;
            put_exp_flt(number)
        }
        state->precision = _precision;
        return 0;
    }

    return -1;
}

handle_decl(cw) {
    int chars = *chars_count;
    put_int(chars)

    if (state->size_spec == spec_hh) {
        signed char *ptr = va_arg(*args, signed char*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_h) {
        short *ptr = va_arg(*args, short*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_none) {
        int *ptr = va_arg(*args, int*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_l) {
        long *ptr = va_arg(*args, long*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_ll) {
        long long *ptr = va_arg(*args, long long*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_j) {                                                        // Уточнение размера "j"
        intmax_t *ptr = va_arg(*args, intmax_t*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_z) {
        signed_size_t *ptr = va_arg(*args, signed_size_t*);
        *ptr = chars;
        return 0;
    }

    if (state->size_spec == spec_t) {
        ptrdiff_t *ptr = va_arg(*args, ptrdiff_t*);
        *ptr = chars;
        return 0;
    }

    return -1;
}

handle_decl(ptr) {
    if (state->size_spec)
        return -1;

    void *ptr = va_arg(*args, void*);
    long ptrint = (long)ptr;
    int caps = 0;

    put_hex_uint(ptrint)

    return 0;
}

/* === Функции для printf_state_t === */

#define __get_size_spec_func(printf) concat3(get_, printf, _size_spec)
#define get_size_spec __get_size_spec_func(PRINTF)

size_spec_t get_size_spec(const CHAR *format, const CHAR **new_format) {
    if (!new_format) return -1;
    *new_format = format + 1;
    switch(*format) {
        case 'h':
            format++;
            if (*format == 'h') {
                *new_format = format + 2;
                return spec_hh;
            }
            return spec_h;
        case 'l':
            format++;
            if (*format == 'l') {
                *new_format = format + 2;
                return spec_ll;
            }
            return spec_l;
        case 'j': return spec_j;
        case 'z': return spec_z;
        case 't': return spec_t;
        case 'L': return spec_L;
        default:
            *new_format = format;
            return spec_none;
    }
}

#define __handle_case(fmt, func, the_printf) \
    case fmt: return concat3(handle_##func##_,the_printf,_fmtspec) (str, maxlen, state, args, chars_count, put);
#define __handle_case_caps(fmt1, fmt2, func, the_printf) \
    case fmt1: return concat3(handle_##func##_,the_printf,_fmtspec) (str, maxlen, state, args, chars_count, put, 0);   \
    case fmt2: return concat3(handle_##func##_,the_printf,_fmtspec) (str, maxlen, state, args, chars_count, put, 1);

#define handle_case(fmt, func)              __handle_case(fmt, func, PRINTF)
#define handle_case_caps(fmt1, fmt2, func)  __handle_case_caps(fmt1, fmt2, func, PRINTF)

#define __handle_fmt_func(printf) concat3(handle_, printf, _format)
#define handle_fmt_func __handle_fmt_func(PRINTF)

int handle_fmt_func(void **str, size_t maxlen, printf_state_t *state, size_t *chars_count, va_list *args, put_func_t put) {
    switch (state->spec) {
        handle_case('c', char)
        handle_case('s', str)
        handle_case('d', sint)
        handle_case('i', sint)
        handle_case('u', uint)
        handle_case('o', oct_int)
        handle_case_caps('x', 'X', hex_int)
        handle_case_caps('f', 'F', float)
        handle_case_caps('e', 'E', exp_flt)
        handle_case_caps('a', 'A', exp16_flt)
        handle_case_caps('g', 'G', flt)
        handle_case('n', cw)
        handle_case('p', ptr)
        default: return -1;
    }
}

#define __nprintf_func(the_printf)  concat(__n, the_printf)
#define __nprintf                   __nprintf_func(PRINTF)

// __nprintf - это основной шаблон для всех *printf-функций.
int __nprintf(void *out, size_t maxlen, const CHAR *format, va_list args, put_func_t put) {
    printf_state_t state;
    size_t chars_count = 0;

    while (*format && chars_count < maxlen - 1) {
        if (*format != '%') {
            put(*format++, &out, &chars_count, maxlen);
            continue;
        }

        format++;
        if (*format == '%') {
            put('%', &out, &chars_count, maxlen);
            format ++;
            continue;
        }

        state.flags = 0;
        state.precision = 0;
        state.min_width = 0;
        state.size_spec = 0;
        state.spec = 0;

        if (*format == '-') {
            state.flags |= FMT_LEFT_JUSTIFIED;
            format++;
        }
        if (*format == '+') {
            state.flags |= FMT_ALWAYS_SIGN;
            format ++;
        }
        if (*format == ' ') {
            state.flags |= FMT_PREPEND_SPACE;
            format ++;
        }
        if (*format == '#') {
            state.flags |= FMT_ALTERNATE;
            format++;
        }
        if (*format == '0') {
            state.flags |= FMT_PAD_W_ZERO;
            format++;
        }

        int min_width;
        if (*format == '*') {
            min_width = -1;
            format++;
        }
        else if (ISDIGIT(*format)) {
            min_width = (int)(STRTOL(format, (CHAR**)&format, 10));
        } else {
            min_width = 0;
        }
        state.min_width = min_width;

        if (*format == '.') {
            format++;
            int precision;
            if (*format == '*') {
                precision = -1;
                format++;
            } else if (isdigit(*format)) {
                precision = (int)(STRTOL(format, (CHAR**)&format, 10));
            } else {
                // Невозможное значение точности
                return -1;
            }
            state.precision = precision;
        } else {
            state.precision = -2;
        }

        state.size_spec = get_size_spec(format, &format);

        state.spec = *format++;

        int fmt_result = handle_fmt_func(&out, maxlen, &state, &chars_count, &args, put);

        if (fmt_result)         // Произошла ошибка при обработке формата
            return fmt_result;
    }

    put(0, &out, &chars_count, maxlen);

    va_end(args);
    return chars_count;
}

/* === Основные функции === */

#ifdef WCHAR
#define __str_put_func  wide_str_put
#define __f_put_func    wide_f_put
#else
#define __str_put_func  str_put
#define __f_put_func    f_put
#endif

#ifdef SNPRINTF

#define __vsnprintf(printf) concat(vsn, printf)
#define __vsprintf(printf)  concat(vs, printf)
#define __snprintf(printf)  concat(sn, printf)
#define __sprintf(printf)   concat(s, printf)

int __vsnprintf(PRINTF)(CHAR *out, size_t maxlen, const CHAR *format, va_list args) {
    return __nprintf(out, maxlen, format, args, __str_put_func);
}

int __vsprintf(PRINTF)(CHAR *out, const CHAR *format, va_list args) {
    return __nprintf(out, (size_t)-1, format, args, __str_put_func);
}

int __snprintf(PRINTF)(CHAR *out, size_t maxlen, const CHAR *format, ...) {
    va_list args;
    va_start(args, format);
    return __nprintf(out, maxlen, format, args, __str_put_func);
}

int __sprintf(PRINTF)(CHAR *out, const CHAR *format, ...) {
    va_list args;
    va_start(args, format);
    return __nprintf(out, (size_t)-1, format, args, __str_put_func);
}

#else

#define __vsprintf(printf)  concat(vs, printf)
#define __sprintf(printf)   concat(s, printf)

int __vsprintf(PRINTF)(CHAR *out, size_t bufsz, const CHAR *format, va_list args) {
    return __nprintf(out, bufsz, format, args, __str_put_func);
}

int __sprintf(PRINTF)(CHAR *out, size_t bufsz, const CHAR *format, ...) {
    va_list args;
    va_start(args, format);
    return __nprintf(out, bufsz, format, args, __str_put_func);
}

#endif

#define __vfprintf(printf)  concat(vf, printf)
#define __fprintf(printf)   concat(f, printf)
#define __vprintf(printf)   concat(v, printf)
#define __printf(printf)    printf

int __vfprintf(PRINTF)(FILE *stream, const CHAR *format, va_list args) {
    f_putbuf_index = 0;
    int result = __nprintf(stream, (size_t)-1, format, args, __f_put_func);
    if (result < 0)
        return -1;
    if (f_putbuf_index)
        flush_f_put_buf(stream);
    return result;
}

int __fprintf(PRINTF)(FILE *stream, const CHAR *format, ...) {
    va_list args;
    va_start(args, format);
    return __vfprintf(PRINTF)(stream, format, args);
}

int __vprintf(PRINTF)(const CHAR *format, va_list args) {
    return __vfprintf(PRINTF)(stdout, format, args);
}

int __printf(PRINTF)(const CHAR *format, ...) {
    va_list args;
    va_start(args, format);
    return __vfprintf(PRINTF)(stdout, format, args);
}

#undef NANSTRL
#undef NANSTRU
#undef INFSTRL
#undef INFSTRU
#undef __str_put_func
#undef __f_put_func