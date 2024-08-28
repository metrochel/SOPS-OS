//
//  Менеджер страниц
//
//  - Руководит страницами памяти.
//

#ifndef _PAGING_INCL
#define _PAGING_INCL

#include "../util/nums.hpp"

#define PAGING_BASE 0x101000

/// @brief Создаёт страницу по данному адресу.
/// @param vaddr Виртуальный адрес страницы
/// @param paddr Физический адрес блока в памяти
void createPage(dword vaddr, dword paddr);

/// @brief Создаёт несколько страниц по данному адресу
/// @param vaddr Виртуальный адрес первой страницы
/// @param paddr Физический адрес первого блока
/// @param count Число страниц
void createPages(dword vaddr, dword paddr, word count);

/// @brief Создаёт таблицу страниц по данному адресу.
/// @param vaddr Виртуальный адрес
void createPageTable(dword vaddr);

/// @brief Определяет физический адрес для данного виртуального адреса.
/// @param vaddr Виртуальный адрес
dword getPhysAddr(dword vaddr);

#endif