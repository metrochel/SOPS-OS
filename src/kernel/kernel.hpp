#ifndef _KERNEL_INCL
#define _KERNEL_INCL
//
//  Основные данные ядра
//
//  Содержит описания основных структур, используемых ядром
//
#include "graphics/graphics.hpp"
#include "util/nums.hpp"

// ### BootLoaderData
// Структура с данными, полученными в загрузчике.
struct BootLoaderData {
    byte DiskNo;                // Номер диска, с которого загрузилась СОпС
    char CPUID_Vendor[12];      // Имя производителя процессора
    dword CPUID_Flags1;         // Флаги ЦП-1
    dword CPUID_Flags2;         // Флаги ЦП-2
    VBEModeInfo VBEInfo;        // Информация о графическом режиме
    word MemMapEntriesCount;    // Количество меток в разметке памяти
} __attribute__((packed));

/// @brief Указатель на данные загрузчика
extern BootLoaderData *bld;

#endif