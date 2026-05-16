/*
 * stdio.hpp - Стандартный ввод/вывод процессов
 *
 * Объявляет структуры, контроллирующие ввод/вывод для процесса
 * через стандартные потоки: stdin, stdout и stderr.
 */

#ifndef _STDIO_INCL
#define _STDIO_INCL

#include "../file/file.hpp"
#include "../keyboard/keyboard.hpp"

struct Process;

/// @brief stdout_file - это класс, характеризующий
/// поток стандартного вывода файла.
class stdout_file : public File {
    Process *process;                   // Текущий процесс

public:
    stdout_file(Process *process)
    : File(), process(process) {}

    virtual dword read(dword read_start, dword read_size, byte *out) override;
    virtual dword write(dword write_start, dword write_size, byte *in) override;
};

/// @brief stdin_file - это класс, характеризующий
/// поток стандартного ввода файла.
class stdin_file : public File {
    Process *process;                   // Текущий процесс

public:
    stdin_file(Process *process)
    : File(), process(process) {}

    virtual dword read(dword read_start, dword read_size, byte *out) override;
    virtual dword write(dword write_start, dword write_size, byte *in) override;
};

/// @brief stderr_file - это класс, характеризующий
/// поток стандартной ошибки файла.
class stderr_file : public File {
    Process *process;                   // Текущий процесс

public:
    stderr_file(Process *process)
    : File(), process(process) {}

    virtual dword read(dword read_start, dword read_size, byte *out) override;
    virtual dword write(dword write_start, dword write_size, byte *in) override;
};

#endif
