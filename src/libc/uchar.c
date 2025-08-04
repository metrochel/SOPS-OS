/*
 * uchar.c
 *
 *
 */

#include "include/uchar.h"
#include "include/stdlib.h"
#include "include/errno.h"

size_t mbrtoc16(char16_t restrict *c16, const char restrict *c8, size_t count, mbstate_t restrict *state) {
    if (state->utf16_is_surrogate) {
        char16_t write = 0xDC00 + state->symbol & 0x3FF;
        if (c16) *c16 = write;
        state->symbol = 0;
        state->utf16_is_surrogate = 0;
        return 0;
    }

    wchar_t c;
    size_t sz = mbtowc(&c, c8, count);
    if (sz == (size_t)-2 || sz == (size_t)-1)
        return sz;

    if (!c) {
        state->symbol = 0;
        state->utf16_is_surrogate = 0;
        return 0;
    }

    if (c >= 0x10000) {
        state->symbol = c;
        state->utf16_is_surrogate = 1;
        char16_t write = 0xD800 + (state->symbol >> 10) & 0x3FF;
        if (c16) *c16 = write;
        return (size_t)-3;
    }

    char16_t write = c;
    if (c16) *c16 = write;
    return sz;
}

size_t c16rtomb(char restrict *c8, char16_t c16, mbstate_t *state) {
    if (state->utf16_is_surrogate && c16 >= 0xDC00 && c16 <= 0xDFFF) {
        wchar_t c = (state->symbol) | ((c16 & 0x3FF) << 10);
        state->utf16_is_surrogate = 0;
        state->symbol = 0;
        int result = wctomb(c8, c);
        if (result < 0)
            errno = EILSEQ;
        return result;
    }

    if (c16 >= 0xD800 && c16 <= 0xDBFF) {
        state->symbol = c16 & 0x3FF;
        state->utf16_is_surrogate = 1;
        return 0;
    }

    wchar_t symbol = c16;
    int result = wctomb(c8, symbol);
    if (result < 0)
        errno = EILSEQ;
    return result;
}

size_t mbrtoc32(char32_t restrict *c32, const char restrict *c8, size_t count, mbstate_t restrict *state) {
    wchar_t wc;
    int sz = mbtowc(&wc, c8, count);
    if (sz == -1) {
        errno = EILSEQ;
        return -1;
    }
    if (sz == -2)
        return -2;

    if (c32) *c32 = wc;
    return sz;
}

size_t c32rtomb(char restrict *c8, char32_t c32, mbstate_t restrict *state) {
    int result = wctomb(c8, c32);
    if (result < 0) {
        error = EILSEQ;
        return -1;
    }
    return result;
}

size_t mbrtoc8(char8_t restrict *c8, const char restrict *c, size_t count, mbstate_t restrict *state) {
    wchar_t wc;
    size_t sz = mbtowc(&wc, c, count);
    if (sz == (size_t)-2)
        return -2;
    if (sz == (size_t)-1) {
        errno = EILSEQ;
        return -1;
    }

    if (wc == 0) {
        if (c8) *c8 = 0;
        return 0;
    }

    if (state->utf8_followers) {
        int write = 0b10000000 | (state->symbol & 0b111111);
        state->symbol >>= 6;
        state->utf8_followers--;
        if (c8) *c8 = write;
        return -3;
    }

    if (wc < 0x80) {
        if (c8) *c8 = wc;
        return sz;
    }

    int follows, write;
    if (wc < 0x800) {
        follows = 1;
        write = 0b11000000 | ((wc >> 6) & 0x1F);
    }
    else if (wc < 0x10000) {
        follows = 2;
        write = 0b11100000 | ((wc >> 12) & 0x0F);
    }
    else if (wc < 0x110000) {
        follows = 3;
        write = 0b11110000 | ((wc >> 18) & 0x07);
    }
    else {
        errno = EILSEQ;
        return -1;
    }

    if (c8) *c8 = write;
    int symbol = 0;
    for (int i = 0; i < follows; i++) {
        symbol = (symbol << 6) | (wc & 0x3F);
        wc >>= 6;
    }
    state->symbol = symbol;
    state->utf8_followers = follows;
    return sz;
}

size_t c8rtomb(char restrict *c, char8_t c8, mbstate_t restrict *state) {
    if (state->utf8_followers) {
        wchar_t symbol = (state->symbol << 6) | (c8 & 0x3F);
        state->symbol = symbol;
        state->utf8_followers--;
        if (!state->utf8_followers) {
            int result = wctomb(c, &symbol);
            if (result == -1)
                errno = EILSEQ;
            return result;
        }
        return -3;
    }
    int size, chr;
    if ((c8 & 0b11111000) == 0b11110000) {
        size = 4;
        chr = c8 & 0x07;
    }
    else if ((c8 & 0b11110000) == 0b11100000) {
        size = 3;
        chr = c8 & 0x0F;
    }
    else if ((c8 & 0b11100000) == 0b11000000) {
        size = 2;
        chr = c8 & 0x1F;
    }
    else {
        size = 1;
        chr = c8 & 0x7F;
    }

    if (size == 1) {
        if (c) *c = chr;
        return 1;
    }
    state->symbol = chr;
    state->utf8_followers = size - 1;
    return 0;
}