//
//  Библиотека с вспомогательными функциями
//
//  - Разные полезные штуки.
//
#ifndef _UTIL_INCL
#define _UTIL_INCL

#include "nums.hpp"

#define directconv(target, type) (*((type*)&(target)))

/// @brief Меняет значения A и B местами.
inline void swap(char* a, char* b) {
    char c;
    c = *b;
    *b = *a;
    *a = c;
}

/// @brief Устанавливает всю память на одно значение.
/// @param mem Указатель на обрабатываемый участок
/// @param count Число изменяемых байтов
/// @param val Новое значение
template<typename T>
inline void memset(T *mem, dword count, byte val) {
    byte *ptr = (byte*)mem;
    for (dword i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

/// @brief Копирует N Б памяти с указателя A на указатель B.
template<typename T1, typename T2>
inline void memcpy(T1 *a, T2 *b, dword n) {
    byte *aptr = (byte*)a;
    byte *bptr = (byte*)b;
    for (dword i = 0; i < n; i++) {
        *bptr++ = *aptr++;
    }
}

/// @brief Сравнивает N Б памяти на указателях A и B.
template<typename T1, typename T2>
inline byte memcmp(T1 *a, T2 *b, dword n) {
    byte *aptr = (byte*)a;
    byte *bptr = (byte*)b;
    for (dword i = 0; i < n; i++) {
        if (aptr[i] != bptr[i])
            return false;
    }
    return true;
}

template<typename T>
inline void memshiftleft(T *ptr, dword dataSize, dword shift) {
    byte *bptr = (byte*)ptr;
    memset(bptr, shift, 0);
    for (dword i = shift; i < dataSize; i++) {
        bptr[i-shift] = bptr[i];
    }
}

inline qword max(qword a, qword b) {
    return a > b ? a : b;
}

inline qword min(qword a, qword b) {
    return a < b ? a : b;
}

#endif