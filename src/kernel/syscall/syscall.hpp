/*
 * syscall.hpp - Системные вызовы
 *
 * Содержит все системные вызовы, обрабатываемые ОС.
 */

#ifndef _SYSCALLS_INCL
#define _SYSCALLS_INCL

#include "syscalls/incl/syscalls_graphics.hpp"
#include "syscalls/incl/syscalls_hid_input.hpp"
#include "syscalls/incl/syscalls_files.hpp"
#include "syscalls/incl/syscalls_exec_ctrl.hpp"
#include "syscalls/incl/syscalls_threads.hpp"

#define SYSCALL_BASE 0
#include "syscall_base.hpp"

/// В пространстве имён `syscalls` объявлены все системные вызовы.
/// Это пространство имён должно использоваться в любом файле, объявляющем системные вызовы.
namespace syscalls {
    /// Обработчик неизвестного системного вызова.
    __syscall_hdr_hdl_decl(invalid);
    /// Выход
    declare_syscall(exit)
}

#undef SYSCALL_BASE

/// @brief Получает значение обработчика для данного системного вызова.
/// @param syscall_no Номер системного вызова
/// @return Обработчик системного вызова
/// @note Если не найден обработчик для данного вызова, возвращается `handle_invalid_syscall`.
syscall_handle_t get_syscall_handle(dword syscall_no);

#endif
