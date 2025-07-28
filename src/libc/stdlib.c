#include "include/stdlib.h"

#include "etc/syscalls.h"
#include "include/string.h"
#include "include/math.h"
#include "include/locale.h"
#include "include/ctype.h"
#include "include/limits.h"

#define EX_CODE_ABORT -1

typedef void (*atexit_func_t)();
atexit_func_t *atexit_funcs = NULL;
unsigned int atexit_func_cnt = 0;

#define CHAR_T char
#define ISSPACE         isspace
#define ISDIGIT         isdigit
#define ISXDIGIT        isxdigit
#define ISLOWER         islower
#define TOLOWER         tolower
#define TOUPPER         toupper
#define STRSTR          strstr
#define STRLEN          strlen
#define STRPBRK         strpbrk
#define STRTO           strto
#include "./.no-compile/strtonum_base.c"

void abort() {
    syscall1(0, EX_CODE_ABORT);
}

void exit(int code) {
    for (unsigned int i = 0; i < atexit_func_cnt; i++) {
        atexit_funcs[i]();
    }

	syscall1(0, code);
}

int atexit(void (*func)(void)) {
    if (!atexit_funcs) {
        atexit_funcs = malloc(sizeof func);
        if (!atexit_funcs)
            return -1;
        *atexit_funcs = func;
        return 0;
    }
    atexit_func_t *new_atexit_funcs = realloc(atexit_funcs, sizeof func * (++atexit_func_cnt));
    if (!new_atexit_funcs) {
        atexit_func_cnt --;
        return -1;
    }
    atexit_funcs = new_atexit_funcs;
    atexit_funcs[atexit_func_cnt - 1] = func;
    return 0;
}

int system(const char *cmd) {
    // TODO: Обработчик команд в ОС
    return syscall1(0x402, (int)cmd);
}

char* getenv(const char *var_name) {
    // TODO: Окружение программы
    return (char*)syscall1(0x403, (int)var_name);
}

void* malloc(size_t sz) {
    return (void*)syscall1(0x400, (int)sz);
}

void free(void *ptr) {
    syscall1(0x401, (int)ptr);
}

void* calloc(size_t count, size_t sz) {
    void *ptr = malloc(count * sz);
    if (!ptr) return NULL;
    memset(ptr, 0, count * sz);
    return ptr;
}

void* realloc(void *ptr, size_t new_sz) {
    void *new_ptr = malloc(new_sz);
    if (!new_ptr) return NULL;
    if (ptr) memcpy(new_ptr, ptr, new_sz);
    free(ptr);
    return new_ptr;
}

double atof(const char *str) {
    return strtod(str, NULL);
}

int atoi(const char *str) {
    return (int)strtol(str, NULL, 10);
}

long atol(const char *str) {
    return strtol(str, NULL, 10);
}

long long atoll(const char *str) {
    return strtoll(str, NULL, 10);
}

#define utf8_follow_char(c) (((c) & 0b10000000) == 0)

int mblen(const char *s, size_t n) {
    if (!s) return 0;

    char c = *s;
    if (!c) return 0;

    if ((c & 0b11000000) == 0b10000000)
        return -1;

    if (utf8_follow_char(c))
        return 1;

    if ((c & 0b11100000) == 0b11000000) {
        if (n < 2)
            return -1;
        s++;
        if (utf8_follow_char(s[0]))
            return 2;
        else return -1;
    }

    if ((c & 0b11110000) == 0b11100000) {
        if (n < 3)
            return -1;
        s++;
        if (utf8_follow_char(s[0]) && utf8_follow_char(s[1]))
            return 3;
        else return -1;
    }

    if ((c & 0b11111000) == 0b11110000) {
        if (n < 4)
            return -1;
        s++;
        if (utf8_follow_char(s[0]) && utf8_follow_char(s[1]) && utf8_follow_char(s[2]))
            return 4;
        else return -1;
    }

    return -1;
}

int mbtowc(wchar_t *wc, const char *s, size_t n) {
    size_t sz = mblen(s, n);
    if (sz > n) return -1;          // sz == -1 (sz беззнаковое и должно быть меньше n)

    wchar_t wch;
    if (sz == 0) {
        if (wc) *wc = 0;
        return 0;
    }

    if (sz == 1) {
        wch = *s & 0x7F;
        if (wc) *wc = wch;
        return 1;
    }

    if (sz == 2) {
        wch = *s & 0x1F;
        wch <<= 6;
        wch |= *s & 0x3F;
        if (wc) *wc = wch;
        return 1;
    }

    if (sz == 3) {
        wch = *s & 0x0F;
        for (int i = 0; i < 2; i++) {
            wch <<= 6;
            wch |= *s & 0x3F;
        }
        if (wc) *wc = wch;
        return 1;
    }

    if (sz == 4) {
        wch = *s & 0x07;
        for (int i = 0; i < 2; i++) {
            wch <<= 6;
            wch |= *s & 0x3F;
        }
        if (wc) *wc = wch;
        return 1;
    }

    return -1;
}

int wctomb(char *s, wchar_t wc) {
    if (!s)
        return 0;

    size_t sz;

    if (wc < 0x80)
        sz = 1;
    else if (wc < 0x800)
        sz = 2;
    else if (wc < 0x10000)
        sz = 3;
    else if (wc < 0x110000)
        sz = 4;
    else return -1;             // Не поместится в UTF-8-символ

    int shift;
    switch (sz) {
        case 1:
            *s = wc;
            return 1;
        case 2:
            *s++ = 0xC0 | (wc & 0x1F);
            shift = 0;
            break;
        case 3:
            *s++ = 0xE0 | (wc & 0x0F);
            shift = 6;
            break;
        case 4:
            *s++ = 0xF0 | (wc & 0x07);
            shift = 12;
            break;
    }

    for (size_t i = 0; i < sz - 1; i++) {
        wchar_t mask = 0b111111 << shift;
        *s++ = 0b10000000 | ((wc & mask) >> shift);
        shift -= 6;
    }

    return sz;
}

size_t mbstowcs(wchar_t *wstr, const char *str, size_t len) {
    size_t count = 0;

    while (*str) {
        wchar_t wc;
        size_t sz = mbtowc(&wc, str, MB_LEN_MAX);
        if (sz > MB_LEN_MAX) return -1;

        if (count < len) {
            *wstr++ = wc;
            count++;
            str += sz;
        }
    }

    return count;
}

size_t wcstombs(char *str, const wchar_t *wstr, size_t len) {
    size_t count = 0;

    while (*wstr) {
        wchar_t wc = *wstr++;
        char buf[MB_LEN_MAX];
        size_t sz = wctomb(buf, wc);
        for (size_t i = 0; i < sz; i++) {
            if (i + count == len) {
                count += i;
                return count;
            }
            *str++ = buf[i];
        }
        count += sz;
    }

    return count;
}

int __seed = 1;

void srand(unsigned int seed) {
    __seed = (int)seed;
}

int rand() {
    // Вы умеете генерировать случайные числа? Я вот нет.
    // А компьютер надо научить. Ну, давайте попробуем...

    int rnd = __seed * (__seed + 13213) * (__seed + 1001) * (__seed - 19919);

    // ...сначала вот так...
    rnd = (rnd * 48193248124) + (rnd * 91210) * ((rnd & 134232) * 9109301);

    // ...потом вот так...
    rnd = (rnd / 19283) * (rnd / 1932031) + (rnd << 2) / 11111;

    // ...ну, закончим как-нибудь вот так...
    rnd = (rnd * rnd * rnd * rnd) / 1923123 + (rnd * rnd / 9124580459) - (rnd * rnd * rnd) * 1010;

    __seed = rnd;

    // Возможно, сработает.
    return rnd;
}

void qsort(void *array, size_t cnt, size_t sz, int (*cmp)(void*, void*)) {
    if (cnt == 1)
        return;

    int pivot_index = (cnt / 2) + 1;
    void *pivot = malloc(sz);
    memcpy(pivot, array + pivot_index * sz, sz);
    void *before_pivot;
    void *after_pivot;
    int before_cnt = 0, after_cnt = 0;

    for (int i = 0; i < cnt; i++) {
        if (i == pivot_index) continue;

        void *element = array + i * sz;
        int cmp_result = cmp(element, pivot);
        if (cmp_result < 0) {
            if (!before_cnt) {
                before_pivot = malloc(sz);
                memcpy(before_pivot, element, sz);
            } else {
                before_pivot = realloc(before_pivot, (before_cnt + 1) * sz);
                memcpy(before_pivot + before_cnt * sz, element, sz);
            }
            before_cnt++;
        } else {
            if (!after_cnt) {
                after_pivot = malloc(sz);
                memcpy(after_pivot, element, sz);
            } else {
                after_pivot = realloc(after_pivot, (after_cnt + 1) * sz);
                memcpy(after_pivot + after_cnt * sz, element, sz);
            }
            after_cnt++;
        }
    }

    if (before_cnt) {
        qsort(before_pivot, before_cnt, sz, cmp);
        memcpy(array, before_pivot, before_cnt * sz);
        free(before_pivot);
    }
    if (after_cnt) {
        qsort(after_pivot, after_cnt, sz, cmp);
        memcpy(array + (before_cnt + 1) * sz, after_pivot, after_cnt * sz);
        free(after_pivot);
    }

    memcpy(array + before_cnt * sz, pivot, sz);
}

void* bsearch(const void *key, const void *base, size_t cnt, size_t sz, int (*cmp)(void*, void*)) {
    size_t left = 0, right = cnt - 1;
    while (left != right) {
        size_t middle = (left + right) / 2;
        void *element = (void*)base + middle * sz;
        int cmp_result = cmp(element, (void*)key);
        if (!cmp_result)
            return element;
        if (cmp_result == 1) {
            right = middle;
        } else {
            left = middle;
        }
    }
    return NULL;
}

#define abs_code(_INT) {    \
    return x < 0 ? -x : x;  \
}

int abs(int x) abs_code(int)
long labs(long x) abs_code(long)
long long llabs(long long x) abs_code(long long)

#undef abs_code

#define div_code(_INT, _DIV) {      \
    return (_DIV){x / y, x % y};    \
}

div_t div(int x, int y) div_code(int, div_t)
ldiv_t ldiv(long x, long y) div_code(long, ldiv_t)
lldiv_t lldiv(long long x, long long y) div_code(long long, lldiv_t)

#undef div_code