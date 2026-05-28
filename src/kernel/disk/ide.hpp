//
//  Драйвер IDE
//
//  Работает с IDE-дисками. (Я уже даже не знаю, как тут шутить.)
//

#ifndef _IDE_INCL
#define _IDE_INCL

#include "../libk/nums.hpp"

#define ATA_DATA_PRIMARY                    0x1F0
#define ATA_ERROR_PRIMARY                   0x1F1
#define ATA_FEATURES_PRIMARY                0x1F1
#define ATA_SECTOR_COUNT_PRIMARY            0x1F2
#define ATA_LBA_LOW_PRIMARY                 0x1F3
#define ATA_LBA_MID_PRIMARY                 0x1F4
#define ATA_LBA_HIGH_PRIMARY                0x1F5
#define ATA_DRIVE_HEAD_PRIMARY              0x1F6
#define ATA_STATUS_PRIMARY                  0x1F7
#define ATA_COMMAND_PRIMARY                 0x1F7
#define ATA_ALT_STATUS_PRIMARY              0x3F6
#define ATA_DRIVE_CONTROL_PRIMARY           0x3F6
#define ATA_DRIVE_ADDRESS_PRIMARY           0x3F7

#define ATA_DATA_SECONDARY                  0x170
#define ATA_ERROR_SECONDARY                 0x171
#define ATA_FEATURES_SECONDARY              0x171
#define ATA_SECTOR_COUNT_SECONDARY          0x172
#define ATA_LBA_LOW_SECONDARY               0x173
#define ATA_LBA_MID_SECONDARY               0x174
#define ATA_LBA_HIGH_SECONDARY              0x175
#define ATA_DRIVE_HEAD_SECONDARY            0x176
#define ATA_STATUS_SECONDARY                0x177
#define ATA_COMMAND_SECONDARY               0x177
#define ATA_ALT_STATUS_SECONDARY            0x376
#define ATA_DRIVE_CONTROL_SECONDARY         0x376
#define ATA_DRIVE_ADDRESS_SECONDARY         0x377

#define ATA_STATUS_ERROR                    1
#define ATA_STATUS_INDEX                    2
#define ATA_STATUS_CORRECT                  4
#define ATA_STATUS_DRQ                      8
#define ATA_STATUS_SERVICE_RQ               16
#define ATA_STATUS_DRIVE_FAULT              32
#define ATA_STATUS_READY                    64
#define ATA_STATUS_BUSY                     128

#define IDE_COMMAND_PRIMARY                 0x100
#define IDE_STATUS_PRIMARY                  0x102
#define IDE_PRDT_ADDR_PRIMARY               0x104
#define IDE_COMMAND_SECONDARY               0x108
#define IDE_STATUS_SECONDARY                0x10A
#define IDE_PRDT_ADDR_SECONDARY             0x10C

#define SECT_SIZE                           512


// Структура дескриптора физического адреса
struct PRD {
    dword base;      // Физический адрес буфера данных
    word count;     // Число байтов для считывания (0 = 64к)
    byte reserved;   // Резервирован
    byte msb;        // Старший байт; установлен, если этот PRD последний в таблице
} __attribute__ ((packed));

// Таблица PRD для 1 канала
extern PRD* prdt1;
extern PRD* prdt1base;
// Таблица PRD для 2 канала
extern PRD* prdt2;
extern PRD* prdt2base;

/// @brief ШУФ-ключ контроллера IDE
extern dword ideCon;

// Флаг; если установлен, PRDT1 предназначена для чтения
extern bool prdt1read;
// Флаг; если установлен, PRDT2 предназначена для чтения
extern bool prdt2read;
// Флаг; если установлен, на данный момент происходит обмен данными
extern bool transferring;
// Сектор, с которого начата текущая передача данных
extern dword transfer_start;
// Количество фактически переданных секторов
extern dword transferred_sectors;
/// @brief Инициализирует IDE-контроллер.
bool initIDE();

/// @brief Считывает секторы с ATA-диска.
/// @param startLBA LBA начала
/// @param sectorsCount Число секторов
/// @param driveNo Номер диска
/// @param out Буфер приёма
/// @return Количество фактически считанных секторов
dword readSectorsATA(dword startLBA, byte sectorsCount, byte driveNo, byte *out);

/// @brief Записывает секторы на ATA-диск.
/// @param startLBA LBA начала
/// @param sectorsCount Число секторов
/// @param driveNo Номер диска
/// @param out Буфер данных
/// @return Количество фактически записанных секторов
dword writeSectorsATA(dword startLBA, byte sectorsCount, byte driveNo, byte *out);

/// @brief Очищает таблицу PRD для 1 канала.
void cleanPRDT1();

/// @brief Очищает таблицу PRD для 2 канала.
void cleanPRDT2();

#endif