//
//  Менеджер памяти
//
//  - Управляет использованием ОЗУ.
//
#ifndef _MEMMGR_INCL
#define _MEMMGR_INCL

#include "../util/nums.hpp"
#include "paging.hpp"

#define HEAP_PHYSADDR 0x7000000
#define HEAP_VIRTADDR 0x2000000

#define MEMBLOCK_FLAG_OCCUPIED      0b1
#define MEMBLOCK_FLAG_PAGE_ALLOC    0b10

// ### MemBlock
// Эта структура описывает один выделенный блок в памяти.
struct MemBlock {
    MemBlock *previousBlock;    // Указатель на предыдущий блок
    qword size;                 // Размер блока, Б
    byte *memPtr;               // Указатель на блок в виртуальной памяти
    qword physaddr;             // Адрес блока в физической памяти
    word  pid;                  // PID процесса-владельца блока
    word  flags;                // Флаги блока
    MemBlock *nextBlock;        // Указатель на следующий блок
} __attribute__((packed));

// ### MemMapEntry
// Эта структура описывает одну метку в разметке памяти.
struct MemMapEntry {
    qword base;             // Основание блока
    qword length;           // Длина блока, Б
    dword type;             // Тип блока
    dword exAttributes;     // Расширенные аттрибуты поля
} __attribute__((packed));

/// @brief Инициализирует менеджер памяти.
void initMemMgr();

/// @brief Выделяет несколько байт из кучи.
/// @param amount Длина блока, Б
/// @param pid PID процесса-получателя блока
/// @return Указатель на выделенное место
byte* kmalloc(dword amount, word pid);

/// @brief Выделяет несколько байт из кучи.
/// @note Без параметра `pid` функция выделяет память ядру.
/// @param amount Длина блока, Б
/// @return Указатель на выделенное место
byte *kmalloc(dword amount);

/// @brief Выделяет физическую память для данного виртуального адреса.
/// @param virtAddr Виртуальный адрес
/// @param amount Длина блока, Б
/// @param pid PID процесса-получателя блока
/// @return Указатель на выделенное место
byte *kmallocPhys(dword virtAddr, dword amount, word pid);

/// @brief Выделяет физическую память для данного виртуального адреса.
/// @param virtAddr Виртуальный адрес
/// @param amount Длина блока, Б
/// @return Указатель на выделенное место
/// @note Без параметра `pid` функция выделяет память ядру.
byte *kmallocPhys(dword virtAddr, dword amount);

/// @brief Освобождает место в куче.
/// @param Указатель высвобождаемой переменной
/// @param pid PID владельца переменной
void kfree(void *var, word pid);

/// @brief Освобождает место в куче.
/// @param var Адрес удаляемой переменной
/// @note Без параметра `pid` владельцем переменной считается ядро.
/// @attention Ядро удалит переменную из памяти вне зависимости от её настоящего владельца!
void kfree(void* var);

/// @brief Выводит в отладку все блоки.
void logBlocks();

/// @brief Сортирует блоки в порядке возрастания адреса и объединяет "дыры".
void refreshBlocks();

#endif