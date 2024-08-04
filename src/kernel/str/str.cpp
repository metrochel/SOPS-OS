#include "str.hpp"

bool strcmp(char* str1, char* str2) {
    if (*str1 != *str2)
        return false;
    while (*str1 != 0 && *str2 != 0) {
        if (*str1 != *str2)
            return false;
        str1 ++;
        str2 ++;
    }
    return *str1 == *str2;
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

uint8_t strcpy(char* str1, char* str2) {
    uint8_t len = 0;
    while (*str1 != 0) {
        *str2 = *str1;
        str1 ++;
        str2 ++;
        len ++;
    }
    *str2++ = *str1++;
    return len;
}

uint32_t strlen(char* str) {
    uint32_t len = 0;
    while (*str != 0) {
        len ++;
        str ++;
    }
    return len;
}

uint8_t numasstr(uint32_t num, char* out) {
    if (num == 0) {
        *out++ = '0';
        return 1;
    }
    uint8_t digits = 0;
    uint32_t numclone = num;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    out += digits - 1;
    for (uint8_t i = 0; i < digits; i++) {
        *out-- = num % 10 + 0x30;
        num /= 10;
    }
    return digits;
}