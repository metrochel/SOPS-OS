//
//  Менеджер памяти
//
//  - Управляет использованием ОЗУ.
//
#ifndef _MEMMGR_INCL
#define _MEMMGR_INCL

#include "../util/nums.hpp"
#include "paging.hpp"
#include "pmm.hpp"
#include "vmm.hpp"

#define HEAP_PHYSADDR 0x7000000
#define HEAP_VIRTADDR 0x2000000

#define MEMBLOCK_FLAG_OCCUPIED      0b1
#define MEMBLOCK_FLAG_PAGE_ALLOC    0b10

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
byte *kmallocPhys(ptrint virtAddr, dword amount, word pid);

/// @brief Выделяет физическую память для данного виртуального адреса.
/// @param virtAddr Виртуальный адрес
/// @param amount Длина блока, Б
/// @return Указатель на выделенное место
/// @note Без параметра `pid` функция выделяет память ядру.
byte *kmallocPhys(ptrint virtAddr, dword amount);

/// @brief Освобождает место в куче.
/// @param Указатель высвобождаемой переменной
/// @param pid PID владельца переменной
void kfree(void *var, word pid);

/// @brief Освобождает место в куче.
/// @param var Адрес удаляемой переменной
/// @note Без параметра `pid` владельцем переменной считается ядро.
/// @attention Ядро удалит переменную из памяти вне зависимости от её настоящего владельца!
void kfree(void* var);

#endif