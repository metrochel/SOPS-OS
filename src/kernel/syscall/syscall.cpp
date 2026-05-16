/*
 * syscall.cpp - Общие системные вызовы
 */

#include "syscall.hpp"
#include "../dbg/dbg.hpp"

#define handle(name)            __syscall_ext_hdl_decl(name)

using namespace syscalls;

handle(exit) {
    dword exit_code = (dword)arg1;

    return exit_code;
}

handle(invalid) {
    kdebug("ОШИБКА: Процесс %d попытался исполнить несуществующий системный вызов\n", pid);
    kdebug("Аргументы:\n\t%d (%x),\n\t%d (%x),\n\t%d (%x),\n\t%d (%x),\n\t%d (%x)\n",
           arg1, arg1, arg2, arg2, arg3, arg3, arg4, arg4, arg5, arg5);
    return -1;
}

#define handle_case(name) case __syscall_number_name(name): \
    kdebug("Выбранный системный вызов: " #name ".\n");      \
    return __syscall_handle_name(name);

syscall_handle_t get_syscall_handle(dword syscall) {
    switch (syscall) {
        handle_case(exit)

        handle_case(put_character)
        handle_case(clear_screen)
        handle_case(print)
        handle_case(positioned_print)
        handle_case(coloured_print)
        handle_case(positioned_coloured_print)
        handle_case(get_screen_size)
        handle_case(set_screen_bounds)
        handle_case(enable_cursor)
        handle_case(disable_cursor)

        handle_case(read_key)
        handle_case(read_str)

        handle_case(open_file)
        handle_case(close_file)
        handle_case(read)
        handle_case(read_char)
        handle_case(write)
        handle_case(write_char)
        handle_case(get_file_size)
        handle_case(remove_file)
        handle_case(move_file)

        handle_case(malloc)
        handle_case(free)
        handle_case(shell_cmd)
        handle_case(get_env_var)
        handle_case(set_env_var)
        handle_case(get_unix_time)
        handle_case(get_nanosec_time)
        handle_case(get_proc_time)

        handle_case(start_thread)
        handle_case(exit_thread)
        handle_case(get_current_thread)
        handle_case(yield)
        handle_case(join_thread)
        handle_case(block_thread)
        handle_case(timed_block_thread)
        handle_case(detach_thread)
        handle_case(create_mutex)
        handle_case(create_cond_var)
        handle_case(create_tss)
        handle_case(destroy_mutex)
        handle_case(destroy_cond_var)
        handle_case(destroy_tss)
        handle_case(get_mutex_locked)
        handle_case(lock_mutex)
        handle_case(unlock_mutex)
        handle_case(signal_cond_var)
        handle_case(broadcast_cond_var)
        handle_case(get_tss)
        handle_case(set_tss)

        default: return handle_invalid_syscall;
    }
}