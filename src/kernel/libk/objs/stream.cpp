/*
 * stream.cpp - Класс stream
 *
 * Реализует методы класса stream.
 */

#include "../include/stream.hpp"
#include "../../memmgr/memmgr.hpp"

#define is_line_stop(ch) ((ch) == 0 || (ch) == '\n')

stream::stream(put_func_t put) : put(put), get(nullptr),
    unget_buf((dword*)kmalloc(sizeof((dword)0) * unget_buf_sz))
    {}

stream::stream(get_func_t get) : put(nullptr), get(get),
    unget_buf((dword*)kmalloc(sizeof((dword)0) * unget_buf_sz))
{}

stream::stream(put_func_t put, get_func_t get) : put(put), get(get),
    unget_buf((dword*)kmalloc(sizeof((dword)0) * unget_buf_sz))
{}

void stream::write_char(dword c) {
    if (!put) {
        // TODO: бросать исключение
        return;
    }

    put(c);
}

dword stream::read_char() {
    if (!get) {
        // TODO: бросать исключение
        return maxdword;
    }

    if (unget_idx) {
        if (unget_idx >= unget_buf_sz) {
            // TODO: бросать исключение
            return maxdword;
        }

        if (!unget_buf) {
            // TODO: бросать исключение
            return maxdword;
        }

        return unget_buf[--unget_idx];
    }

    return get();
}

void stream::unget(dword ch) {
    if (unget_idx >= unget_buf_sz) {
        // TODO: бросать исключение
        return;
    }

    if (!unget_buf) {
        // TODO: бросать исключение
        return;
    }

    unget_buf[unget_idx++] = ch;
}

void stream::write_dec_uint(qword num) {
    dword length = 32;
    dword array[length];

    qword div = 1;
    while (div <= num) div *= 10;
    div /= 10;

    dword index = length - 1;
    while (num) {
        dword digit = num / div;
        array[index--] = '0' + digit;
        num %= div;
        div /= 10;
    }

    while (index < length) {
        write_char(array[index++]);
    }
}

void stream::write_bin_uint(qword num) {
    if (!num) {
        put('0');
        return;
    }

    byte bits = 0;
    while ((1 << bits) <= num)
        bits++;

    put('0');
    put('b');

    while (bits) {
        byte digit = (num >> (bits - 1)) & 1;
        write_char('0' + digit);
        bits--;
    }
}

void stream::write_oct_uint(qword num) {
    if (!num) {
        put('0');
        return;
    }

    byte triplets = 0;
    while ((1 << (3 * triplets)) <= num) {
        triplets++;
    }

    put('0');
    put('o');

    while (triplets) {
        byte digit = (num >> (3 * (triplets - 1))) & 7;
        write_char('0' + digit);
        triplets--;
    }
}

void stream::write_hex_uint(qword num) {
    if (!num) {
        put('0');
        return;
    }

    byte quads = 0;
    while ((1 << (4 * quads)) <= num) {
        quads++;
    }

    put('0');
    put('x');

    while (quads) {
        byte digit = (num >> (4 * (quads - 1))) & 0xF;
        if (digit < 10)
            write_char('0' + digit);
        else
            write_char('A' + digit - 10);
        quads--;
    }
}

qword stream::read_dec_uint() {
    qword num = 0;

    dword ch = read_char();
    while (ch >= '0' && ch <= '9') {
        num = 10 * num + (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

qword stream::read_bin_uint() {
    qword num = 0;

    dword ch = read_char();
    while (ch == '1' || ch == '0') {
        num = (num << 1) | (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

qword stream::read_oct_uint() {
    qword num = 0;

    dword ch = read_char();
    while (ch >= '0' && ch <= '7') {
        num = (num << 3) | (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

qword stream::read_hex_uint() {
    qword num = 0;

    dword ch = read_char();
    while (ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'F' || ch >= 'a' && ch <= 'f') {
        byte digit = ch - '0';
        if (digit >= 10) {
            digit = ch - 'A' + 10;
        }
        if (digit >= 16) {
            digit = ch - 'a' + 10;
        }

        num = (num << 4) | digit;
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

void stream::set_modifier(stream::data_modifier new_mod) {
    modifier = new_mod;
}

void stream::write_uint(qword num) {
    switch (modifier) {
        case normal:
            write_dec_uint(num);
            return;
        case bin:
            write_bin_uint(num);
            return;
        case oct:
            write_oct_uint(num);
            return;
        case hex:
            write_hex_uint(num);
            return;
        default:
            // TODO: бросать исключение
            return;
    }
}

void stream::write_int(long long num) {
    if (num < 0) {
        write_char('-');
        num = -num;
    }

    write_uint(num);
}

void stream::write_str(const char *str) {
    while (*str) {
        write_char(*str++);
    }
}

void stream::write_str(const string& str) {
    for (char c : str) {
        write_char(c);
    }
}

qword stream::read_uint() {
    switch (modifier) {
        case normal: return read_dec_uint();
        case bin: return read_bin_uint();
        case oct: return read_oct_uint();
        case hex: return read_hex_uint();
        default:
            // TODO: бросать исключение
            return -1;
    }
}

long long stream::read_int() {
    bool negative = false;

    dword ch = read_char();
    if (ch == '-') {
        negative = true;
    } else {
        // TODO: unget(ch);
    }

    long long uint = (long long)read_uint();
    return negative ? -uint : uint;
}

string stream::read_str() {
    string str;

    dword ch = read_char();
    while (!is_line_stop(ch)) {
        str += ch;
        ch = read_char();
    }
    unget(ch);

    return str;
}