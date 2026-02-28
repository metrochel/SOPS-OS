/*
 * syscall_hid_input.hpp - Системные вызовы ввода
 *
 * Объявляет системные вызовы, связанные с вводом с устройств.
 */

#ifndef _SYSCALLS_HID_INPUT_INCL
#define _SYSCALLS_HID_INPUT_INCL

#define SYSCALL_BASE    0x2000
#include "../../syscall_base.hpp"

/// В пространстве имён `syscalls` объявлены все системные вызовы.
/// Это пространство имён должно использоваться в любом файле, объявляющем системные вызовы.
namespace syscalls {
    /// @brief Чтение одной клавиши
    /// @return - Верхний байт - статус нажатия (клавиши-модификаторы: Ctrl, Alt и т.д.)
    /// @return - Нижний байт - код нажатой клавиши
    declare_syscall(read_key)

    /// @brief Чтение строки с клавиатуры
    /// @param ESI Адрес строки-буфера для данных
    /// @return Количество считанных символов
    declare_syscall(read_str)
}

#undef SYSCALL_BASE

#endif
