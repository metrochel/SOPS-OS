//
//  Библиотека для управления хранилищами данных
//
//  - Реализует управление жёсткими дисками и компакт-дисками.
//
#include <stdint.h>
#ifndef IO_SIG
#include "../io/io.hpp"
#endif

#define DISK_ATA_DATA_PRIMARY                 0x1F0
#define DISK_ATA_ERROR_PRIMARY                0x1F1
#define DISK_ATA_FEATURES_PRIMARY             0x1F1
#define DISK_ATA_SECTOR_COUNT_PRIMARY         0x1F2
#define DISK_ATA_LBA_LOW_PRIMARY              0x1F3
#define DISK_ATA_LBA_MID_PRIMARY              0x1F4
#define DISK_ATA_LBA_HIGH_PRIMARY             0x1F5
#define DISK_ATA_DRIVE_HEAD_PRIMARY           0x1F6
#define DISK_ATA_STATUS_PRIMARY               0x1F7
#define DISK_ATA_COMMAND_PRIMARY              0x1F7
#define DISK_ATA_ALT_STATUS_PRIMARY           0x3F6
#define DISK_ATA_DRIVE_CONTROL_PRIMARY        0x3F7
#define DISK_ATA_DRIVE_ADDRESS_PRIMARY        0x3F7

#define DISK_ATA_DATA_SECONDARY               0x170
#define DISK_ATA_ERROR_SECONDARY              0x171
#define DISK_ATA_FEATURES_SECONDARY           0x171
#define DISK_ATA_SECTOR_COUNT_SECONDARY       0x172
#define DISK_ATA_SECTOR_NUM_SECONDARY         0x173
#define DISK_ATA_CYL_LOW_SECONDARY            0x174
#define DISK_ATA_CYL_HIGH_SECONDARY           0x175
#define DISK_ATA_DRIVE_HEAD_SECONDARY         0x176
#define DISK_ATA_STATUS_SECONDARY             0x177
#define DISK_ATA_COMMAND_SECONDARY            0x177
#define DISK_ATA_ALT_STATUS_SECONDARY         0x376
#define DISK_ATA_DRIVE_CONTROL_SECONDARY      0x377
#define DISK_ATA_DRIVE_ADDRESS_SECONDARY      0x377

#define DISK_STATUS_ERROR                     1
#define DISK_STATUS_INDEX                     2
#define DISK_STATUS_CORRECT                   4
#define DISK_STATUS_DRQ                       8
#define DISK_STATUS_SERVICE_RQ                16
#define DISK_STATUS_DRIVE_FAULT               32
#define DISK_STATUS_READY                     64
#define DISK_STATUS_BUSY                      128

#define DISK_TYPE_NONE  0
#define DISK_TYPE_ATA   1
#define DISK_TYPE_ATAPI 2
#define DISK_TYPE_SATA  3

/// @brief Данные о диске
struct DiskData {
    uint8_t DiskType;               // Тип диска
    uint32_t TotalLBA28Sectors;     // Суммарное число секторов, доступных в режиме LBA28
    bool LBA48Supported;            // Флаг доступности режима LBA48
    uint64_t TotalLBA48Sectors;     // Суммарное число секторов, доступных в режиме LBA48
};

/// @brief Определяет вид диска.
/// @return Соответствующий виду диска код
DiskData identifyDisk();

void readSector(uint8_t *buf, uint32_t lba);
void readSectors(uint8_t *buf, uint32_t lba, uint32_t count);

void writeSector(uint8_t* buf, uint32_t lba);
void writeSectors(uint8_t* buf, uint32_t lba, uint32_t count);