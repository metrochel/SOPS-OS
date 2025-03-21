#ifndef _FILE_INCL
#define _FILE_INCL
//
//  Библиотека файлов
//
//  - Общие функции, связанные с управлением файлов.
//

#include "filebase.hpp"
#include "fat.hpp"

#define MAX_FILE_HANDLES 256

#define FILE_MODE_READ      0
#define FILE_MODE_WRITE     1
#define FILE_MODE_APPEND    2
#define FILE_MODE_FLAG_BIN  8
#define FILE_MODE_FLAG_UPD  16
#define FILE_MODE_FLAG_EX   32

// ### FileHandle
// Обработчик данных файла.
// Предоставляет данные о потоке данных, созданным
// открытым файлом.
struct FileHandle {
    File *file;
    qword filePos;
    byte *buffer;
    dword bufferSize;
    byte mode;
    byte error  : 1;
    byte eof    : 1;
};

void initFiles();

/// @brief Открывает файл.
/// @param path Путь к файлу
/// @param driveNo Номер диска, содержащего файл
/// @return Ссылка на открытый файл
FileHandle *openFile(char *path, byte driveNo, byte mode);

/// @brief Закрывает файл.
/// @param handle Указатель на закрываемый `FileHandle`
void closeFile(FileHandle *handle);

/// @brief Считывает из файла кусок.
/// @param handle Указатель на обработчик файла
/// @param size Размер куска, Б
/// @param out Указатель на выход данных
/// @return `true`, если чтение удалось
bool readFile(FileHandle *handle, dword size, byte *out);

/// @brief Записывает кусок данных в файл.
/// @param handle Указатель на обработчик файла
/// @param size Размер куска, Б
/// @param in Указатель на кусок
/// @return `true`, если чтение удалось
bool writeFile(FileHandle *handle, dword size, byte *in);

/// @brief Создаёт папку.
/// @param path Путь к папке
/// @param drive Номер диска с папкой
/// @param force Флаг; если `true`, то все папки, отсутствующие на пути, создаются
/// @return `true`, если создание удалось
bool createFolder(char *path, byte drive, bool force);

#endif