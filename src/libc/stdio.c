#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/stddef.h"
#include "include/ctype.h"
#include "include/tgmath.h"
#include "include/wchar.h"
#include "include/string.h"
#include <stdint.h>
#include "etc/syscalls.h"

typedef struct {
    short size_spec;
    short flags;
    int min_width;
    int precision;
    char spec;
} fmt_state_t;

#define signed_size_t long
#define uptrdiff_t unsigned long

#define FMT_LEFT_JUSTIFIED  1
#define FMT_ALWAYS_SIGN     2
#define FMT_PREPEND_SPACE   4
#define FMT_ALTERNATE       8
#define FMT_PAD_W_ZERO      16

short get_size_spec(const char *format, const char **newFormat) {
    if (!newFormat) return -1;
    *newFormat = format + 1;
    switch(*format) {
        case 'h':
            format++;
            if (*format == 'h') {
                *newFormat = format + 2;
                return 1;
            }
            return 2;
        case 'l':
            format++;
            if (*format == 'l') {
                *newFormat = format + 2;
                return 4;
            }
            return 3;
        case 'j': return 5;
        case 'z': return 6;
        case 't': return 7;
        case 'L': return 8;
        default:
            *newFormat = format;
            return 0;
    }
}


#define put(ch) {       \
    *(*str)++ = ch;     \
    *(chars_count)++;   \
}


#define is_int_spec(spec) (spec == 'd' || spec == 'i' || spec == 'u' || spec == 'o' || spec == 'x' || spec == 'X')

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
        char padding = state->flags & FMT_PAD_W_ZERO ? '0' : ' ';   \
        if (state->flags & FMT_LEFT_JUSTIFIED)                      \
            padding = ' ';                                          \
        if (is_int_spec(state->spec) && state->precision > 0)       \
            padding = ' ';                                          \
        if (state->flags & FMT_LEFT_JUSTIFIED) {                    \
            put_func;                                               \
            for (int i = 0; i < state->min_width - len; i++) {      \
                put(padding);                                       \
            }                                                       \
        } else {                                                    \
            for (int i = 0; i < state->min_width - len; i++) {      \
                put(padding);                                       \
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
        put('+');                                                   \
    }                                                               \
    else if (state->flags & FMT_PREPEND_SPACE && number >= 0) {     \
        put(' ');                                                   \
    }                                                               \
    char buf[precision + 50] = {};                                  \
    char *ptr = buf + precision + 49;                               \
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
        put(*ptr++);                                                \
    });


#define put_uint(number)    \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number)                                      \
        return 0;                                                   \
    char buf[precision + 50];                                       \
    char *_buf = buf + precision + 49;                              \
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
        put(*_buf++);                                               \
    });


#define put_oct_uint(number)                                        \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    else if (precision == -2)                                       \
        precision = 1;                                              \
    if (!precision && !number) {                                    \
        if (state->flags & FMT_ALTERNATE)                           \
            put('0');                                               \
        return 0;                                                   \
    }                                                               \
    char _buf[precision + 50];                                      \
    char *buf = _buf + precision + 49;                              \
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
        put(*buf++);                                                \
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
    char _buf[precision + 50];                                      \
    char *buf = _buf + precision + 49;                              \
    int len = 0;                                                    \
    int is_zero = number == 0;                                      \
    while (number) {                                                \
        char digit = number & 0x0F;                                 \
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
        put(*buf--);                                                \
    });


#define put_float(number)                                           \
    if (isnan(number)) {                                            \
        const char *nanstr = "nan";                                 \
        if (caps) {                                                 \
            nanstr = "NAN";                                         \
        }                                                           \
        while (*nanstr) {                                           \
            put(*nanstr++);                                         \
        }                                                           \
        return 0;                                                   \
    }                                                               \
    if (isinf(number)) {                                            \
        const char *infstr = "inf";                                 \
        if (caps) {                                                 \
            infstr = "INF";                                         \
        }                                                           \
        while (*infstr) {                                           \
            put(*infstr++);                                         \
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
        put('-');                                                   \
        number = -number;                                           \
    }                                                               \
    while (number > 1) {                                            \
        char digit = '0' + (int)(number / max_pow);                 \
        put(digit);                                                 \
        number = fmod(number, max_pow);                             \
        max_pow /= 10;                                              \
    }                                                               \
    if (precision == 0) {                                           \
        if (state->flags & FMT_ALTERNATE)                           \
            put('.');                                               \
    } else {                                                        \
    max_pow = pow(10, precision - 1);                               \
    number *= max_pow;                                              \
    number *= 10;                                                   \
    number = round(number);                                         \
    for (int i = 0; i < precision; i++) {                           \
        char digit = '0' + (int)(number / max_pow);                 \
        put(digit);                                                 \
        number = fmod(number, max_pow);                             \
        max_pow /= 10;                                              \
    }                                                               \
    }


// На кой чёрт выводить float в 16-ричной системе ?!?!?!?!??!?

#define put_hex_float(number)                                       \
    int precision = state->precision;                               \
    if (precision == -1)                                            \
        precision = va_arg(*args, int);                             \
    __typeof__(number) max_pow = 1;                                 \
    while (max_pow < number)                                        \
        max_pow *= 16;                                              \
    max_pow /= 16;                                                  \
    if (number < 0) {                                               \
        put('-');                                                   \
        number = -number;                                           \
    }                                                               \
    put('0');                                                       \
    if (caps) {                                                     \
        put('X');                                                   \
    } else {                                                        \
        put('x');                                                   \
    }                                                               \
    while (number > 1) {                                            \
        char digit = '0' + (int)(number / max_pow);                 \
        if (digit >= 0x3A) {                                        \
            digit += 7;                                             \
            if (!caps) digit += 0x20;                               \
        }                                                           \
        put(digit);                                                 \
        number = fmod(number, max_pow);                             \
        max_pow /= 16;                                              \
    }                                                               \
    if (precision == 0) {                                           \
        if (state->flags & FMT_ALTERNATE)                           \
            put('.');                                               \
    } else {                                                        \
    put('.');                                                       \
    if (precision > 0) {                                            \
        max_pow = pow(16, precision - 1);                           \
        number *= max_pow;                                          \
        number *= 16;                                               \
        number = round(number);                                     \
        for (int i = 0; i < precision; i++) {                       \
            char digit = '0' + (int)(number / max_pow);             \
            if (digit >= 0x3A) {                                    \
                digit += 7;                                         \
                if (!caps) digit += 0x20;                           \
            }                                                       \
            put(digit);                                             \
            number = fmod(number, max_pow);                         \
            max_pow /= 16;                                          \
        }                                                           \
    } else {                                                        \
        while (number > 0) {                                        \
            number *= 16;                                           \
            char digit = '0' + (int)(number / 1);                   \
            if (digit >= 0x3A) {                                    \
                digit += 7;                                         \
                if (!caps) digit += 0x20;                           \
            }                                                       \
            put(digit);                                             \
            number = fmod(number, 1);                               \
        }                                                           \
    }                                                               \
    }                                                               \

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
    if (caps) { put('E'); }                                         \
    else { put('e'); }                                              \
    if (exponent > 0) {                                             \
        put('+');                                                   \
    } else {                                                        \
        put('-');                                                   \
        exponent = -exponent;                                       \
    }                                                               \
    if (exponent < 10 && exponent > -10)                            \
        put('0');                                                   \
    int max_exp_pow = 1;                                            \
    while (max_exp_pow < exponent)                                  \
        max_exp_pow *= 10;                                          \
    max_exp_pow /= 10;                                              \
    while (exponent) {                                              \
        put('0' + (exponent / max_exp_pow) % 10);                   \
        exponent /= 10;                                             \
    }
    
int handle_char_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec == 0) {                                                        // Нет уточнения размера
        int ch = va_arg(*args, int);
        unsigned char c = (unsigned char)ch;
        put_w_padding(1, put(c));
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение "l"
        wint_t ch = va_arg(*args, wint_t);
        char buf[4] = {0, 0, 0, 0};
        mbstate_t mbstate = {};
        int char_sz = wcrtomb(buf, ch, &mbstate);
        char *_buf = buf;
        put_w_padding(char_sz, while (*_buf) {
            put(*_buf++);
        });
        return 0;
    }
    
    return -1;
}

int handle_str_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec == 0) {                                                        // Нет уточнения размера
        const char *string = va_arg(*args, const char*);
        size_t len = strlen(string);
        int stop = len;
        
        
        if (state->precision == -1) {
            stop = va_arg(*args, int);
        }
        else if (state->precision > 0)
            stop = state->precision;
        
        put_w_padding(stop, for (int i = 0; i < stop; i++) {
            put(string[i]);
        });
        
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение "l"
        const wchar_t *string = va_arg(*args, const wchar_t*);
        size_t len = wcslen(string);
        int stop = len;
        
        if (state->precision == -1) {
            stop = va_arg(*args, int);
        }
        else if (state->precision > 0)
            stop = state->precision;
        
        put_w_padding(stop, {for (int i = 0; i < stop; i++) {
        	mbstate_t mbstate = {};
            char buf[4] = {};
            char *_buf = buf;     
            size_t wc_len = wcrtomb(buf, string[i], &mbstate);
            while (*_buf) {
                put(*_buf++);
            }
        }});
        
        return 0;
    }
    
    return -1;
}

int handle_sint_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec == 0 || state->size_spec == 1 || state->size_spec == 2) {      // Уточнение размера "hh", "h", ""
        int number = va_arg(*args, int);
        put_int(number);
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение размера "l"
        long number = va_arg(*args, long);
        put_int(number);
        return 0;
    }
    
    if (state->size_spec == 4) {                                                        // Уточнение размера "ll"
        long long number = va_arg(*args, long long);
        put_int(number);
        return 0;
    }
    
    if (state->size_spec == 5) {                                                        // Уточнение размера "j"
        intmax_t number = va_arg(*args, intmax_t);
        put_int(number);
        return 0;
    }
    
    if (state->size_spec == 6) {                                                        // Уточнение размера "z"
        signed_size_t number = va_arg(*args, signed_size_t);
        put_int(number);
        return 0;
    }
    
    if (state->size_spec == 7) {                                                        // Уточнение размера "t"
        ptrdiff_t number = va_arg(*args, ptrdiff_t);
        put_int(number);
        return 0;
    }
    
    return -1;
}

int handle_uint_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec == 0 || state->size_spec == 1 || state->size_spec == 2) {      // Уточнение размера "hh", "h", ""
        unsigned int number = va_arg(*args, unsigned int);
        put_uint(number);
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение размера "l"
        unsigned long number = va_arg(*args, unsigned long);
        put_uint(number);
        return 0;
    }
    
    if (state->size_spec == 4) {                                                        // Уточнение размера "ll"
        unsigned long long number = va_arg(*args, unsigned long long);
        put_uint(number);
        return 0;
    }
    
    if (state->size_spec == 5) {                                                        // Уточнение размера "z"
        uintmax_t number = va_arg(*args, uintmax_t);
        put_uint(number);
        return 0;
    }
    
    if (state->size_spec == 6) {                                                        // Уточнение размера "t"
        size_t number = va_arg(*args, size_t);
        put_uint(number);
        return 0;
    }
    
    if (state->size_spec == 7) {
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_uint(number);
        return 0;
    }
    
    return -1;
}

int handle_oct_int_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec == 0 || state->size_spec == 1 || state->size_spec == 2) {      // Уточнение размера "hh", "h", ""
        unsigned int number = va_arg(*args, unsigned int);
        put_oct_uint(number);
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение размера "l"
        unsigned long number = va_arg(*args, unsigned long);
        put_oct_uint(number);
        return 0;
    }
    
    if (state->size_spec == 4) {                                                        // Уточнение размера "ll"
        unsigned long long number = va_arg(*args, unsigned long long);
        put_oct_uint(number);
        return 0;
    }
    
    if (state->size_spec == 5) {                                                        // Уточнение размера "j"
        uintmax_t number = va_arg(*args, uintmax_t);
        put_oct_uint(number);
        return 0;
    }
    
    if (state->size_spec == 6) {                                                        // Уточнение размера "z"
        size_t number = va_arg(*args, size_t);
        put_oct_uint(number);
        return 0;
    }
    
    if (state->size_spec == 7) {
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_oct_uint(number);
        return 0;
    }
    
    return -1;
}

int handle_hex_int_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count, int caps) {
    if (state->size_spec == 0 || state->size_spec == 1 || state->size_spec == 2) {      // Уточнение размера "hh", "h", ""
        unsigned int number = va_arg(*args, unsigned int);
        put_hex_uint(number);
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение размера "l"
        unsigned long number = va_arg(*args, unsigned long);
        put_hex_uint(number);
        return 0;
    }
    
    if (state->size_spec == 4) {                                                        // Уточнение размера "ll"
        unsigned long long number = va_arg(*args, unsigned long long);
        put_hex_uint(number);
        return 0;
    }
    
    if (state->size_spec == 5) {                                                        // Уточнение размера "j"
        uintmax_t number = va_arg(*args, uintmax_t);
        put_hex_uint(number);
        return 0;
    }
    
    if (state->size_spec == 6) {                                                        // Уточнение размера "z"
        size_t number = va_arg(*args, size_t);
        put_hex_uint(number);
        return 0;
    }
    
    if (state->size_spec == 7) {                                                        // Уточнение размера "t"
        uptrdiff_t number = va_arg(*args, uptrdiff_t);
        put_hex_uint(number);
        return 0;
    }
    
    return -1;
}

int handle_float_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count, int caps) {
    if (state->size_spec == 0 || state->size_spec == 4) {                               // Уточнение размера "", "l"
        double number = va_arg(*args, double);
        put_float(number);
        return 0;
    }
    
    if (state->size_spec == 8) {                                                        // Уточнение размера "L"
        long double number = va_arg(*args, long double);
        put_float(number);
        return 0;
    }
    
    return -1;
}

int handle_exp_flt_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count, int caps) {
    if (state->size_spec == 0 || state->size_spec == 4) {
        double number = va_arg(*args, double);
        
        put_exp_flt(number);
        
        return 0;
    }
    
    if (state->size_spec == 8) {                                                        // Уточнение размера "L"
        long double number = va_arg(*args, double);
        
        put_exp_flt(number);
        
        return 0;
    }
}

int handle_exp16_flt_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count, int caps) {
    if (state->size_spec == 0 || state->size_spec == 4) {
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
        put_hex_float(number);
        if (caps) { put('P'); }
        else { put('p'); }
        if (exponent > 0) {
            put('+');
        } else {
            put('-');
            exponent = -exponent;
        }
        
        if (exponent < 16 && exponent > -16)
            put('0');
        
        int max_exp_pow = 1;
        while (max_exp_pow < exponent)
            max_exp_pow <<= 4;
        max_exp_pow >>= 4;
            
        while (exponent) {
            char digit = '0' + (exponent / max_exp_pow);
            if (digit >= 0x3A) {
                digit += 7;
                if (!caps) digit += 0x20;
            }
            exponent >>= 4;
            max_exp_pow >>= 4;
        }
        
        return 0;
    }
    
    if (state->size_spec == 8) {                                                        // Уточнение размера "L"
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
        put_hex_float(number);
        if (caps) { put('P'); }
        else { put('p'); }
        if (exponent > 0) {
            put('+');
        } else {
            put('-');
            exponent = -exponent;
        }
        
        if (exponent < 16 && exponent > -16)
            put('0');
        
        int max_exp_pow = 1;
        while (max_exp_pow < exponent)
            max_exp_pow <<= 4;
        max_exp_pow >>= 4;
            
        while (exponent) {
            char digit = '0' + (exponent / max_exp_pow);
            if (digit >= 0x3A) {
                digit += 7;
                if (!caps) digit += 0x20;
            }
            exponent >>= 4;
            max_exp_pow >>= 4;
        }
        
        return 0;
    }
    
    return -1;
}

int handle_flt_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count, int caps) {
    if (state->size_spec == 0 || state->size_spec == 4) {                               // Уточнение размера "", "l"
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
            put_float(number);
        } else {
            state->precision = p - 1;
            put_exp_flt(number);
        }
        state->precision = _precision;
        return 0;
    }
    
    if (state->size_spec == 8) {                                                        // Уточнение размера "L"
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
            put_float(number);
        } else {
            state->precision = p - 1;
            put_exp_flt(number);
        }
        state->precision = _precision;
        return 0;
    }
    
    return -1;
}

int handle_cw_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    int chars = *chars_count;
    put_int(chars);
    
    if (state->size_spec == 1) {                                                        // Уточнение размера "hh"
        signed char *ptr = va_arg(*args, signed char*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 2) {                                                        // Уточнение размера "h"
        short *ptr = va_arg(*args, short*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 0) {                                                        // Нет уточнения размера
        int *ptr = va_arg(*args, int*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 3) {                                                        // Уточнение размера "l"
        long *ptr = va_arg(*args, long*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 4) {                                                        // Уточнение размера "ll"
        long long *ptr = va_arg(*args, long long*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 5) {                                                        // Уточнение размера "j"
        intmax_t *ptr = va_arg(*args, intmax_t*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 6) {                                                        // Уточнение размера "z"
        signed_size_t *ptr = va_arg(*args, signed_size_t*);
        *ptr = chars;
        return 0;
    }
    
    if (state->size_spec == 7) {                                                        // Уточнение размера "t"
        ptrdiff_t *ptr = va_arg(*args, ptrdiff_t*);
        *ptr = chars;
        return 0;
    }
    
    return -1;
}

int handle_ptr_fmtspec(char **str, fmt_state_t *state, va_list *args, int *chars_count) {
    if (state->size_spec)
        return -1;
    
    void *ptr = va_arg(*args, void*);
    long ptrint = (long)ptr;
    int caps = 0;
    
    put_hex_uint(ptrint);
    
    return 0;
}

int handle_format(char **str, fmt_state_t *state, int *chars_count, va_list *args) {
    switch (state->spec) {
        case 'c': return handle_char_fmtspec(str, state, args, chars_count);
        case 's': return handle_str_fmtspec(str, state, args, chars_count);
        case 'd': return handle_sint_fmtspec(str, state, args, chars_count);
        case 'i': return handle_sint_fmtspec(str, state, args, chars_count);
        case 'u': return handle_uint_fmtspec(str, state, args, chars_count);
        case 'o': return handle_oct_int_fmtspec(str, state, args, chars_count);
        case 'x': return handle_hex_int_fmtspec(str, state, args, chars_count, 0);
        case 'X': return handle_hex_int_fmtspec(str, state, args, chars_count, 1);
        case 'f': return handle_float_fmtspec(str, state, args, chars_count, 0);
        case 'F': return handle_float_fmtspec(str, state, args, chars_count, 1);
        case 'e': return handle_exp_flt_fmtspec(str, state, args, chars_count, 0);
        case 'E': return handle_exp_flt_fmtspec(str, state, args, chars_count, 1);
        case 'a': return handle_exp16_flt_fmtspec(str, state, args, chars_count, 0);
        case 'A': return handle_exp16_flt_fmtspec(str, state, args, chars_count, 1);
        case 'g': return handle_flt_fmtspec(str, state, args, chars_count, 0);
        case 'G': return handle_flt_fmtspec(str, state, args, chars_count, 1);
        case 'n': return handle_cw_fmtspec(str, state, args, chars_count);
        case 'p': return handle_ptr_fmtspec(str, state, args, chars_count);
        default: return -1;
    }
}

int sprintf(char *out, const char *format, ...) {
    fmt_state_t state;
    va_list args;
    va_start(args, format);
    int chars_count = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == '%') {
                *out++ = '%';
                format ++;
                chars_count ++;
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
            else if (isdigit(*format)) {
                min_width = (int)(strtol(format, (char**)&format, 10));
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
                    precision = (int)(strtol(format, (char**)&format, 10));
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
            
            int fmt_result = handle_format(&out, &state, &chars_count, &args);
            
            if (fmt_result)         // Произошла ошибка при обработке формата
                return fmt_result;
            
            continue;
        }
        
        *out++ = *format++;
        chars_count++;
    }
    
    va_end(args);
    return chars_count;
}