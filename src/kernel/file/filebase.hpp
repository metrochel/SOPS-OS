#ifndef _FILE_BASE_INCL
#define _FILE_BASE_INCL

#include "../util/nums.hpp"
#include "../timing/time.hpp"

extern const dword defaultBufSize;

// ### File
// Класс, описывающий базовый файл.
// Предоставляет функции чтения/записи, которые впоследствии
// расширяются функциями, специфичными для каждой файловой системы
// (FAT32, NTFS, EXT и др.)
class File {
    public:
        char *name;                 // Имя файла
        byte attributes;            // Атрибуты файла
        byte drive;                 // Номер диска с файлом
        dword size;                 // Размер файла в байтах
        Time creationDate;          // Дата создания файла
        Time lastEditDate;          // Дата редактирования файла

        /// @brief Создаёт файл на диске.
        virtual void create();

        /// @brief Считывает весь файл в память.
        /// @param out Указатель выхода данных
        virtual bool read(byte *out);

        /// @brief Считывает кусок из файла в память.
        /// @param start Сдвиг первого байта
        /// @param size Размер считываемого куска, Б
        /// @param out Указатель выхода данных
        virtual bool read(dword start, dword size, byte *out);

        /// @brief Записывает данные в файл.
        /// @param in Указатель входа данных
        /// @param dataSize Размер данных, Б
        virtual bool write(byte *in, dword dataSize);

        /// @brief Переименовывает файл.
        /// @param newname Новое имя файла
        virtual void rename(char *newname);

        /// @brief Удаляет файл с диска.
        virtual void remove();

        /// @brief Очищает содержимое файла, но не удаляет файл.
        virtual void clear();

        bool operator!();
};

#endif