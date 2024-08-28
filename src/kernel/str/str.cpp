#include "str.hpp"
#include "../io/com.hpp"

byte strcmp(char* str1, char* str2) {
    byte s1 = *str1;
    byte s2 = *str2;
    if (s1 > s2)
        return 0xFF;
    if (s1 < s2)
        return 0x00;
    while (*str1 != 0 && *str2 != 0) {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 > s2)
            return 0xFF;
        if (s1 < s2)
            return 0x00;
    }
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
    return *substr == *str;
}

byte strcpy(char* str1, char* str2) {
    byte len = 0;
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