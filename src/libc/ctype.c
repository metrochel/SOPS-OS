/*
 * ctype.c
 *
 *
 */

#include "include/ctype.h"

#include "locales/ext_lconv.h"
#include "include/string.h"
#include "include/stdlib.h"
#include "include/limits.h"

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
    return islower(c) || isupper(c);
}

int islower(int c) {
    return ('a' <= c && c <= 'z') || strchr(locale.ctype.lowercase_letters, c);
}

int isupper(int c) {
    return ('A' <= c && c <= 'Z') || strchr(locale.ctype.uppercase_letters, c);
}

int isdigit(int c) {
    return ('0' <= c && c <= '9') || strchr(locale.ctype.digits, c);
}

int isxdigit(int c) {
    return ('0' <= c && c <= '9')
    || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')
    || strchr(locale.ctype.xdigits, c);
}

int iscntrl(int c) {
    return c < ' ' && c != '\t';
}

int ispunct(int c) {
    return strchr(".,/<>?;':\"[]{}-=_+~!@#$%^&*()`|\\\t", c)
    || strchr(locale.ctype.punct_chars, c);
}

int isgraph(int c) {
    return isalnum(c) || strchr(locale.ctype.other_print_chars, c);
}

int isspace(int c) {
    return c == ' ';
}

int isblank(int c) {
    return c == ' ' || c == '\t';
}

int isprint(int c) {
    return isgraph(c) || isspace(c);
}

int tolower(int c) {
    if ('A' <= c && c <= 'Z')
        return c + 0x20;

    char *ch = strchr(locale.ctype.uppercase_letters, c);
    if (!ch) return c;
    int index = (int)(ch - locale.ctype.uppercase_letters);
    char *lower_ch = locale.ctype.lowercase_letters + index;

    size_t char_sz = mblen(lower_ch, MB_LEN_MAX);
    int new_ch = 0;
    char *buf = (char*)&new_ch;
    for (size_t i = 0; i < char_sz; i++) {
        buf[i] = *lower_ch++;
    }

    return new_ch;
}

int toupper(int c) {
    if ('A' <= c && c <= 'Z')
        return c + 0x20;

    char *ch = strchr(locale.ctype.lowercase_letters, c);
    if (!ch) return c;
    int index = (int)(ch - locale.ctype.lowercase_letters);
    char *upper_ch = locale.ctype.uppercase_letters + index;

    size_t char_sz = mblen(upper_ch, MB_LEN_MAX);
    int new_ch = 0;
    char *buf = (char*)&new_ch;
    for (size_t i = 0; i < char_sz; i++) {
        buf[i] = *upper_ch++;
    }

    return new_ch;
}