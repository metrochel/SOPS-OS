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

/*
 * Режим открытия файла - это 5-битовое число, имеющее следующий вид:
 * 0b000XUBMM,
 * где
 *  M - сам режим открытия (0 - чтение, 1 - запись, 2 - добавление),
 *  B - флаг двоичного открытия (0 - текст, 1 - бинарник),
 *  U - флаг обновления (0 - только в одну сторону, 1 - можно в обе),
 *  X - флаг эксклюзивности (т.е. если такой файл уже есть на диске, открытие проваливается)
 */

#define FILE_MODE_READ      0
#define FILE_MODE_WRITE     1
#define FILE_MODE_APPEND    2
#define FILE_MODE_FLAG_BIN  8
#define FILE_MODE_FLAG_UPD  16
#define FILE_MODE_FLAG_EX   32

void initFiles();

/// @brief Открывает файл.
/// @param path Путь к файлу
/// @param driveNo Номер диска, содержащего файл
/// @return Ссылка на открытый файл
File *openFile(char *path, byte driveNo, byte mode);

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