/*
 * stdio.cpp - Стандартный ввод-вывод для процессов
 *
 */

#include "stdio.hpp"
#include "process.hpp"
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

    static dword buffer = 0;
    if (write_size == 1 && *in >= 0x80) {
        if (buffer) {
            buffer = (*in << 8) | buffer;
            kprint((const char*)&buffer);
            buffer = 0;
        } else {
            buffer = *in;
        }
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

    static byte buffer[maxInputSize];
    static dword idx = maxInputSize;

    dword read = 0;
    for (dword i = 0; i < read_size; i++) {
        read++;
        if (idx >= maxInputSize) {
            kread(buffer);
            idx = 0;
        }

        if (buffer[idx] == 0) {
            idx = maxInputSize;
            break;
        }

        out[i] = buffer[idx++];
    }

    return read;
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