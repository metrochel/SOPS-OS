//
//  Библиотека для управления хранилищами данных
//
//  - Реализует управление жёсткими дисками и компакт-дисками.
//
#ifndef _DISK_INCL
#define _DISK_INCL
#include "ide.hpp"
#include "../io/io.hpp"
#include "../util/nums.hpp"

#define DISK_TYPE_NONE   0
#define DISK_TYPE_ATA    1
#define DISK_TYPE_ATAPI  2
#define DISK_TYPE_SATA   3
#define DISK_TYPE_SATAPI 4

/// @brief Данные о диске
struct DiskData {
    byte DiskType;               // Тип диска
    dword TotalLBA28Sectors;     // Суммарное число секторов, доступных в режиме LBA28
    bool LBA48Supported;            // Флаг доступности режима LBA48
    qword TotalLBA48Sectors;     // Суммарное число секторов, доступных в режиме LBA48
    byte MaxUDMAMode;            // Максимально доступный режим UDMA
    byte ActUDMAMode;            // Активный режим UDMA
};

/// @brief Определяет вид диска.
/// @return Соответствующий виду диска код
DiskData identifyDisk();

void readSector(byte *buf, dword lba);
void readSectors(byte *buf, dword lba, dword count);

void writeSector(byte* buf, dword lba);
void writeSectors(byte* buf, dword lba, dword count);

#endif