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

/// @brief Устанавливает всю память на одно значение.
/// @param ptr Указатель на обрабатываемый участок
/// @param count Число изменяемых байтов
/// @param val Новое значение
inline void memset(uint8_t *ptr, uint32_t count, uint8_t val) {
    for (uint32_t i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

/// @brief Копирует N Б памяти с указателя A на указатель B.
inline void memcpy(uint8_t* a, uint8_t* b, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        *b++ = *a++;
    }
}

/// @brief Сравнивает N Б памяти на указателях A и B.
inline bool memcmp(uint8_t *a, uint8_t* b, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

#endif