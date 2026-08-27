#ifndef _RUN_INCL
#define _RUN_INCL
//
//  Запуск программ
//
//  - Библиотека для запуска пользовательских программ.
//

#include "../file/fat.hpp"

#define ELF_MAGIC                   0x464C457F

#define ELF_SECTION_SYMTAB          2
#define ELF_SECTION_STRTAB          3
#define ELF_SECTION_RELOCATE        9

#define RUN_ERR_NO_PROGRAM_TABLE    0xFFFFFFF0
#define RUN_ERR_NO_SECTION_TABLE    0xFFFFFFF1
#define RUN_ERR_INVALID_ENCODING    0xFFFFFFF2
#define RUN_ERR_BIG_ENDIAN          0xFFFFFFF3
#define RUN_ERR_BAD_FILE_TYPE       0xFFFFFFF4
#define RUN_ERR_BAD_RELOC_TYPE      0xFFFFFFF5
#define RUN_ERR_BAD_RELOC_SECTION   0xFFFFFFF6
#define RUN_ERR_BAD_ENTRY_POINT     0xFFFFFFF7
#define RUN_ERR_PID_ALLOC_FAILURE   0xFFFFFFF8

#define RUN_STACK_SIZE              4096

typedef dword (*EntryPoint)(int, char**);

// ### ELF_Header32
// Эта структура описывает заголовок 32-битного ELF-файла.
struct ELF_Header32 {
    dword elfMagic;             // "Волшебное число ELF" (0x464C457F)
    byte fileClass;             // Класс файла
    byte encoding;              // Тип кодирования данных
    byte elfVersion;            // Версия ELF-файла
    byte ABIid;                 // Идентификатор ОС (увы, у СОпС его нет)
    byte _reserved[8];          // (резервировано)
    word type;                  // Тип файла
    word machine;               // Тип компьютера
    dword version;              // Версия объектного файла (>= 0)
    dword entryPoint;           // Адрес точки входа в программу
    dword programHeaderOff;     // Сдвиг программного заголовка в файле
    dword sectionHeaderOff;     // Сдвиг заголовка таблицы секций в файле
    dword flags;                // Флаги
    word elfHeaderSize;         // Размер ELF-заголовка
    word phEntrySize;           // Размер одной метки в таблице программы
    word phEntryCount;          // Количество меток в таблице программы
    word shEntrySize;           // Размер одной метки в таблице секций
    word shEntryCount;          // Количество меток в таблице секций
    word strTableIndex;         // Номер метки в таблице, соответствующей таблице имён секций
} __attribute__((packed));

// ### ELF_ProgramHeader32
// Эта структура описывает одну метку в программной таблице в 32-битном ELF-файле.
struct ELF_ProgramHeader32 {
    dword type;         // Тип метки
    dword offset;       // Сдвиг фрагмента по файлу
    dword vAddr;        // Виртуальный адрес, куда грузить фрагмент
    dword pAddr;        // Физический адрес, куда грузить фрагмент (не используется)
    dword fileSize;     // Размер фрагмента в файле, Б
    dword memSize;      // Размер фрагмента, который он займёт в памяти, Б
    dword flags;        // Флаги
    dword align;        // Равнение фрагмента
} __attribute__((packed));

// ### ELF_SectionHeader32
// Эта структура описывает одну метку в таблице секций в 32-битном ELF-файле.
struct ELF_SectionHeader32 {
    dword name;         // Имя секции (индекс в таблице строк)
    dword type;         // Тип секции
    dword flags;        // Флаги
    dword addr;         // Адрес, на который погружать секцию (0, если погружать не нужно)
    dword offset;       // Сдвиг секции в файле, Б
    dword size;         // Размер секции, Б
    dword link;         // Информация о линковании данной секции (TODO)
    dword info;         // Информация о секции
    dword addrAlign;    // Равнение адреса
    dword entrySize;    // Размер метки, Б
} __attribute__((packed));

// ### ELF_SymTabEntry32
// Эта структура описывает одну метку в таблице символов в 32-битном ELF-файле.
struct ELF_SymTabEntry32 {
    dword name;         // Имя символа (индекс в таблице строк)
    dword value;        // Значение символа
    dword size;         // Размер символа
    byte info;          // Информация о символе
    byte other;         // Видимость символа
    word shIndex;       // Номер секции для данного символа
} __attribute__((packed));

// ### ELF_RelocateData32
// Эта структура описывает данные о перемещении одного поля в 32-битном ELF-файле.
struct ELF_RelocateData32 {
    dword offset;           // Сдвиг, по которому записывать перемещённое значение
    byte type;              // Тип перемещения
    byte symbol;            // Символ перемещения
    word _reserved;         // (резервировано)
} __attribute__((packed));

/// @brief Запускает исполняемый файл.
/// @param file Файл
/// @param args Строка запуска файла
/// @param path Рабочая папка
/// @return Код выхода программы
dword runExecutable(FAT32_File file, char *args, char *path);

/// @brief Запускает перемещаемый ELF-файл.
/// @param file Указатель на данные файла
/// @param pid PID запускаемого файла
/// @param args Строка запуска файла
/// @return Код выхода программы
dword runRelocatableELF(byte *file, word pid, char *args);

/// @brief Запускает загружаемый ELF-файл.
/// @param file Указатель на данные файла
/// @param pid PID запускаемого файла
/// @param args Строка запуска файла
/// @return Код выхода программы
dword runLoadableELF(byte *file, word pid, char *args);

/// @brief Запускает динамично компонуемый ELF-файл.
/// @param file Указатель на данные файла
/// @param pid PID запускаемого файла
/// @param args Строка запуска файла
/// @return Код выхода программы
dword runDynamicELF(byte *file, word pid, char *args);

#endif