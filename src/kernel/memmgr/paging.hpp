#include <stdint.h>
//
//  Менеджер страниц
//
//  - Руководит страницами памяти.
//

#ifndef _PAGING_INCL
#define _PAGING_INCL

#define PAGING_BASE 0x101000

/// @brief Создаёт страницу по данному адресу.
/// @param vaddr Виртуальный адрес страницы
/// @param paddr Физический адрес блока в памяти
void createPage(uint32_t vaddr, uint32_t paddr);

/// @brief Создаёт несколько страниц по данному адресу
/// @param vaddr Виртуальный адрес первой страницы
/// @param paddr Физический адрес первого блока
/// @param count Число страниц
void createPages(uint32_t vaddr, uint32_t paddr, uint16_t count);

/// @brief Создаёт таблицу страниц по данному адресу.
/// @param vaddr Виртуальный адрес
void createPageTable(uint32_t vaddr);

/// @brief Определяет физический адрес для данного виртуального адреса.
/// @param vaddr Виртуальный адрес
uint32_t getPhysAddr(uint32_t vaddr);

#endif