/*
 * stdio.cpp - Стандартный ввод-вывод для процессов
 *
 */

#include "stdio.hpp"
#include "process.hpp"
#include "../memmgr/memmgr.hpp"
#include "../graphics/glyphs.hpp"
#include "../keyboard/keyboard.hpp"

#define _unused [[maybe_unused]]

dword stdout_file::read(_unused dword read_start, _unused dword read_read_size, _unused byte *out) {
    kerror("ОШИБКА: Нельзя читать из stdout\n");
    return false;
}

dword stdout_file::write(dword write_start, dword write_size, byte *in) {
    if (process->parent) {
        return process->parent->stdout->write(write_start, write_size, in);
    }

    kprint((const char*)in);
    return true;
}

dword stdin_file::write(_unused dword write_start, _unused dword write_size, _unused byte *in) {
    kerror("ОШИБКА: Нельзя записать в stdin\n");
    return false;
}

dword stdin_file::read(dword read_start, dword read_size, byte *out) {
    if (process->parent) {
        return process->parent->stdin->read(read_start, read_size, out);
    }

    if (size < maxInputSize) {
        kerror("ОШИБКА: Не хватает места для чтения\n");
        return false;
    }

    kread(out);
    return true;
}

dword stderr_file::read(_unused dword read_start, _unused dword read_size, _unused byte *out) {
    kerror("ОШИБКА: Нельзя считать из stderr\n");
    return false;
}

dword stderr_file::write(dword write_start, dword write_size, byte *in) {
    if (process->parent) {
        return process->parent->stderr->write(write_start, write_size, in);
    }

    kerror("ОШИБКА: %s", in);
    return true;
}