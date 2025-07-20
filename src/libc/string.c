/*
 * string.c
 *
 *
 */

#include "include/string.h"
#include "include/stdlib.h"
#include "include/ctype.h"
#include "include/errno.h"
#include "etc/errmsgs.h"
#include "locales/ext_lconv.h"
#include "locales/c_locale.h"

#define always_inline __attribute__((always_inline)) inline

/* Вспомогательные функции для UTF-8 */

always_inline size_t get_utf8_char_len(const char *str) {
    if ((*str & 0x80) == 0)
        return 1;
    if ((*str & 0xE0) == 0xC0)
        return 2;
    if ((*str & 0xF0) == 0xE0)
        return 3;
    if ((*str & 0xF8) == 0xF0)
        return 4;
    return 0;
}

always_inline int get_utf8_char(const char *str) {
    int c;
    char *buf = (char*)&c;
    size_t charSz = get_utf8_char_len(str);
    for (size_t i = 0; i < charSz; i++) {
        buf[i] = *str++;
    }
    return c;
}

always_inline size_t write_utf8_char(int c, char **dest, size_t max) {
    if (c > 0x10FFFF)
        return 0;

    char utf8_buf[4] = {0, 0, 0, 0};
    size_t cnt;

    if (c < 0x80) {
        utf8_buf[0] = c;
        cnt = 1;
    }
    else if (c < 0x800) {
        utf8_buf[0] = 0b11000000 | ((c & 0x7C0) >> 6);
        utf8_buf[1] = 0b10000000 | (c & 0x3F);
        cnt = 2;
    }
    else if (c < 0x10000) {
        utf8_buf[0] = 0b11100000 | ((c & 0xF000) >> 12);
        utf8_buf[1] = 0b10000000 | ((c & 0xFC0) >> 6);
        utf8_buf[2] = 0b10000000 | (c & 0x3F);
        cnt = 3;
    }
    else {
        utf8_buf[0] = 0b11110000 | ((c & 0x1C0000) >> 18);
        utf8_buf[1] = 0b10000000 | ((c & 0x3F000) >> 12);
        utf8_buf[2] = 0b10000000 | ((c & 0xFC0) >> 6);
        utf8_buf[3] = 0b10000000 | (c & 0x3F);
        cnt = 4;
    }

    if (!*dest)
        return cnt;

    if (max) {
        size_t write_cnt = 0;
        for (size_t i = 0; i < cnt && i < max; i++) {
            **dest++ = utf8_buf[i];
            write_cnt++;
        }
        return write_cnt;
    } else {
        for (size_t i = 0; i < cnt; i++) {
            **dest++ = utf8_buf[i];
        }
        return cnt;
    }
}

/* **** */

char* strcpy(char *dest, const char *src) {
    char *_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = 0;
    return _dest;
}

char* strncpy(char *dest, const char *src, size_t n) {
    char *_dest = dest;
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        *dest++ = src[i];
    }
    while (i < n) {
        *dest++ = 0;
        i++;
    }
    return _dest;
}

char* strcat(char *dest, const char *src) {
    char *_dest = dest;
    while (*dest)
        dest++;
    while (*src)
        *dest++ = *src++;
    *dest = 0;
    return _dest;
}

char* strncat(char *dest, const char *src, size_t n) {
    char *_dest = dest;
    while (*dest)
        dest++;
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        *dest++ = src[i];
    }
    *dest = 0;
    return _dest;
}

char* strdup(const char *str) {
    size_t len = strlen(str);

    char *new_str = malloc(len + 1);
    if (!new_str) return NULL;

    strcpy(new_str, str);
    return new_str;
}

char* strndup(const char *str, size_t n) {
    size_t len = strlen(str);
    if (len > n)
        len = n;

    char *new_str = malloc(len + 1);
    if (!new_str) return NULL;

    strcpy(new_str, str);
    return new_str;
}

/* strcoll */

// TODO: Сделать более детальное сравнение (регистр, диакритики и т.д.)

typedef struct {
    const char *str;
    int index;
    int byte_len;
    int char_len;
} coll_t;

// FIXME: Вообще что-то не святое, это по-любому не работает

always_inline coll_t get_next_coll_seq(const char *str) {
    char seq_buf[16];
    memset(seq_buf, 0, sizeof seq_buf);

    size_t sz = 0;
    int chars = 0;
    int prev_seq_count = 0;
    int eq_index = 0;
    const char *_str = str;

    while (*str) {
        size_t charSz = get_utf8_char_len(str);
        int c = get_utf8_char(str);
        c = tolower(c);
        char *buf = (char*)&c;
        for (size_t i = 0; i < charSz; i++) {
            seq_buf[i + sz] = *buf++;
        }
        sz += charSz;
        str += charSz;
        char **colls = locale.coll.order;
        int seqs_count = 0;
        int index = 0;
        for (int i = 0; colls[i]; i++) {
            char *coll = colls[i];
            if (strstr(coll, seq_buf) == coll) {
                seqs_count++;
                index = i;
            }
            if (strcmp(coll, seq_buf) == 0) {
                eq_index = i;
            }
        }
        if (seqs_count == 1) {
            return (coll_t){_str, sz, index, chars};
        }
        if (seqs_count == 0 && prev_seq_count > 0) {
            return (coll_t){_str, sz - charSz, eq_index, chars - 1};
        }
        if (seqs_count == 0 && prev_seq_count == 0) {
            return (coll_t){NULL, -1, -1, -1};
        }
        prev_seq_count = seqs_count;
        chars ++;
    }

    return (coll_t){NULL, -1, -1, -1};
}

always_inline int compare_colls(coll_t coll1, coll_t coll2) {
    if (coll1.index > coll2.index)
        return 1;
    if (coll2.index > coll1.index)
        return -1;
    int shift = 0;
    for (int i = 0; i < coll1.char_len; i++) {
        int c1 = get_utf8_char(coll1.str + shift);
        int c2 = get_utf8_char(coll2.str + shift);
        shift += get_utf8_char_len(coll1.str + shift);
        if (isupper(c1) && islower(c2))
            return 1;
        if (islower(c1) && isupper(c2))
            return -1;
    }
    return 0;
}

int strcoll(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        int c1 = *str1;
        int c2 = *str2;
        coll_t coll1 = get_next_coll_seq(str1);
        coll_t coll2 = get_next_coll_seq(str2);
        if (!coll1.str || !coll2.str) {
            if (c1 > c2)
                return 1;
            if (c2 > c1)
                return -1;

            if (coll1.str) str1 += coll1.byte_len;
            else str1++;

            if (coll2.str) str2 += coll2.byte_len;
            else str2++;
        }
        int cmp_res = compare_colls(coll1, coll2);
        if (cmp_res)
            return cmp_res;
    }
    return *str2 ? -1 : 0;
}

/* **** */

size_t strxfrm(char *dest, const char *src, size_t count) {
    if (count != 0 && !dest)
        return -1;

    if (memcmp(&locale, &C_LOCALE, sizeof locale) == 0) {
        strncpy(dest, src, count);
        return count ? count : strlen(dest);
    }

    size_t written = 0;
    while (*src) {
        coll_t coll = get_next_coll_seq(src);

        // TODO: Исправить трансформацию: добавить поддержку чувствительности регистра, диакретиков (Ё или Е) и т.д.
        int c = coll.index;

        size_t add = write_utf8_char(c, &dest, count ? (count - written - 1) : 0);
        written += add;
        if (written >= count) break;
    }

    return written;
}

size_t strlen(const char *str) {
    if (!str)
        return -1;

    size_t len = 0;
    while (*str) {
        str++;
        len++;
    }
    return len;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        int c1 = *str1++;
        int c2 = *str2++;
        if (c1 > c2) return 1;
        if (c1 < c2) return -1;
    }
    return *str2 ? -1 : 0;
}

int strncmp(const char *str1, const char *str2, size_t count) {
    size_t i;
    for (i = 0; i < count && str1[i] && str2[i]; i++) {
        int c1 = str1[i];
        int c2 = str2[i];
        if (c1 > c2) return 1;
        if (c1 < c2) return -1;
    }
    if (i == count) return 0;
    return str2[i] ? -1 : 0;
}

char* strchr(const char *str, int ch) {
    while (*str) {
        if (*str == (char)ch)
            return (char*)str;
        str++;
    }
    return NULL;
}

char* strrchr(const char *str, int ch) {
    const char *end = str;
    while (*end)
        end++;
    end--;
    while (end > str) {
        if (*end == (char)ch)
            return (char*)end;
        end--;
    }
    return NULL;
}

size_t strspn(const char *str, const char *chars) {
    size_t sz = 0;
    while (*str) {
        char ch = *str;
        if (strchr(chars, ch)) {
            sz ++;
            str++;
        } else break;
    }
    return sz;
}

size_t strcspn(const char *str, const char *chars) {
    size_t sz = 0;
    while (*str) {
        char ch = *str;
        if (strchr(chars, ch)) break;
        str++;
        sz++;
    }
    return sz;
}

char* strpbrk(const char *str, const char *breaks) {
    while (*breaks) {
        char ch = *breaks++;
        char *brk = strchr(str, ch);
        if (brk) return brk;
    }
    return NULL;
}

char* strstr(const char *str, const char *substr) {
    size_t len = strlen(str);
    size_t sublen = strlen(substr);

    if (sublen > len)
        return NULL;

    char buf[sublen + 3];
    strncpy(buf + 1, str, sublen);
    str += sublen;

    while (*str) {
        if (strcmp(buf + 1, substr) == 0)
            return (char*)str - sublen;
        for (int i = 0; i < (int)(sizeof buf) - 3; i++) {
            buf[i] = buf[i + 1];
        }
        buf[sublen] = *str;
        str++;
    }

    if (strcmp(buf + 1, substr) == 0)
        return (char*)str - sublen;

    return NULL;
}

char *__strtok_internal_state = NULL;

char* strtok(char *str, const char *delim) {
    if (!str)
        str = __strtok_internal_state;

    size_t cspn = strcspn(str, delim);
    if (cspn == strlen(str))
        return NULL;

    str[cspn - 1] = 0;
    __strtok_internal_state = str + cspn;
    return str;
}

/* mem-функции */

void* memchr(const void *ptr, int ch, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (((unsigned char*)ptr)[i] == (unsigned char)ch)
            return (void*)ptr + i;
    }
    return NULL;
}

int memcmp(const void *ptr1, const void *ptr2, size_t count) {
    unsigned char *buf1 = (unsigned char*)ptr1;
    unsigned char *buf2 = (unsigned char*)ptr2;
    for (size_t i = 0; i < count; i++) {
        unsigned char c1 = buf1[i];
        unsigned char c2 = buf2[i];
        if (c1 > c2)
            return 1;
        if (c1 < c2)
            return -1;
    }
    return 0;
}


void* memset(void *ptr, int c, size_t count) {
    unsigned char *buf = (unsigned char*)ptr;
    for (size_t i = 0; i < count; i++) {
        buf[i] = c;
    }
    return buf;
}

// Мы выключаем оптимизацию dead-code-removal для этой функции, потому что, как оказалось,
// оптимизатор GCC может удалить вызов этой функции, а это может сломать безопасность
// некоторых программ. Таким образом, GCC гарантирует генерацию вызова этой функции.

__attribute__((optimize ("no-dce")))
void* memset_explicit(void *ptr, int c, size_t count) {
    unsigned char *buf = (unsigned char*)ptr;
    for (size_t i = 0; i < count; i++) {
        buf[i] = c;
    }
    return buf;
}

void* memcpy(void *dest, const void *src, size_t count) {
    for (size_t i = 0; i < (count >> 2); i++) {
        ((int*)dest)[i] = ((int*)src)[i];
    }
    for (size_t i = 0; i < (count & 3); i++) {
        ((char*)dest)[i] = ((int*)src)[i];
    }
    return dest;
}

void* memmove(void *dest, const void *src, size_t count) {
    void *buf = malloc(count);
    memcpy(buf, src, count);
    memcpy(dest, buf, count);
    free(buf);
    return dest;
}

void* memccpy(void* restrict dest, const void* restrict src, int ch, size_t count) {
    unsigned char c = (unsigned char)ch;
    for (size_t i = 0; i < count; i++) {
        unsigned char b = ((unsigned char*)src)[i];
        ((unsigned char*)dest)[i] = b;
        if (b == c) break;
    }
    return dest;
}

#define errcase(err) case err: return (char*)STR_##err;

char* strerror(int errnum) {
    switch (errnum) {
        errcase(EDOM)
        errcase(ERANGE)
        errcase(EILSEQ)
    }
    return NULL;
}