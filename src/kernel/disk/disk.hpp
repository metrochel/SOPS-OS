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

/// @brief Считывает один блок данных.
/// @param buf Буфер выхода данных
/// @param lba LBA-координаты блока
/// @param drive Номер диска в системе
void readSector(byte *buf, dword lba, byte drive);

/// @brief Считывает несколько блоков данных.
/// @param buf Буфер выхода данных
/// @param lba LBA-координаты первого блока
/// @param count Количество блоков
/// @param drive Номер диска в системе
void readSectors(byte *buf, dword lba, dword count, byte drive);

/// @brief Записывает один блок данных.
/// @param buf Буфер новых данных
/// @param lba LBA-координаты блока
/// @param drive Номер диска в системе
void writeSector(byte* buf, dword lba, byte drive);

/// @brief Записывает несколько блоков данных.
/// @param buf Буфер новых данных
/// @param lba LBA-координаты первого блока
/// @param count Количество блоков
/// @param drive Номер диска в системе
void writeSectors(byte* buf, dword lba, dword count, byte drive);

#endif