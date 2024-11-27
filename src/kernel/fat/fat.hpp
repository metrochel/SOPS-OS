#ifndef _FAT_INCL
#define _FAT_INCL
//
//  Драйвер для FAT32
//
//  - Предоставляет функционал, способный читать и записывать файлы в системе FAT32.
//

#include "../util/nums.hpp"
#include "../timing/time.hpp"

#define FAT_FILEATTR_READONLY   0b1
#define FAT_FILEATTR_HIDDEN     0b10
#define FAT_FILEATTR_SYSTEM     0b100
#define FAT_FILEATTR_VOLUME_ID  0b1000
#define FAT_FILEATTR_DIRECTORY  0b10000
#define FAT_FILEATTR_ARCHIVE    0b100000
#define FAT_FILEATTR_LFN        0b1111

#define is_lfn(x) x->attr == FAT_FILEATTR_LFN
#define clustersize(x) bpbs[x].bytesPerSector * bpbs[x].sectorsPerCluster

/// @brief BIOS Parameter Block - структура, описывающая базовые данные о FAT.
struct FAT_BPB {
    byte  reserved[3];          // (резервировано)
    byte  oemName[8];           // Идентификатор производителя
    word  bytesPerSector;       // Количество байтов на сектор
    byte  sectorsPerCluster;    // Количество секторов на кластер
    word  reservedSectors;      // Количество резервированных секторов
    byte  fatsCount;            // Количество таблиц FAT на диске
    word  rootEntriesCount;     // Количество вхождений в корневом каталоге
    word  oldSectCount;         // Количество секторов на диске (FAT12/FAT16)
    byte  media;                // Идентификатор носителя
    word  oldFATSize;           // Размер одной таблицы FAT (FAT12/FAT16)
    word  sectsPerTrack;        // Количество секторов на дорожку
    word  headsCount;           // Количество головок носителя
    dword hiddenSects;          // Количество скрытых секторов
    dword totalSects;           // Количество секторов на диске (FAT32/exFAT)
} __attribute__((packed));

/// @brief Extended BIOS Parameter Block - расширенный BPB для FAT32.
struct FAT32_EBPB {
    dword fatSize;              // Размер одной FAT
    word  extFlags;             // Расширенные флаги для FAT32
    word  fat32Ver;             // Версия FAT32
    dword rootCluster;          // Номер кластера корневого каталога.
    word  fsInfoSect;           // Номер сектора структуры FSInfo
    word  backupBootSect;       // Номер сектора, содержащего запасной загрузочный сектор
    byte  reserved[12];         // (резервировано)
    byte  driveNo;              // Номер диска (из INT 0x13)
    byte  reserved1;            // (резервировано)
    byte  bootSignature;        // Подпись (0x29)
    dword volumeID;             // Идентификатор тома
    byte  volumeLabel[11];      // Метка тома
    byte  flSysType[8];         // "Тип файловой системы" (на деле всегда "FAT32   ")
} __attribute__((packed));

/// @brief Файл.
class File {
    public:
        char *name;             // Имя файла
        byte attributes;        // Атрибуты файла
        dword startCluster;     // Первый кластер файла
        byte drive;             // Номер диска с файлом
        dword size;             // Размер файла в байтах
        Time creationDate;      // Дата создания файла
        Time lastEditDate;      // Дата редактирования файла

        static File construct(byte *ptr, byte driveNo);

        void read(byte *out);
};

struct FAT_DirEntry {
    byte name[11];
    byte attr;
    byte _ntRes;
    byte createTimeMS;
    word createTime;
    word createDate;
    word lastAccessDate;
    word clusterHi;
    word lastWriteTime;
    word lastWriteDate;
    word clusterLo;
    dword fileSize;
} __attribute__((packed));

struct FAT_LFNEntry {
    byte order;
    word name1[5];
    byte attr;
    byte type;
    byte chksum;
    word name2[6];
    word _reserved;
    word name3[2];
} __attribute__((packed));

extern byte bootNo;

extern FAT_BPB bpbs[16];
extern FAT32_EBPB ebpbs[16];

/// @brief Инициализирует FAT. (Ну надо же!)
/// @param driveNo Номер диска, на котором читать FAT
bool initFAT(byte driveNo);

/// @brief Считывает один кластер.
/// @param driveNo Номер диска, с которого считать кластер
/// @param clusterNo Номер кластера
/// @param out Буфер выхода данных
void readCluster(byte driveNo, dword clusterNo, byte *out);

#endif