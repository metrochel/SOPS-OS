/*
 * syscalls_hid_input.c - Вводящие системные вызовы
 */

#include "incl/syscalls_hid_input.hpp"


#define handle(name)    __syscall_ext_hdl_decl(name)

using namespace syscalls;

// FIXME: Модуль написан с учётом старого ядра. Переписать.

//handle(read_key) {
//    return kreadkey();
//}
//
//handle(read_str) {
//    byte *buf = (byte*)arg1;
//    word ownerPid = getBlockOwnerPID((ptrint)buf, maxInputSize);
//    if (ownerPid != pid) {
//        kdebug("ВНИМАНИЕ: Процесс %d запросил использование блока, ему не принадлежащего\n");
//        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
//        return RUNTIME_ERROR_INVALID_ACCESS;
//    }
//    kread(buf);
//    return strlen((char*)buf);
//}