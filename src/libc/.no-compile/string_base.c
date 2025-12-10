/*
 * string_base.c
 *
 *
 */

#include "../include/stdlib.h"
#include "../locales/ext_lconv.h"

#ifndef CHAR_T

#define CHAR_T                  char
#define SINGLE_CHAR_T           int
#define COLL_T                  coll_t
#define VOID_T                  void

#define GET_CHAR_LEN_FUNC       get_utf8_char_len
#define GET_CHAR_FUNC           get_utf8_char
#define WRITE_CHAR_FUNC         write_utf8_char

#define STRCPY_FUNC             strcpy
#define STRNCPY_FUNC            strncpy
#define STRCAT_FUNC             strcat
#define STRNCAT_FUNC            strncat
#define STRDUP_FUNC             strdup
#define STRNDUP_FUNC            strndup
#define STRCOLL_FUNC            strcoll
#define STRXFRM_FUNC            strxfrm
#define STRLEN_FUNC             strlen
#define STRCMP_FUNC             strcmp
#define STRNCMP_FUNC            strncmp
#define STRCHR_FUNC             strchr
#define STRRCHR_FUNC            strrchr
#define STRSPN_FUNC             strspn
#define STRCSPN_FUNC            strcspn
#define STRPBRK_FUNC            strpbrk
#define STRSTR_FUNC             strstr
#define MEMCHR_FUNC             memchr
#define MEMCMP_FUNC             memcmp
#define MEMSET_FUNC             memset
#define MEMSET_EXPLICIT_FUNC    memset_explicit
#define MEMCPY_FUNC             memcpy
#define MEMMOVE_FUNC            memmove
#define MEMCCPY_FUNC            memccpy
#define STRERROR_FUNC           strerror

#endif

#define always_inline __attribute__((always_inline)) inline

/* Вспомогательные функции для UTF-8 */

size_t get_utf8_char_len(const char *str) {
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

always_inline SINGLE_CHAR_T get_utf8_char(const char *str) {
    int c;
    char *buf = (char*)&c;
    size_t char_sz = get_utf8_char_len(str);
    for (size_t i = 0; i < char_sz; i++) {
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

/* Вспомогательные функции для UTF-32 */

always_inline size_t get_utf32_char_len(const wchar_t *wstr) {
    // Одно знаковое 32-битное число (int) вмещает в себя
    // 4 294 967 296 различных значений, тогда как всего символов
    // в Юникоде не больше 1 112 064. Это означает, что любой символ Юникода
    // кодируется всего одним 32-битным числом, поэтому размер
    // символа в UTF-32 всегда 1.
    return 1;
}

always_inline wchar_t get_utf32_char(const wchar_t *wstr) {
    // Так как размер одного символа в UTF-32 всегда 1,
    // нам достаточно просто считать число по указателю
    // строки.
    return *wstr;
}

always_inline size_t write_utf32_char(wchar_t wc, wchar_t **wstr, size_t max) {
    if (max) {
        *(*wstr)++ = wc;
        return 1;
    } else
        return 0;
}

/* **** */

CHAR_T* STRCPY_FUNC(CHAR_T *dest, const CHAR_T *src) {
    CHAR_T *_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = 0;
    return _dest;
}

CHAR_T* STRNCPY_FUNC(CHAR_T *dest, const CHAR_T *src, size_t n) {
    CHAR_T *_dest = dest;
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

CHAR_T* STRCAT_FUNC(CHAR_T *dest, const CHAR_T *src) {
    CHAR_T *_dest = dest;
    while (*dest)
        dest++;
    while (*src)
        *dest++ = *src++;
    *dest = 0;
    return _dest;
}

CHAR_T* STRNCAT_FUNC(CHAR_T *dest, const CHAR_T *src, size_t n) {
    CHAR_T *_dest = dest;
    while (*dest)
        dest++;
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        *dest++ = src[i];
    }
    *dest = 0;
    return _dest;
}

CHAR_T* STRDUP_FUNC(const CHAR_T *str) {
    size_t len = STRLEN_FUNC(str);

    CHAR_T *new_str = malloc(len + 1);
    if (!new_str) return NULL;

    STRCPY_FUNC(new_str, str);
    return new_str;
}

CHAR_T* STRNDUP_FUNC(const CHAR_T *str, size_t n) {
    size_t len = STRLEN_FUNC(str);
    if (len > n)
        len = n;

    CHAR_T *new_str = malloc(len + 1);
    if (!new_str) return NULL;

    STRCPY_FUNC(new_str, str);
    return new_str;
}

/* strcoll */

// TODO: Сделать более детальное сравнение (регистр, диакритики и т.д.)

typedef struct {
    const CHAR_T *str;
    int index;
    int byte_len;
    int char_len;
} COLL_T;

// FIXME: Вообще что-то не святое, это по-любому не работает

always_inline COLL_T get_next_coll_seq(const CHAR_T *str) {
    CHAR_T seq_buf[16];
    MEMSET_FUNC(seq_buf, 0, sizeof seq_buf);

    size_t sz = 0;
    SINGLE_CHAR_T char_s = 0;
    SINGLE_CHAR_T prev_seq_count = 0;
    SINGLE_CHAR_T eq_index = 0;
    const CHAR_T *_str = str;

    while (*str) {
        size_t char_sz = GET_CHAR_LEN_FUNC(str);
        SINGLE_CHAR_T c = GET_CHAR_FUNC(str);
        c = TOLOWER(c);
        CHAR_T *buf = (CHAR_T*)&c;
        for (size_t i = 0; i < char_sz; i++) {
            seq_buf[i + sz] = *buf++;
        }
        sz += char_sz;
        str += char_sz;
        CHAR_T **colls = (CHAR_T**)locale.coll.order;
        SINGLE_CHAR_T seqs_count = 0;
        SINGLE_CHAR_T index = 0;
        for (int i = 0; colls[i]; i++) {
            CHAR_T *coll = colls[i];
            if (STRSTR_FUNC(coll, seq_buf) == coll) {
                seqs_count++;
                index = i;
            }
            if (STRCMP_FUNC(coll, seq_buf) == 0) {
                eq_index = i;
            }
        }
        if (seqs_count == 1) {
            return (COLL_T){_str, sz, index, char_s};
        }
        if (seqs_count == 0 && prev_seq_count > 0) {
            return (COLL_T){_str, sz - char_sz, eq_index, char_s - 1};
        }
        if (seqs_count == 0 && prev_seq_count == 0) {
            return (COLL_T){NULL, -1, -1, -1};
        }
        prev_seq_count = seqs_count;
        char_s ++;
    }

    return (COLL_T){NULL, -1, -1, -1};
}

always_inline SINGLE_CHAR_T compare_colls(COLL_T coll1, COLL_T coll2) {
    if (coll1.index > coll2.index)
        return 1;
    if (coll2.index > coll1.index)
        return -1;
    SINGLE_CHAR_T shift = 0;
    for (int i = 0; i < coll1.char_len; i++) {
        SINGLE_CHAR_T c1 = GET_CHAR_FUNC(coll1.str + shift);
        SINGLE_CHAR_T c2 = GET_CHAR_FUNC(coll2.str + shift);
        shift += GET_CHAR_LEN_FUNC(coll1.str + shift);
        if (ISUPPER(c1) && ISLOWER(c2))
            return 1;
        if (ISLOWER(c1) && ISUPPER(c2))
            return -1;
    }
    return 0;
}

SINGLE_CHAR_T STRCOLL_FUNC(const CHAR_T *str1, const CHAR_T *str2) {
    while (*str1 && *str2) {
        SINGLE_CHAR_T c1 = *str1;
        SINGLE_CHAR_T c2 = *str2;
        COLL_T coll1 = get_next_coll_seq(str1);
        COLL_T coll2 = get_next_coll_seq(str2);
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

size_t STRXFRM_FUNC(CHAR_T *dest, const CHAR_T *src, size_t count) {
    if (count != 0 && !dest)
        return -1;

    if (!*(localeconv()->grouping)) {               // Используется локаль "C"
        STRNCPY_FUNC(dest, src, count);
        return count ? count : STRLEN_FUNC(dest);
    }

    size_t written = 0;
    while (*src) {
        COLL_T coll = get_next_coll_seq(src);

        // TODO: Исправить трансформацию: добавить поддержку чувствительности регистра, диакритиков (Ё или Е) и т.д.
        SINGLE_CHAR_T c = coll.index + 1;

        size_t add = WRITE_CHAR_FUNC(c, &dest, count ? (count - written - 1) : 0);
        written += add;
        if (written >= count) break;
    }

    return written;
}

size_t STRLEN_FUNC(const CHAR_T *str) {
    if (!str)
        return -1;

    size_t len = 0;
    while (*str) {
        str++;
        len++;
    }
    return len;
}

int STRCMP_FUNC(const CHAR_T *str1, const CHAR_T *str2) {
    while (*str1 && *str2) {
        SINGLE_CHAR_T c1 = *str1++;
        SINGLE_CHAR_T c2 = *str2++;
        if (c1 > c2) return 1;
        if (c1 < c2) return -1;
    }
    return *str2 ? -1 : 0;
}

int STRNCMP_FUNC(const CHAR_T *str1, const CHAR_T *str2, size_t count) {
    size_t i;
    for (i = 0; i < count && str1[i] && str2[i]; i++) {
        SINGLE_CHAR_T c1 = str1[i];
        SINGLE_CHAR_T c2 = str2[i];
        if (c1 > c2) return 1;
        if (c1 < c2) return -1;
    }
    if (i == count) return 0;
    return str2[i] ? -1 : 0;
}

CHAR_T* STRCHR_FUNC(const CHAR_T *str, SINGLE_CHAR_T ch) {
    while (*str) {
        if (*str == (CHAR_T)ch)
            return (CHAR_T*)str;
        str++;
    }
    return NULL;
}

CHAR_T* STRRCHR_FUNC(const CHAR_T *str, SINGLE_CHAR_T ch) {
    const CHAR_T *end = str;
    while (*end)
        end++;
    end--;
    while (end > str) {
        if (*end == (CHAR_T)ch)
            return (CHAR_T*)end;
        end--;
    }
    return NULL;
}

size_t STRSPN_FUNC(const CHAR_T *str, const CHAR_T *char_s) {
    size_t sz = 0;
    while (*str) {
        CHAR_T ch = *str;
        if (STRCHR_FUNC(char_s, ch)) {
            sz ++;
            str++;
        } else break;
    }
    return sz;
}

size_t STRCSPN_FUNC(const CHAR_T *str, const CHAR_T *char_s) {
    size_t sz = 0;
    while (*str) {
        CHAR_T ch = *str;
        if (STRCHR_FUNC(char_s, ch)) break;
        str++;
        sz++;
    }
    return sz;
}

CHAR_T* STRPBRK_FUNC(const CHAR_T *str, const CHAR_T *breaks) {
    while (*breaks) {
        CHAR_T ch = *breaks++;
        CHAR_T *brk = STRCHR_FUNC(str, ch);
        if (brk) return brk;
    }
    return NULL;
}

CHAR_T* STRSTR_FUNC(const CHAR_T *str, const CHAR_T *substr) {
    size_t len = STRLEN_FUNC(str);
    size_t sublen = STRLEN_FUNC(substr);

    if (sublen > len)
        return NULL;

    CHAR_T buf[sublen + 3];
    STRNCPY_FUNC(buf + 1, str, sublen);
    str += sublen;

    while (*str) {
        if (STRCMP_FUNC(buf + 1, substr) == 0)
            return (CHAR_T*)str - sublen;
        for (int i = 0; i < (SINGLE_CHAR_T)(sizeof buf) - 3; i++) {
            buf[i] = buf[i + 1];
        }
        buf[sublen] = *str;
        str++;
    }

    if (STRCMP_FUNC(buf + 1, substr) == 0)
        return (CHAR_T*)str - sublen;

    return NULL;
}

/* mem-функции */

VOID_T* MEMCHR_FUNC(const VOID_T *ptr, SINGLE_CHAR_T ch, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (((CHAR_T*)ptr)[i] == (CHAR_T)ch)
            return (VOID_T*)ptr + i;
    }
    return NULL;
}

int MEMCMP_FUNC(const VOID_T *ptr1, const VOID_T *ptr2, size_t count) {
    CHAR_T *buf1 = (CHAR_T*)ptr1;
    CHAR_T *buf2 = (CHAR_T*)ptr2;
    for (size_t i = 0; i < count; i++) {
        CHAR_T c1 = buf1[i];
        CHAR_T c2 = buf2[i];
        if (c1 > c2)
            return 1;
        if (c1 < c2)
            return -1;
    }
    return 0;
}


VOID_T* MEMSET_FUNC(VOID_T *ptr, SINGLE_CHAR_T c, size_t count) {
    CHAR_T *buf = (CHAR_T*)ptr;
    for (size_t i = 0; i < count; i++) {
        buf[i] = c;
    }
    return buf;
}

// Мы выключаем оптимизацию dead-code-removal для этой функции, потому что, как оказалось,
// оптимизатор GCC может удалить вызов этой функции, а это может сломать безопасность
// некоторых программ. Таким образом, GCC гарантирует генерацию вызова этой функции.

__attribute__((optimize ("no-dce")))
VOID_T* MEMSET_EXPLICIT_FUNC(VOID_T *ptr, SINGLE_CHAR_T c, size_t count) {
    CHAR_T *buf = (CHAR_T*)ptr;
    for (size_t i = 0; i < count; i++) {
        buf[i] = c;
    }
    return buf;
}

VOID_T* MEMCPY_FUNC(VOID_T *dest, const VOID_T *src, size_t count) {
    for (size_t i = 0; i < (count >> 2); i++) {
        ((SINGLE_CHAR_T*)dest)[i] = ((SINGLE_CHAR_T*)src)[i];
    }
    for (size_t i = 0; i < (count & 3); i++) {
        ((CHAR_T*)dest)[i] = ((SINGLE_CHAR_T*)src)[i];
    }
    return dest;
}

VOID_T* MEMMOVE_FUNC(VOID_T *dest, const VOID_T *src, size_t count) {
    VOID_T *buf = malloc(count);
    MEMCPY_FUNC(buf, src, count);
    MEMCPY_FUNC(dest, buf, count);
    free(buf);
    return dest;
}

VOID_T* MEMCCPY_FUNC(VOID_T* restrict dest, const VOID_T* restrict src, SINGLE_CHAR_T ch, size_t count) {
    CHAR_T c = (CHAR_T)ch;
    for (size_t i = 0; i < count; i++) {
        CHAR_T b = ((CHAR_T*)src)[i];
        ((CHAR_T*)dest)[i] = b;
        if (b == c) break;
    }
    return dest;
}

