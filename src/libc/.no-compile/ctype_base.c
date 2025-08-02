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
    return ('a' <= c && c <= 'z') || STRCHR(LOWERCASE, c);
}

int ISUPPER(CHAR c) {
    return ('A' <= c && c <= 'Z') || STRCHR(UPPERCASE, c);
}

int ISDIGIT(CHAR c) {
    return ('0' <= c && c <= '9') || STRCHR(DIGITS, c);
}

int ISXDIGIT(CHAR c) {
    return ('0' <= c && c <= '9')
    || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')
    || STRCHR(XDIGITS, c);
}

int ISCNTRL(CHAR c) {
    return c < ' ' && c != '\t';
}

int ISPUNCT(CHAR c) {
    return STRCHR((SMALL_CHAR*)".,/<>?;':\"[]{}-=_+~!@#$%^&*()`|\\\t", c)
    || STRCHR(PUNCTS, c);
}

int ISGRAPH(CHAR c) {
    return ISALNUM(c) || STRCHR(OTHER_PRINTS, c);
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