#include <stdint.h>

#define IO_SIG 5

//
//  Стандартные IO-функции
//
//  - Позволяет использовать вывод на IO-порты ЦП.
//

/// @brief Считывает байт с порта.
/// @param port Порт
/// @return Считанный байт
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Считывает слово с порта.
/// @param port Порт
/// @return Считанное слово
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Считывает двойное слово с порта.
/// @param port Порт
/// @return Считанное двойное слово
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %w1, %l0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Выводит байт на порт.
/// @param value Байт
/// @param port Порт
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Выводит слово на порт.
/// @param value Слово
/// @param port Порт
static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Выводит двойное слово на порт.
/// @param value Двойное слово
/// @param port Порт
static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %l0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Создаёт небольшую задержку, чтобы устройство сумело обработать команду.
static inline void io_wait() {
    outb(0x80, 0);
}