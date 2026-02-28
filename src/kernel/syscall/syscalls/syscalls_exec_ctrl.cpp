/*
 * syscalls_exec_ctrl.cpp - Управляющие системные вызовы
 */

#include "incl/syscalls_exec_ctrl.hpp"
#include "../../run/process.hpp"
#include "../../memmgr/memmgr.hpp"
#include "../../timing/time.hpp"
#include "../../dbg/dbg.hpp"
#include "../../util/util.hpp"
#include "../../run/process.hpp"

#define handle(name)        __syscall_ext_hdl_decl(name)

using namespace syscalls;

handle(malloc) {
    ptrint alloc_size = (ptrint)arg1;

    kdebug("Процесс %d запросил %d Б памяти.\n", pid, alloc_size);
    ptrint alloc = (ptrint)kmalloc(alloc_size, pid);
    if (!alloc) {
        kdebug("ВНИМАНИЕ: Выделение памяти для процесса %d провалено", pid);
        return RUNTIME_ERROR_ALLOC_FAILURE;
    }
    return alloc;
}

handle(free) {
    void *ptr = (void*)arg1;

    kdebug("Процесс %d освобождает память по адресу %x.\n", pid, ptr);
    word owner_pid = getBlockOwnerPID((ptrint)ptr, 1);
    if (owner_pid != pid) {
        kdebug("ОШИБКА: Блок не принадлежит процессу %d\n", pid);
        kdebug(pid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n",
               owner_pid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }
    kfree((void*)arg1, pid);
    return 0;
}

handle(shell_cmd) {
    return -1;
}

handle(get_env_var) {
    return -1;
}

handle(set_env_var) {
    return -1;
}

handle(get_unix_time) {
    return kgettime().to_unix();
}

handle(get_nanosec_time) {
    return 0;
}

handle(get_proc_time) {
    Process p = getProcessData(pid);
    qword cur_timestamp = get_timestamp();
    return cur_timestamp - p.start_timestamp;
}