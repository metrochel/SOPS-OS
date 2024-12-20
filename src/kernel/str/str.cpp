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

char chartranslit(word c) {
    switch (c) {
        case 'А': return 'A';
        case 'Б': return 'B';
        case 'В': return 'V';
        case 'Г': return 'G';
        case 'Д': return 'D';
        case 'Е': return 'E';
        case 'Ё': return 'E';
        case 'Ж': return 'Z';
        case 'З': return 'Z';
        case 'И': return 'I';
        case 'К': return 'K';
        case 'Л': return 'L';
        case 'М': return 'M';
        case 'Н': return 'N';
        case 'О': return 'O';
        case 'П': return 'P';
        case 'Р': return 'R';
        case 'С': return 'S';
        case 'Т': return 'T';
        case 'У': return 'U';
        case 'Ф': return 'F';
        case 'Х': return 'H';
        case 'Ц': return 'C';
        case 'Ч': return 'C';
        case 'Ш': return 'S';
        case 'Щ': return 'S';
        case 'Ъ': return '\'';
        case 'Ы': return 'I';
        case 'Ь': return '\'';
        case 'Э': return 'E';
        case 'Ю': return 'U';
        case 'Я': return 'A';
        
        case 'а': return 'a';
        case 'б': return 'b';
        case 'в': return 'v';
        case 'г': return 'g';
        case 'д': return 'd';
        case 'е': return 'e';
        case 'ё': return 'e';
        case 'ж': return 'z';
        case 'з': return 'z';
        case 'и': return 'i';
        case 'к': return 'k';
        case 'л': return 'l';
        case 'м': return 'm';
        case 'н': return 'n';
        case 'о': return 'o';
        case 'п': return 'p';
        case 'р': return 'r';
        case 'с': return 's';
        case 'т': return 't';
        case 'у': return 'u';
        case 'ф': return 'f';
        case 'х': return 'h';
        case 'ц': return 'c';
        case 'ч': return 'c';
        case 'ш': return 's';
        case 'щ': return 's';
        case 'ъ': return '\'';
        case 'ы': return 'i';
        case 'ь': return '\'';
        case 'э': return 'e';
        case 'ю': return 'u';
        case 'я': return 'a';

        default: return c;
    }
}