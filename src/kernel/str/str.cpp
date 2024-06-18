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
    return true;
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