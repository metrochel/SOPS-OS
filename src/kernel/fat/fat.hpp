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
#define FAT_CLUSTER_FREE        0x0000000

#define is_lfn(x) ((x).attr == FAT_FILEATTR_LFN)
#define is_eof(x) (x >= FAT_CLUSTER_EOF)
#define is_last(x) ((x).name[0] == 0x00)

#define clustersize(x) (bpbs[x].bytesPerSector * bpbs[x].sectorsPerCluster)
#define root(x) (ebpbs[x].rootCluster)
#define freeEntry(x) x.name[0] = 0xE5

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

/// ### File
/// Этот класс описывает данные о файле и позволяет с ним взаимодействовать
/// (читать, создавать, записывать и др.).
class File {
    public:
        char *name;             // Имя файла
        byte attributes;        // Атрибуты файла
        dword startCluster;     // Первый кластер файла
        dword directoryCluster; // Кластер, содержащий папку, в которой лежит файл
        word dirEntryOffset;    // Сдвиг по кластеру папки, на котором находится главная метка для данного файла
        byte drive;             // Номер диска с файлом
        dword size;             // Размер файла в байтах
        Time creationDate;      // Дата создания файла
        Time lastEditDate;      // Дата редактирования файла
        bool text = true;       // Флаг; `true`, если файл открыт в текстовом режиме

        /// @brief Конструктор по метке директории.
        /// @param cluster Кластер с меткой
        /// @param offset Сдвиг метки
        /// @param driveNo Номер диска метки
        /// @attention Сдвиг указывать на первую LFN-метку файла!
        File(dword cluster, word offset, byte driveNo);

        /// @brief Конструктор по непосредственным данным файла.
        /// @param name Имя файла
        /// @param attr Атрибуты файла
        /// @param drive Номер диска
        /// @param size Размер файла
        /// @param creationDate Время создания
        /// @note Очень полезно для создания виртуальных файлов, то есть таких, которых нет сейчас на диске.
        File(char *name, byte attr, byte drive, dword size, Time creationDate, dword directoryCluster);

        /// @brief Конструктор по пути к файлу.
        /// @param path Абсолютный путь к файлу
        /// @param driveNo Номер диска 
        /// @param force Флаг; если `true`, то все отсутствующие по пути папки создаются автоматически
        File(char *path, byte driveNo, bool force);

        /// @brief Деструктор файла.
        ~File();

        /// @brief Создаёт файл на диске.
        void create();

        /// @brief Считывает весь файл в память.
        /// @param out Указатель выхода данных
        void read(byte *out);

        /// @brief Записывает данные в файл.
        /// @param in Указатель входа данных
        /// @param dataSize Размер данных, Б
        void write(byte *in, dword dataSize);

        /// @brief Переименовывает файл.
        /// @param newname Новое имя файла
        void rename(char *newname);

        /// @brief Удаляет файл с диска.
        void remove();
    protected:
        /// @brief Пустой конструктор.
        File();
};

// ### FAT_DirEntry
// Эта структура описывает одну метку файла внутри папки.
struct FAT_DirEntry {
    byte name[11];          // "Короткое" имя
    byte attr;              // Аттрибуты файла
    byte _ntRes;            // (резервировано)
    byte createTimeMS;      // Миллисекунды создания файла / 10, 0-199
    word createTime;        // Время создания файла
    word createDate;        // Дата создания файла
    word lastAccessDate;    // Дата последнего доступа к файлу
    word clusterHi;         // Высшие 16 бит первого кластера файла
    word lastWriteTime;     // Время последней записи
    word lastWriteDate;     // Дата последней записи
    word clusterLo;         // Низшие 16 бит первого кластера файла
    dword fileSize;         // Размер файла, Б
} __attribute__((packed));

// ### FAT_LFNEntry
// Эта структура описывает одну LFN-метку внутри файла
struct FAT_LFNEntry {
    byte order;         // Номер (если установлен бит 6, то метка последняя)
    word name1[5];      // 5 символов
    byte attr;          // Аттрибуты (0x0F)
    byte type;          // Тип (резервировано на 0)
    byte chksum;        // Контрольная сумма
    word name2[6];      // 6 символов
    word _reserved;     // (резервировано)
    word name3[2];      // 2 символа
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

/// @brief Обновляет метку директории для файла.
/// @param f Файл
void updateDirEntry(File *f);

/// @brief Создаёт SFN-имя для данного названия
/// @param name Исходное название
/// @param out Указатель на буфер выхода
/// @return Флаг; `true`, если у файла есть расширение
bool createSFNName(char *name, char *out);

/// @brief Извлекает имя файла из LFN-меток.
/// @param lfn Указатель на метки
/// @param out Указатель на имя файла
void extractLFNName(FAT_LFNEntry *lfn, byte *out);

/// @brief Извлекает имя файла из главной метки файла.
/// @param entry Метка
/// @param out Указатель на имя файла
void extractShortName(FAT_DirEntry entry, byte *&out);

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

/// @brief Определяет длину цепи кластеров.
/// @param startCluster Начальный кластер
/// @return Длина цепи кластеров
dword getClusterChainLength(dword startCluster, byte drive);

#endif