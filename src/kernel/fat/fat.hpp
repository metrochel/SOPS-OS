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

#define FAT_CLUSTER_EOF         0xFFFFFF8
#define FAT_CLUSTER_BAD         0xFFFFFF7

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
        dword directoryCluster; // Кластер, содержащий папку, в которой лежит файл
        byte drive;             // Номер диска с файлом
        dword size;             // Размер файла в байтах
        Time creationDate;      // Дата создания файла
        Time lastEditDate;      // Дата редактирования файла

        /// @brief Конструктор по метке директории.
        /// @param ptr Указатель на метку
        /// @param driveNo Номер диска
        File(byte *ptr, byte driveNo);

        /// @brief Конструктор по непосредственным данным файла.
        /// @param name Имя файла
        /// @param attr Атрибуты файла
        /// @param drive Номер диска
        /// @param size Размер файла
        /// @param creationDate Время создания
        /// @note Очень полезно для создания виртуальных файлов, то есть таких, которых нет сейчас на диске.
        File(char *name, byte attr, byte drive, dword size, Time creationDate, dword directoryCluster);

        /// @brief Считывает весь файл в память.
        /// @param out Указатель выхода данных
        void read(byte *out);

        /// @brief Создаёт файл на диске.
        void create();
    protected:
        /// @brief Пустой конструктор.
        File();
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

/// ### FSInfo
/// Структура, описывающая важные вещи о ФС: количество свободных кластеров и кластер, 
/// с которого следует искать свободные кластеры.
struct FAT_FSInfo {
    dword leadSignature;
    byte  reserved1[480];
    dword signature1;
    dword lastFreeCount;
    dword nextFreeClus;
    byte  reserved2[12];
    dword trailSignature;
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

/// @brief Записывает один кластер.
/// @param driveNo Номер диска, на котором записывать кластер
/// @param clusterNo Номер кластера
/// @param in Буфер данных
void writeCluster(byte driveNo, dword clusterNo, byte *in);

/// @brief Выделяет один кластер.
/// @param driveNo Номер диска, где искать
/// @return Номер свободного кластера
/// @note Если не нашёлся свободный кластер, возвращается `maxdword`.
dword allocateCluster(byte driveNo);

/// @brief Извлекает, куда на текущий момент указывает кластер.
/// @param driveNo Номер диска
/// @param clusterNo Номер кластера
/// @return Значение, на которое указывает кластер
dword getCluster(byte driveNo, dword clusterNo);

/// @brief Изменяет кластер в FAT.
/// @param driveNo Номер диска
/// @param clusterNo Номер кластера
/// @param newVal Новое значение
void setCluster(byte driveNo, dword clusterNo, dword newVal);

#endif