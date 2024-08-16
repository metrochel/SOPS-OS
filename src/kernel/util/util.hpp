//
//  Библиотека с вспомогательными функциями
//
//  - Разные полезные штуки.
//
#include <stdint.h>
#ifndef _UTIL_INCL
#define _UTIL_INCL

/// @brief Считывает содержимое регистра FLAGS.
inline uint16_t getFlags() {
    uint16_t flags;
    __asm__ ("pushf; pop %w0" : "=m"(flags) :);
    return flags;
}

/// @brief Вызывает прерывание.
/// @param intNo Номер прерывания
inline void interrupt(uint8_t intNo) {
    __asm__ ("int %b0" : : "a"(intNo));
}

/// @brief Меняет значения A и B местами.
inline void swap(char* a, char* b) {
    char c;
    c = *b;
    *b = *a;
    *a = c;
}

#endif