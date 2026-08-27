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

void initFiles();

/// @brief Открывает файл.
/// @param path Путь к файлу
/// @param driveNo Номер диска, содержащего файл
/// @return Ссылка на открытый файл
File *openFile(char *path, byte driveNo, file_open_mode mode);

/// @brief Закрывает файл.
/// @param handle Указатель на закрываемый файл
void closeFile(File *handle);

/// @brief Создаёт папку.
/// @param path Путь к папке
/// @param drive Номер диска с папкой
/// @param force Флаг; если `true`, то все папки, отсутствующие на пути, создаются
/// @return `true`, если создание удалось
bool createFolder(char *path, byte drive, bool force);

#endif