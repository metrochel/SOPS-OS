/*
 * syscalls_files.hpp - Файловые системные вызовы
 *
 * Объявляет системные вызовы, связанные с файлами.
 */

#ifndef _SYSCALLS_FILES_INCL
#define _SYSCALLS_FILES_INCL

#define SYSCALL_BASE    0x3000
#include "../../syscall_base.hpp"

/// В пространстве имён `syscalls` объявлены все системные вызовы.
/// Это пространство имён должно использоваться в любом файле, объявляющем системные вызовы.
namespace syscalls {
    /// @brief Открытие файла
    /// @param ESI Указатель на строку-путь к файлу
    /// @param EDI Режим открытия файла
    /// @return Дескриптор открытого файла
    declare_syscall(open_file)

    /// @brief Закрытие файла
    /// @param ESI Дескриптор закрываемого файла
    declare_syscall(close_file)

    /// @brief Чтение куска файла в память
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер читаемого блока, Б
    /// @param ECX Начало считываемого блока
    /// @param EDX Дескриптор файла
    /// @return Размер считанного блока, Б
    declare_syscall(read)

    /// @brief Чтение одного символа из файла в память
    /// @param ESI Сдвиг символа по файлу, Б
    /// @param EDI Дескриптор файла
    /// @return Считанный символ или -1 в случае провала
    declare_syscall(read_char)

    /// @brief Запись данных в файл
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер записываемого блока, Б
    /// @param ECX Начало записываемого блока
    /// @param EDX Дескриптор файла
    /// @return Размер записанного блока, Б
    declare_syscall(write)

    /// @brief Запись одного символа в файл
    /// @param ESI Записываемый символ
    /// @param EDI Сдвиг записываемого символа
    /// @param ECX Дескриптор файла
    declare_syscall(write_char)

    /// @brief Получение размера файла
    /// @param ESI Дескриптор файла
    /// @return Размер файла, Б
    declare_syscall(get_file_size)

    /// @brief Выделение числа под временный файл
    /// @return Число от 0x00001 до 0xFFFFF или -1 в случае провала
    /// @note Для формата временного файла см. src/libc/stdio.h.
    declare_syscall(allocate_tmp_file)

    /// @brief Удаление файла
    /// @param ESI Название удаляемого файла
    /// @return 0 в случае успеха
    declare_syscall(remove_file)

    /// @brief Перемещение файла
    /// @param ESI Название старого файла
    /// @param EDI Название нового файла
    /// @return 0 в случае успеха
    declare_syscall(move_file)
}

#undef SYSCALL_BASE

#endif
