/*
 * syscalls_files.cpp - Файловые системные вызовы
 */

#include "incl/syscalls_files.hpp"
#include "../../run/process.hpp"
#include "../../dbg/dbg.hpp"
#include "../../disk/disk.hpp"

#define handle(name)    __syscall_ext_hdl_decl(name)

using namespace syscalls;

handle(open_file) {
    Process p = getProcessData(pid);
    word handleSlot = maxword;
    for (byte i = 0; i < PROC_MAX_FILES; i++) {
        if (!p.handles[i]) {
            handleSlot = i;
            break;
        }
    }
    if (handleSlot >= PROC_MAX_FILES) {
        kdebug("ВНИМАНИЕ: Процесс %d попытался открыть больше, чем %d файлов, одновременно", pid, PROC_MAX_FILES);
        return RUNTIME_ERROR_FILE_MAX_REACHED;
    }

    char *path = (char*)arg1;
    byte mode = arg2;
    byte drive = determineDriveNo(path);
    while (*path != '}') {path ++;}
    path ++;
    FileHandle *handle = openFile(path, drive, mode);
    if (!handle) {
        kdebug("ВНИМАНИЕ: Открытие файла провалено\n");
        return RUNTIME_ERROR_FILE_OPEN_FAILURE;
    }
    p.handles[handleSlot] = handle;
    setProcessData(pid, p);
    return handleSlot;
}

handle(close_file) {
    FileHandle *handle = (FileHandle*)arg1;
    word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
    if (ownerPid != PID_KERNEL) {
        kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }
    closeFile(handle);
    return 0;
}

handle(read) {
    byte *out = (byte*)arg1;
    dword size = arg2;
    FileHandle *handle = (FileHandle*)arg3;

    word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
    if (ownerPid != PID_KERNEL) {
        kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    ownerPid = getBlockOwnerPID(arg1, size);
    if (ownerPid != pid) {
        kdebug("ОШИБКА: Блок не принадлежит процессу\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    bool res = readFile(handle, size, out);
    if (!res) {
        kdebug("ОШИБКА: Чтение из файла провалено\n");
        return RUNTIME_ERROR_FILE_READ_FAILURE;
    }
    return 0;
}

handle(read_char) {
    return 0;
}

handle(write) {
    byte *in = (byte*)arg1;
    dword size = arg2;
    FileHandle *handle = (FileHandle*)arg3;
    word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
    if (ownerPid != PID_KERNEL) {
        kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    ownerPid = getBlockOwnerPID(arg1, size);
    if (ownerPid != pid) {
        kdebug("ОШИБКА: Блок не принадлежит процессу\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }
    bool res = writeFile(handle, size, in);
    if (!res) {
        kdebug("ОШИБКА: Запись в файл провалена\n");
        return RUNTIME_ERROR_FILE_WRITE_FAILURE;
    }
    return 0;
}

handle(write_char) {
    return -1;
}

handle(get_file_size) {
    FileHandle *handle = (FileHandle*)arg1;
    word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
    if (ownerPid != PID_KERNEL) {
        kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
        kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }
    return handle->file->size;
}

handle(allocate_tmp_file) {
    return 0xABCDE;
}

handle(remove_file) {
    return -1;
}

handle(move_file) {
    return 0;
}