//
//  Менеджер страниц
//
//  - Руководит страницами памяти.
//

#ifndef _PAGING_INCL
#define _PAGING_INCL

#include "../util/nums.hpp"

#define PAGING_BASE         0x101000
#define PAGE_SIZE           0x1000
#define PAGE_TABLE_SIZE     (0x1000*1024)
#define PAGE_TABLE_COUNT    (PAGE_TABLE_SIZE / PAGE_SIZE)

/// @brief Создаёт страницу по данному адресу.
/// @param vaddr Виртуальный адрес страницы
/// @param paddr Физический адрес блока в памяти
void createPage(dword vaddr, dword paddr);

/// @brief Создаёт несколько страниц по данному адресу
/// @param vaddr Виртуальный адрес первой страницы
/// @param paddr Физический адрес первого блока
/// @param count Число страниц
void createPages(dword vaddr, dword paddr, dword count);

/// @brief Создаёт таблицу страниц по данному адресу.
/// @param vaddr Виртуальный адрес
void createPageTable(dword vaddr);

/// @brief Определяет физический адрес для данного виртуального адреса.
/// @param vaddr Виртуальный адрес
dword getPhysAddr(dword vaddr);

/// @brief Устанавливает минимальный уровень полномочий для доступа к таблице.
/// @param vaddr Виртуальный адрес, содержимый в странице
/// @param cpl Уровень полномочий
void setPagePermsLevel(dword vaddr, byte cpl);

/// @brief Устанавливает возможность записи в страницу.
/// @param vaddr Виртуальный адрес, содержимый в странице
/// @param rw Допустимость записи
void setPageReadWrite(dword vaddr, bool rw);

#endif