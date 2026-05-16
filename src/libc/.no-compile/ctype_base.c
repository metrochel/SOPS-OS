/*
 * ctype_base.c
 *
 *
 */

#include "../locales/ext_lconv.h"
#include "../include/limits.h"

#ifndef CHAR

#define CHAR char
#define STRCHR STRCHR

#endif

int ISALNUM(CHAR c) {
    return ISALPHA(c) || ISDIGIT(c);
}

int ISALPHA(CHAR c) {
    return ISLOWER(c) || ISUPPER(c);
}

int ISLOWER(CHAR c) {
    int result = ('a' <= c && c <= 'z');
    if (!LOWERCASE) return result;
    return result || STRCHR(LOWERCASE, c);
}

int ISUPPER(CHAR c) {
    int result = ('A' <= c && c <= 'Z');
    if (!UPPERCASE) return result;
    return result || STRCHR(UPPERCASE, c);
}

int ISDIGIT(CHAR c) {
    int result = ('0' <= c && c <= '0');
    if (!DIGITS) return result;
    return result || STRCHR(DIGITS, c);
}

int ISXDIGIT(CHAR c) {
    int result = ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    if (!XDIGITS) return result;
    return result || STRCHR(XDIGITS, c);
}

int ISCNTRL(CHAR c) {
    return c < ' ' && c != '\t';
}

int ISPUNCT(CHAR c) {
    int result = STRCHR((SMALL_CHAR*)".,/<>?;':\"[]{}-=_+~!@#$%^&*()`|\\\t", c) != NULL;
    if (!PUNCTS) return result;
    return result || STRCHR(PUNCTS, c);
}

int ISGRAPH(CHAR c) {
    int result = ISALNUM(c);
    if (!OTHER_PRINTS) return result;
    return result || STRCHR(OTHER_PRINTS, c);
}

int ISSPACE(CHAR c) {
    return c == ' ';
}

int ISBLANK(CHAR c) {
    return c == ' ' || c == '\t';
}

int ISPRINT(CHAR c) {
    return ISGRAPH(c) || ISSPACE(c);
}