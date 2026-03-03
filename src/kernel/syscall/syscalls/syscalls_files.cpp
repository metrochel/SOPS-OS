/*
 * syscalls_files.cpp - Файловые системные вызовы
 */

#include "incl/syscalls_files.hpp"
#include "../../run/process.hpp"
#include "../../dbg/dbg.hpp"
#include "../../disk/disk.hpp"

#define handle(name)    __syscall_ext_hdl_decl(name)

using namespace syscalls;

inline File* get_file_pointer(word pid, word desc) {
    Process p = getProcessData(pid);
    if (desc >= 3 + PROC_MAX_FILES)
        return nullptr;
    if (desc == 0)
        return p.stdout;
    if (desc == 1)
        return p.stdin;
    if (desc == 2)
        return p.stderr;
    word handle_slot = desc - 3;
    return p.files[handle_slot];
}

handle(open_file) {
    char *path = (char*)arg1;
    byte mode = arg2;
    byte drive = determineDriveNo(path);

    Process p = getProcessData(pid);

    word handle_slot = maxword;
    for (word i = 0; i < MAX_FILE_HANDLES; i++) {
        if (!p.files[i]) {
            handle_slot = i;
            break;
        }
    }

    if (handle_slot == maxword) {
        kdebug("ОШИБКА: Не удалось выделить слот для файла\n");
        kdebug("Похоже, процесс открыл слишком много файлов.\n");
        return RUNTIME_ERROR_FILE_MAX_REACHED;
    }

    File *file_ptr = openFile(path, drive, mode);
    if (!file_ptr) {
        kdebug("ОШИБКА: Не удалось открыть файл\n");
        return RUNTIME_ERROR_FILE_OPEN_FAILURE;
    }

    p.files[handle_slot] = file_ptr;
    setProcessData(pid, p);

    return handle_slot + 3;
}

handle(close_file) {
    word desc = (word)arg1;

    kdebug("Процесс %d закрывает файл с дескриптором %d.\n", pid, desc);
    if (desc < 3) {
        kdebug("ОШИБКА: Нельзя закрыть стандартный поток\n");
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    closeFile(file_ptr);
    word handle_slot = desc - 3;
    Process p = getProcessData(pid);
    p.files[handle_slot] = nullptr;
    setProcessData(pid, p);

    return 0;
}

handle(read) {
    byte *out = (byte*)arg1;
    dword data_size = (dword)arg2;
    dword read_start = (dword)arg3;
    word desc = (word)arg4;

    kdebug("Процесс %d считывает %d Б из файла № %d.\n", pid, data_size, desc);
    kdebug("Выделенный буфер: %x.\n", out);

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }

    return file_ptr->read(read_start, data_size, out);
}

handle(read_char) {
    return 0;
}

handle(write) {
    byte *in = (byte*)arg1;
    dword size = (dword)arg2;
    _mb_unused dword write_start = (dword)arg3;
    word desc = (word)arg4;

    kdebug("Процесс %d записывает %d Б в файл № %d.\n", pid, size, desc);
    kdebug("Выделенный буфер: %x.\n", in);

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }

    return file_ptr->write(write_start, size, in);
}

handle(write_char) {
    return -1;
}

handle(get_file_size) {
    word desc = (word)arg1;

    File *ptr = get_file_pointer(pid, desc);
    if (!ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }
    return ptr->size;
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