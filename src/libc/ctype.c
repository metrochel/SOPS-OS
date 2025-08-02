/*
 * ctype.c
 *
 *
 */

#include "include/ctype.h"
#include "include/string.h"
#include "include/stdlib.h"

// Здесь мы применим ту же чёрную магию препроцессора, что и в
// string.c/wchar.c.

#define CHAR int
#define SMALL_CHAR char

#define ISALNUM isalnum
#define ISALPHA isalpha
#define ISDIGIT isdigit
#define ISXDIGIT isxdigit
#define ISLOWER islower
#define ISUPPER isupper
#define ISPUNCT ispunct
#define ISSPACE isspace
#define ISPRINT isprint
#define ISCNTRL iscntrl
#define ISGRAPH isgraph
#define ISBLANK isblank

#define STRCHR strchr

#define LOWERCASE       locale.ctype.lowercase_letters
#define UPPERCASE       locale.ctype.uppercase_letters
#define DIGITS          locale.ctype.digits
#define XDIGITS         locale.ctype.xdigits
#define PUNCTS          locale.ctype.punct_chars
#define OTHER_PRINTS    locale.ctype.other_print_chars

#include ".no-compile/ctype_base.c"

int tolower(int c) {
    if ('A' <= c && c <= 'Z')
        return c + 0x20;

    char *ch = strchr(UPPERCASE, c);
    if (!ch) return c;
    int index = (int)(ch - UPPERCASE);
    const char *lower_ch = LOWERCASE + index;

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

    char *ch = strchr(LOWERCASE, c);
    if (!ch) return c;
    int index = (int)(ch - LOWERCASE);
    const char *upper_ch = UPPERCASE + index;

    size_t char_sz = mblen(upper_ch, MB_LEN_MAX);
    int new_ch = 0;
    char *buf = (char*)&new_ch;
    for (size_t i = 0; i < char_sz; i++) {
        buf[i] = *upper_ch++;
    }

    return new_ch;
}