//
//  Библиотека с низкоуровневыми функциями
//
//  - Вспомогательные функции из Ассемблера.
//
#include <stdint.h>

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