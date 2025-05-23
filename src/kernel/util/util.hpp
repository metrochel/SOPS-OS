//
//  Библиотека с вспомогательными функциями
//
//  - Разные полезные штуки.
//
#ifndef _UTIL_INCL
#define _UTIL_INCL

#include "nums.hpp"

#define directconv(target, type) (*((type*)&(target)))

class CPUContext {
    public:
        dword eax;
        dword ebx;
        dword ecx;
        dword edx;
        dword esi;
        dword edi;
        dword esp;
        dword ebp;
        dword eflags;
        word cs;
        word ds;
        word ss;
        word es;
        word fs;
        word gs;

    public:
        static CPUContext store() {
            CPUContext ctx;
            __asm__ volatile (" \
                movl %%eax, %d0; \
                movl %%ebx, %d1; \
                movl %%ecx, %d2; \
                movl %%edx, %d3; \
                movl %%esi, %d4; \
                movl %%edi, %d5; \
                movl %%esp, %d6; \
                movl %%ebp, %d7; \
                pushf; \
                popl %d8; \
                movw %%cs, %w9; \
                movw %%ds, %w10; \
                movw %%ss, %w11; \
                movw %%es, %w12; \
                movw %%fs, %w13; \
                movw %%gs, %w14;"
                : 
                    "=m"(ctx.eax), "=m"(ctx.ebx), "=m"(ctx.ecx), "=m"(ctx.edx), 
                    "=m"(ctx.esi), "=m"(ctx.edi), 
                    "=m"(ctx.esp), "=m"(ctx.ebp),
                    "=m"(ctx.eflags),
                    "=m"(ctx.cs), "=m"(ctx.ds), "=m"(ctx.ss), "=m"(ctx.es), "=m"(ctx.fs), "=m"(ctx.gs)
                : :);
                return ctx;
        }

        inline void restore() {
            __asm__ volatile (" \
                movw %w9,  %%ax; \
                movw %%ax, %%ds; \
                movw %w10, %%ax; \
                movw %%ax, %%ss; \
                movw %w11, %%ax; \
                movw %%ax, %%es; \
                movw %w12, %%ax; \
                movw %%ax, %%fs; \
                movw %w13, %%ax; \
                movw %%ax, %%gs; \
                movl %d0, %%eax; \
                movl %d1, %%ebx; \
                movl %d2, %%ecx; \
                movl %d3, %%edx; \
                movl %d4, %%esi; \
                movl %d5, %%edi; \
                push %d8;        \
                popf;            \
                push %d6;        \
                pop %%eax;       \
                movl %%eax, %%esp; \
                movl %d7, %%ebp;"
                : : 
                    "m"(this->eax), "m"(this->ebx), "m"(this->ecx), "m"(this->edx), 
                    "m"(this->esi), "m"(this->edi), 
                    "m"(this->esp), "m"(this->ebp),
                    "m"(this->eflags),
                    "m"(this->ds), "m"(this->ss), "m"(this->es), "m"(this->fs), "m"(this->gs)
                :);
        }
};

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
/// @param mem Указатель на обрабатываемый участок
/// @param count Число изменяемых байтов
/// @param val Новое значение
inline void memset(void *mem, dword count, byte val) {
    byte *ptr = (byte*)mem;
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
inline byte memcmp(byte *a, byte *b, dword n) {
    for (dword i = 0; i < n; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/// @brief Знаково сравнивает N Б памяти на указателях A и B.
/// @returns `0x00` - блок A меньше блока B;
/// @returns `0x80` - блоки A и B равны;
/// @returns `0xFF` - блок A больше блока B
inline byte memcmpS(byte *a, byte* b, dword n) {
    for (dword i = 0; i < n; i++) {
        if (a[i] > b[i]) return 0xFF;
        if (a[i] < b[i]) return 0x00;
    }
    return 0x80;
}

inline void memshiftleft(byte *ptr, dword dataSize, dword shift) {
    memset(ptr, shift, 0);
    for (dword i = shift; i < dataSize; i++) {
        ptr[i-shift] = ptr[i];
    }
}

/// @brief Разворачивает байты числа `n`.
inline dword byteswap(dword n) {
    dword out_n;
    __asm__ volatile (
        "movl %d1, %%eax;"
        "bswap %%eax;"
        "movl %%eax, %d0;"
        : "=m"(out_n)
        : "m"(n)
        :
    );
    return out_n;
}

/// @brief Переводит символ из UTF-8 в UTF-16LE.
inline word toUTF16(byte *&utf8) {
    word utf16 = 0;
    byte b = *utf8++;
    if (b < 0x80)
        return (word)b;
    
    if ((b & 0b11100000) == 0b11000000) {
        utf16 |= ((b & 0b11111) << 6);
        b = *utf8++;
        utf16 |= b & 0b111111;
        return utf16;
    }

    if ((b & 0b11110000) == 0b11100000) {
        utf16 |= ((b & 0b1111) << 12);
        b = *utf8++;
        utf16 |= ((b & 0b111111) << 6);
        b = *utf8++;
        utf16 |= (b & 0b111111);
        return utf16;
    }
    return 0;
}

inline word toUTF16(dword utf8) {
    utf8 = byteswap(utf8);
    byte *ptr = (byte*)&utf8;
    while (!*ptr) ptr++;
    return toUTF16(ptr);
}

inline byte digitsLength(qword n) {
    byte len = 0;
    while (n) {
        len ++;
        n /= 10;
    }
    return len;
}

inline qword max(qword a, qword b) {
    return a > b ? a : b;
}

inline qword min(qword a, qword b) {
    return a < b ? a : b;
}

inline void disableInts() {
    __asm__ ("cli");
}

inline void enableInts() {
    __asm__ ("sti");
}

inline void halt() {
    __asm__("hlt");
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