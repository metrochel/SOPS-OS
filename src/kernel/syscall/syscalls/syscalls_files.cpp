/*
 * syscalls_files.cpp - Файловые системные вызовы
 */

#include "incl/syscalls_files.hpp"
#include "../../run/process.hpp"
#include "../../dbg/dbg.hpp"
#include "../../disk/disk.hpp"
#include "../../str/str.hpp"
#include "../../shell/shell.hpp"
#include "../../memmgr/memmgr.hpp"

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
    const char *relative_path = (const char*)arg1;
    byte mode = arg2;

    char *path;
    if (*relative_path == '/') {
        path = (char*)relative_path;
    } else {
        dword path_sz = strlen(current_dir);
        if (*(current_dir + path_sz - 1) == '/') {
            path = (char*)kmalloc(path_sz + 1);
            strcpy(current_dir, path);
        } else {
            strconcat(current_dir, "/", path);
        }
        char *tmp_path = path;
        strconcat(tmp_path, relative_path, path);
        kfree(tmp_path);
    }

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

    union {
        int number;
        file_open_mode _mode;
    } uni;
    uni.number = mode;

    File *file_ptr = openFile(path, drive, uni._mode);
    if (!file_ptr) {
        kdebug("ОШИБКА: Не удалось открыть файл\n");
        kdebugwait();
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
    dword size = (dword)arg2;
    dword read_start = (dword)arg3;
    word desc = (word)arg4;

    kdebug("Процесс %d считывает %d Б из файла № %d.\n", pid, size, desc);
    kdebug("Выделенный буфер: %x.\n", out);

    word owner_pid = getBlockOwnerPID((ptrint)out, size);
    if (owner_pid != pid) {
        kdebug("ОШИБКА: Процесс использует блок, не принадлежащий ему\n");
        kdebug(owner_pid != maxword ?
               "Блок принадлежит процессу %d.\n" : "Блок принадлежит нескольким процессам.\n", owner_pid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }

    return file_ptr->read(read_start, size, out);
}

handle(read_char) {
    dword read_start = (dword)arg1;
    word desc = (word)arg2;

    kdebug("Процесс %d считывает символ из файла № %d.\n", pid, desc);

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }

    byte c = 0;
    dword read = file_ptr->read(read_start, 1, &c);
    kdebug("Считанный символ: %c (%x).\n", c, c);
    return read == 1 ? c : -1;
}

handle(write) {
    byte *in = (byte*)arg1;
    dword size = (dword)arg2;
    dword write_start = (dword)arg3;
    word desc = (word)arg4;

    kdebug("Процесс %d записывает %d Б в файл № %d.\n", pid, size, desc);
    kdebug("Данные находятся по адресу %x.\n", in);

    word owner_pid = getBlockOwnerPID((ptrint)in, size);
    if (owner_pid != pid) {
        kdebug("ОШИБКА: Процесс использует блок, не принадлежащий ему\n");
        kdebug(owner_pid != maxword ?
            "Блок принадлежит процессу %d.\n" : "Блок принадлежит нескольким процессам.\n", owner_pid);
        return RUNTIME_ERROR_INVALID_ACCESS;
    }

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Такого файла нет\n");
        return 0;
    }

    return file_ptr->write(write_start, size, in);
}

handle(write_char) {
    dword ch = (dword)arg1;
    dword write_start = (dword)arg2;
    dword desc = (dword)arg3;

    kdebug("Процесс %d записывает символ \"%c\" (%x) в файл № %d.\n", pid, ch, ch, desc);
    kdebugwait();

    File *file_ptr = get_file_pointer(pid, desc);
    if (!file_ptr) {
        kdebug("ОШИБКА: Файла не существует\n");
        return -1;
    }

    byte ch_str[5] = {BYTE0(ch), BYTE1(ch), BYTE2(ch), BYTE3(ch), 0};

    return file_ptr->write(write_start, 1, ch_str);
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

handle(remove_file) {
    /* TODO:
     * Так как архитектура ядра идёт на перепись, сейчас нет смысла реализовывать эти вызовы.
     */
    return -1;
}

handle(move_file) {
    /* TODO:
     * Так как архитектура ядра идёт на перепись, сейчас нет смысла реализовывать эти вызовы.
     */
    return 0;
}