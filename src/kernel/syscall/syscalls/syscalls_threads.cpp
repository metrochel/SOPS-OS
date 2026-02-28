/*
 * syscalls_threads.cpp - Поточные системные вызовы
 */

#include "incl/syscalls_threads.hpp"

#define handle(name)            __syscall_ext_hdl_decl(name)

using namespace syscalls;

/* TODO
 * Так как система является однопоточной, на текущий момент все эти системные вызовы
 * ничего не делают. Исправить.
 */

handle(start_thread) {
    return 0;
}

handle(exit_thread) {
    return 0;
}

handle(get_current_thread) {
    return 0;
}

handle(yield) {
    return 0;
}

handle(join_thread) {
    return -maxdword;
}

handle(block_thread) {
    return -1;
}

handle(timed_block_thread) {
    return -1;
}

handle(detach_thread) {
    return 0;
}

handle(create_mutex) {
    return 0;
}

handle(create_cond_var) {
    return 0;
}

handle(create_tss) {
    return 0;
}

handle(destroy_mutex) {
    return 0;
}

handle(destroy_cond_var) {
    return 0;
}

handle(destroy_tss) {
    return 0;
}

handle(get_mutex_locked) {
    return 1;
}

handle(lock_mutex) {
    return 0;
}

handle(unlock_mutex) {
    return 0;
}

handle(signal_cond_var) {
    return 0;
}

handle(broadcast_cond_var) {
    return 0;
}

handle(get_tss) {
    return (syscall_ret_t)nullptr;
}

handle(set_tss) {
    return 0;
}