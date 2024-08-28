//
//  Библиотека с вспомогательными функциями
//
//  - Разные полезные штуки.
//
#ifndef _UTIL_INCL
#define _UTIL_INCL

#include "nums.hpp"

/// @brief Считывает содержимое регистра FLAGS.
inline word getFlags() {
    word flags;
    __asm__ ("pushf; pop %w0" : "=m"(flags) :);
    return flags;
}

/// @brief Вызывает прерывание.
/// @param intNo Номер прерывания
inline void interrupt(byte intNo) {
    __asm__ ("int %b0" : : "a"(intNo));
}

/// @brief Меняет значения A и B местами.
inline void swap(char* a, char* b) {
    char c;
    c = *b;
    *b = *a;
    *a = c;
}

/// @brief Устанавливает всю память на одно значение.
/// @param ptr Указатель на обрабатываемый участок
/// @param count Число изменяемых байтов
/// @param val Новое значение
inline void memset(byte *ptr, dword count, byte val) {
    for (dword i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

/// @brief Копирует N Б памяти с указателя A на указатель B.
inline void memcpy(byte* a, byte* b, dword n) {
    for (dword i = 0; i < n; i++) {
        *b++ = *a++;
    }
}

/// @brief Сравнивает N Б памяти на указателях A и B.
inline byte memcmp(byte *a, byte* b, dword n) {
    for (dword i = 0; i < n; i++) {
        if (a[i] > b[i]) return 0xFF;
        if (a[i] < b[i]) return 0x00;
    }
    return 0x80;
}

inline void setCarry() {
    __asm__ ("stc");
}

inline void clearCarry() {
    __asm__ ("clc");
}

inline bool isCarry() {
    return getFlags() & 1;
}

inline bool isZero() {
    return getFlags() & 64;
}

inline bool isDirection() {
    return getFlags() & 1024;
}

#endif