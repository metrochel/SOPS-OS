#include <stdint.h>
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