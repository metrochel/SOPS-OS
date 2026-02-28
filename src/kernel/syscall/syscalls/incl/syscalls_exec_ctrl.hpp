/*
 * syscalls_exec_ctrl.hpp - Управляющие системные вызовы
 *
 * Объявляет системные вызовы, связанные с общим управлением работы программы.
 */

#ifndef _SYSCALLS_EXEC_CTRL_INCL
#define _SYSCALLS_EXEC_CTRL_INCL

#define SYSCALL_BASE    0x4000
#include "../../syscall_base.hpp"

/// В пространстве имён `syscalls` объявлены все системные вызовы.
/// Это пространство имён должно использоваться в любом файле, объявляющем системные вызовы.
namespace syscalls {
    /// @brief Выделение памяти для процесса
    /// @param ESI Размер нужного блока, Б
    /// @return Указатель на блок
    declare_syscall(malloc)

    /// @brief Освобождение памяти, используемой процессом
    /// @param ESI Адрес освобождаемого блока
    declare_syscall(free)

    /// @brief Вызов команды в обработчике
    /// @param ESI Указатель на строку-команду
    /// @return Код выхода команды
    declare_syscall(shell_cmd)

    /// @brief Получение переменной окружения
    /// @param ESI Название переменной
    /// @return Значение переменной
    declare_syscall(get_env_var)

    /// @brief Установка значения переменной окружения
    /// @param ESI Название переменной
    /// @param EDI Значение переменной (строка)
    /// @return 0 в случае успеха
    declare_syscall(set_env_var)

    /// @brief Время в формате UNIX Timestamp
    /// @return Время в формате UNIX Timestamp
    declare_syscall(get_unix_time)

    /// @brief Время с точностью до наносекунды (от 0 до 1 с)
    /// @return Время с точностью до наносекунды (от 0 до 1 с)
    declare_syscall(get_nanosec_time)

    /// @brief Время исполнения процесса (в тактах процессора)
    /// @return Время исполнения процесса (в тактах процессора)
    declare_syscall(get_proc_time)
}

#undef SYSCALL_BASE

#endif
