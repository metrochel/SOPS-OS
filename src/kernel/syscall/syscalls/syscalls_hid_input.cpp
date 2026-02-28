/*
 * syscalls_hid_input.c - Вводящие системные вызовы
 */

#include "incl/syscalls_hid_input.hpp"
#include "../../keyboard/keyboard.hpp"
#include "../../dbg/dbg.hpp"
#include "../../run/process.hpp"
#include "../../str/str.hpp"

#define handle(name)    __syscall_ext_hdl_decl(name)

using namespace syscalls;

handle(read_key) {
    return kreadkey();
}

handle(read_str) {
    byte *buf = (byte*)arg1;
    word ownerPid = getBlockOwnerPID((ptrint)buf, maxInputSize);
    if (ownerPid != pid) {
        kdebug("ВНИМАНИЕ: Процесс %d запросил использование блока, ему не принадлежащего\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }
    kread(buf);
    return strlen((char*)buf);
}