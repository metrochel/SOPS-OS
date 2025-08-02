/*
 * wctype.c
 *
 *
 */

#include "include/wctype.h"
#include "include/wchar.h"

#define CHAR wint_t
#define SMALL_CHAR wchar_t

#define ISALNUM iswalnum
#define ISALPHA iswalpha
#define ISDIGIT iswdigit
#define ISXDIGIT iswxdigit
#define ISLOWER iswlower
#define ISUPPER iswupper
#define ISPUNCT iswpunct
#define ISSPACE iswspace
#define ISPRINT iswprint
#define ISCNTRL iswcntrl
#define ISGRAPH iswgraph
#define ISBLANK iswblank

#define STRCHR wcschr

#define LOWERCASE       locale.ctype.wide_lowercase_letters
#define UPPERCASE       locale.ctype.wide_uppercase_letters
#define DIGITS          locale.ctype.wide_digits
#define XDIGITS         locale.ctype.wide_xdigits
#define PUNCTS          locale.ctype.wide_punct_chars
#define OTHER_PRINTS    locale.ctype.wide_other_print_chars

#include ".no-compile/ctype_base.c"

wint_t towlower(wint_t c) {
    if ('A' <= c && c <= 'Z')
        return c + 0x20;

    wchar_t *ch = wcschr(locale.ctype.wide_uppercase_letters, c);
    if (!ch) return c;
    int index = (int)(ch - locale.ctype.wide_uppercase_letters);
    const wchar_t *lower_ch = locale.ctype.wide_lowercase_letters + index;

    wint_t new_ch = *lower_ch;

    return new_ch;
}

wint_t towupper(wint_t c) {
    if ('A' <= c && c <= 'Z')
        return c + 0x20;

    wchar_t *ch = wcschr(locale.ctype.wide_lowercase_letters, c);
    if (!ch) return c;
    int index = (int)(ch - locale.ctype.wide_lowercase_letters);
    const wchar_t *upper_ch = locale.ctype.wide_uppercase_letters + index;

    wint_t new_ch = *upper_ch;

    return new_ch;
}