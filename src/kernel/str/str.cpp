#include "str.hpp"
#include "../io/com.hpp"
#include "../util/util.hpp"
#include "../graphics/glyphs.hpp"
#include "../memmgr/memmgr.hpp"

bool strcmp(char* str1, char* str2) {
    return strcmpS(str1, str2) == 0x80;
}

byte strcmpS(char* str1, char* str2) {
    return strcmpS(str1, str2, false);
}

byte strcmpS(char* str1, char* str2, bool caseInsensitive) {
    byte s1 = *str1;
    byte s2 = *str2;
    if (s1 >= 0x61 && s1 < 0x7B && caseInsensitive)
        s1 -= 0x20;
    if (s2 >= 0x61 && s2 < 0x7B && caseInsensitive)
        s2 -= 0x20;
    if (s1 > s2)
        return 0xFF;
    if (s1 < s2)
        return 0x00;
    while (*str1 != 0 && *str2 != 0) {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 >= 0x61 && s1 < 0x7B && caseInsensitive)
            s1 -= 0x20;
        if (s2 >= 0x61 && s2 < 0x7B && caseInsensitive)
            s2 -= 0x20;
        if (s1 > s2)
            return 0xFF;
        if (s1 < s2)
            return 0x00;
    }
    s1 = *str1;
    s2 = *str2;
    if (s1 >= 0x61 && s1 < 0x7B && caseInsensitive)
        s1 -= 0x20;
    if (s2 >= 0x61 && s2 < 0x7B && caseInsensitive)
        s2 -= 0x20;
    if (s1 > s2)
        return 0xFF;
    if (s1 < s2)
        return 0x00;
    return 0x80;
}

bool strstartswith(char* str, char* substr) {
    while (*substr != 0) {
        if (*str != *substr)
            return false;
        str ++;
        substr ++;
    }
    return true;
}

dword strcpy(char* str1, char* str2) {
    dword len = 0;
    while (*str1 != 0) {
        *str2 = *str1;
        str1 ++;
        str2 ++;
        len ++;
    }
    *str2++ = *str1++;
    return len;
}

dword strlen(char* str) {
    dword len = 0;
    while (*str != 0) {
        len ++;
        str ++;
    }
    return len;
}

byte numasstr(dword num, char* out) {
    if (num == 0) {
        *out++ = '0';
        return 1;
    }
    byte digits = 0;
    dword numclone = num;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    out += digits - 1;
    for (byte i = 0; i < digits; i++) {
        *out-- = num % 10 + 0x30;
        num /= 10;
    }
    return digits;
}

char** strsplit(char *str, char *pattern) {
    dword stlen = strlen(str);
    dword plen = strlen(pattern);
    dword possibleFits = stlen / plen;

    char** result = (char**)kmalloc(possibleFits * sizeof(char**));
    char *ptr = (char*)kmalloc(stlen * 2 + 2);
    result[0] = ptr;
    word i = 1;
    while (*str) {
        if (memcmp((byte*)str, (byte*)pattern, plen)) {
            *ptr++ = 0;
            result[i] = ptr;
            i ++;
            str += plen;
        } else {
            *ptr++ = *str++;
        }
    }
    result[i] = nullptr;
    *ptr++ = 0;
    return result;
}

void strskiplines(char *&str, dword lines) {
    dword count = 0;
    while (*str && count < lines) {
        if (*str == 0x0A)
            count++;
        *str++;
    }
}

inline bool isHexDigit(char d) {
    return (d >= '0' && d <= '9') || (d >= 'A' && d <= 'F') || (d >= 'a' && d <= 'f');
}

qword strhextoint(char *str) {
    qword ret = 0;
    if (*str == '0' && *(str+1) == 'x')
        str += 2;
    
    while (isHexDigit(*str)) {
        byte digit;
        if (*str >= 'a')
            digit = *str - 0x61 + 10;
        else if (*str >= 'A')
            digit = *str - 0x41 + 10;
        else
            digit = *str - 0x30;

        ret *= 16;
        ret += digit;

        str++;
    }

    return ret;
}

inline bool isDecimalDigit(char c) {
    return (c >= '0') && (c <= '9');
}

qword strdectoint(char *str) {
    qword ret = 0;
    while (isDecimalDigit(*str)) {
        byte digit = (*str++) - 0x30;
        ret *= 10;
        ret += digit;
    }
    return ret;
}