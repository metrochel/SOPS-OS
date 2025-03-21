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

enum DiskBus {
    NotConnected = 0,
    ATA = 1,
    ATAPI = 2,
    SATA = 3,
    SATAPI = 4,
    NVMe = 5,
    USBFlash = 6,
    Floppy = 7,
    USBFloppy = 8
};

enum DiskFilesystem {
    Unrecognized = 0,
    FAT8 = 1,
    FAT12 = 2,
    FAT16 = 3,
    FAT32 = 4,
    exFAT = 5,
    NTFS = 6,
    ext = 7
};

// ### DiskData
// Содержит данные о логическом диске.
struct DiskData {
    DiskBus diskBus;                // Тип шины диска
    word diskOffset;                // Номер диска в шине
    int diskIdentifier;             // Идентификатор диска (буква)
    DiskFilesystem filesystem;      // Тип файловой системы диска
    word diskUnitSize;              // Размер наименьшей единицы диска (сектора)
    qword diskStartUnit;            // Первый сектор логического диска на физическом диске
    qword diskEndUnit;              // Последний сектор логического диска на физическом диске
};

extern DiskData *disks;

/// @brief Инициализирует все диски в системе.
void initDisks();

/// @brief Определяет номер логического диска по данному пути.
/// @param path Путь
/// @return Определённый номер диска
byte determineDriveNo(char *path);

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