#ifndef _PMM_INCL
#define _PMM_INCL
//
//  Менеджер физической памяти
//
//  - Распределяет физическую память процессам.
//
#include "../util/nums.hpp"
#include "paging.hpp"

// ### MemMapEntry
// Эта структура описывает одну метку в разметке памяти.
struct MemMapEntry {
    qword base;             // Основание блока
    qword size;             // Длина блока, Б
    dword type;             // Тип блока
    dword exAttributes;     // Расширенные аттрибуты поля
} __attribute__((packed));

// ### MemMapEntryType
// Здесь описаны все допустимые типы меток разметки памяти.
enum MemMapEntryType {
    Free             = 1,   // Свободная память
    Reserved         = 2,   // Резервированная память
    ACPIReclaimable  = 3,   // Восполняемая память ACPI
    ACPINonVolatile  = 4,   // Невосполняемая память ACPI
    Bad              = 5    // Неисправная память
};

extern byte *pageFramesMap;

/// @brief Инициализирует менеджер физической памяти.
/// @param memMgrPtr Указатель на данные менеджера памяти
void initPMM(byte *&memMgrPtr);

/// @brief Выделяет одну страницу из физической памяти.
/// @return Физический адрес начала страницы
/// @note Если выделение провалено, возвращается 0.
ptrint allocatePageFrame();

/// @brief Освобождает страницу в памяти.
/// @param frame Физический адрес начала страницы
void freePageFrame(ptrint frame);

#endif