//
//  Библиотека для управления хранилищами данных
//
//  - Реализует управление жёсткими дисками и компакт-дисками.
//
#ifndef _DISK_INCL
#define _DISK_INCL
#include <stdint.h>
#include "ide.hpp"
#include "../io/io.hpp"

#define DISK_TYPE_NONE   0
#define DISK_TYPE_ATA    1
#define DISK_TYPE_ATAPI  2
#define DISK_TYPE_SATA   3
#define DISK_TYPE_SATAPI 4

/// @brief Данные о диске
struct DiskData {
    uint8_t DiskType;               // Тип диска
    uint32_t TotalLBA28Sectors;     // Суммарное число секторов, доступных в режиме LBA28
    bool LBA48Supported;            // Флаг доступности режима LBA48
    uint64_t TotalLBA48Sectors;     // Суммарное число секторов, доступных в режиме LBA48
    uint8_t MaxUDMAMode;            // Максимально доступный режим UDMA
    uint8_t ActUDMAMode;            // Активный режим UDMA
};

/// @brief Определяет вид диска.
/// @return Соответствующий виду диска код
DiskData identifyDisk();

void readSector(uint8_t *buf, uint32_t lba);
void readSectors(uint8_t *buf, uint32_t lba, uint32_t count);

void writeSector(uint8_t* buf, uint32_t lba);
void writeSectors(uint8_t* buf, uint32_t lba, uint32_t count);

#endif