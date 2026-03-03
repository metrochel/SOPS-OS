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

        byte *single_chars_buf;     // Буфер для односимвольных манипуляций
        dword single_char_buf_idx;  // Индекс в буфере для односимвольных манипуляций
        dword single_char_buf_pos;  // Положение буфера относительно начала файла

        /// @brief Создаёт файл на диске.
        virtual bool create();

        /// @brief Считывает весь файл в память.
        /// @param out Указатель выхода данных
        /// @return Размер считанного блока, Б
        virtual dword read(byte *out);

        /// @brief Считывает кусок из файла в память.
        /// @param read_start Сдвиг первого байта
        /// @param read_size Размер считываемого куска, Б
        /// @param out Указатель выхода данных
        /// @return Размер считанного блока, Б
        virtual dword read(dword read_start, dword read_size, byte *out);

        /// @brief Записывает данные в файл.
        /// @param write_start Сдвиг первого байта
        /// @param write_size Размер данных, Б
        /// @param in Указатель входа данных
        /// @return Размер записанного блока, Б
        virtual dword write(dword write_start, dword write_size, byte *in);

        /// @brief Записывает данные в файл.
        /// @param write_size Размер данных, Б
        /// @param in Указатель входа данных
        /// @return Размер записанного блока, Б
        /// @note Запись производится в конец файла.
        virtual dword write(dword write_size, byte *in);

        /// @brief Переименовывает файл.
        /// @param newname Новое имя файла
        virtual void rename(char *newname);

        /// @brief Удаляет файл с диска.
        virtual void remove();

        /// @brief Очищает содержимое файла, но не удаляет файл.
        virtual void clear();

        bool operator!();

        File() : name(nullptr), attributes(0), drive(maxbyte), size(0),
                 creationDate(Time()), lastEditDate(Time()),
                 single_chars_buf(nullptr), single_char_buf_idx(0), single_char_buf_pos(0) {}
};

#endif