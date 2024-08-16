//
//  Менеджер памяти
//
//  - Управляет использованием ОЗУ.
//
#include <stdint.h>
#ifndef _MEMMGR_INCL
#define _MEMMGR_INCL

#include "paging.hpp"

#define HEAP_PHYSADDR 0x1000000
#define HEAP_VIRTADDR 0x2000000

/// @brief Блок в памяти
struct MemBlock {
    MemBlock *prevAddr;     // Адрес предыдущего блока
    uint32_t length;        // Длина блока в байтах
    void     *memPtr;       // Указатель на блок в памяти
    MemBlock *nextAddr;     // Адрес следующего блока
} __attribute__((packed));

/// @brief Инициализирует менеджер памяти.
void initMemMgr();

/// @brief Выделяет несколько байт из кучи.
/// @param amount Длина блока, Б
/// @return Указатель на выделенное место
void* kmalloc(uint32_t amount);

/// @brief Освобождает место в куче.
/// @param var Адрес удаляемой переменной
void kfree(void* var);

/// @brief Устанавливает всю память на одно значение.
/// @param ptr Указатель на обрабатываемый участок
/// @param count Число изменяемых байтов
/// @param val Новое значение
void memset(uint8_t *ptr, uint32_t count, uint8_t val);

/// @brief Копирует N Б памяти с указателя A на указатель B.
void memcpy(uint8_t *a, uint8_t *b, uint32_t n);

#endif