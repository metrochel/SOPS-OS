//
//  Стандартные IO-функции
//
//  - Позволяет использовать вывод на IO-порты ЦП.
//

#ifndef _IO_INCL
#define _IO_INCL

#include "../util/nums.hpp"

/// @brief Считывает байт с порта.
/// @param port Порт
/// @return Считанный байт
static inline byte inb(word port) {
    byte ret;
    __asm__ volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Считывает слово с порта.
/// @param port Порт
/// @return Считанное слово
static inline word inw(word port) {
    word ret;
    __asm__ volatile ("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Считывает двойное слово с порта.
/// @param port Порт
/// @return Считанное двойное слово
static inline dword inl(word port) {
    dword ret;
    __asm__ volatile ("inl %w1, %d0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/// @brief Выводит байт на порт.
/// @param value Байт
/// @param port Порт
static inline void outb(word port, byte value) {
    __asm__ volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Выводит слово на порт.
/// @param value Слово
/// @param port Порт
static inline void outw(word port, word value) {
    __asm__ volatile ("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Выводит двойное слово на порт.
/// @param value Двойное слово
/// @param port Порт
static inline void outl(word port, dword value) {
    __asm__ volatile ("outl %d0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/// @brief Создаёт небольшую задержку, чтобы устройство сумело обработать команду.
static inline void io_wait() {
    outb(0x80, 0);
}

#endif