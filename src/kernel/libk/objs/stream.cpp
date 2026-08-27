/*
 * stream.cpp - Класс stream
 *
 * Реализует методы класса stream.
 */

#include "../include/stream.hpp"
#include "../../memmgr/memmgr.hpp"

#define is_line_stop(ch) ((ch) == 0 || (ch) == '\n')

/* === Конструктор === */

stream::stream() : modifier{} {
    unget_buf = (dword*)(kmalloc(unget_buf_sz));
}

/* === Запись и чтение символа === */

void stream::write_char(byte c) {
    // Процедура write_char будет выводить символ в соответствии с UTF-8, если modifier.send_utf8 == true,
    // то есть в функцию put должен попадать полный символ, а не его кусочки.

    if (!modifier.send_utf8) {
        put(c);
        return;
    }

    // Если символ не соответствует UTF-8, хотя должен...
    if ((c >> 6 != 0b10) && write_bytes_remaining) {
        // TODO: бросать исключение
        return;
    }

    // Если символ является частью цепочки байтов символа в UTF-8...
    if (write_bytes_remaining) {
        write_buffer[write_bytes_remaining - 1] = c;
        write_bytes_remaining--;
        if (write_bytes_remaining == 0) {
            dword ch = write_buffer[0] | (write_buffer[1] << 8) | (write_buffer[2] << 16) | (write_buffer[3] << 24);
            put(ch);
            write_buffer[0] = write_buffer[1] = write_buffer[2] = write_buffer[3] = 0;
        }
        return;
    }

    // Если символ начинает цепочку байтов символа UTF-8...
    if (c & 0x80) {
        // Если должен последовать 1 байт...
        if ((c >> 5) == 0b110) {
            write_bytes_remaining = 1;
            write_buffer[1] = c;
            return;
        }

        // Если должно последовать 2 байта...
        if ((c >> 4) == 0b1110) {
            write_bytes_remaining = 2;
            write_buffer[2] = c;
            return;
        }

        // Если должно последовать 3 байта...
        if ((c >> 3) == 0b11110) {
            write_bytes_remaining = 3;
            write_buffer[3] = c;
            return;
        }

        // Иначе получилась нечисть какая-то, валим отсюда
        // TODO: бросать исключение
        return;
    }

    // Если символ является простым символом ASCII, то вывести его.
    put(c);
}

dword stream::read_char() {
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

/* === Запись числа === */

template<typename T>
void stream::write_text_dec_uint(T num) {
    if (num == 0) {
        write_char('0');
        return;
    }

    dword length = 32;
    dword buffer[length];

    int index = 0;
    while (num) {
        dword digit = num % 10;
        buffer[index++] = '0' + digit;
        num /= 10;
    }

    for (int i = index - 1; i >= 0; i--) {
        write_char(buffer[i]);
    }
}

template<typename T>
void stream::write_text_bin_uint(T num) {
    if (!num) {
        write_char('0');
        return;
    }

    byte bits = 0;
    if (msb(num)) {
        bits = 8 * sizeof(num);
    } else {
        while (((T) 1 << bits) <= num)
            bits++;
    }

    write_char('0');
    write_char('b');

    while (bits) {
        byte digit = (num >> (bits - 1)) & 1;
        write_char('0' + digit);
        bits--;
    }
}

template<typename T>
void stream::write_text_oct_uint(T num) {
    if (!num) {
        write_char('0');
        return;
    }

    byte triplets = 0;
    if (msb(num))
        triplets = (8 * sizeof(num) + 2) / 3;
    else {
        while (((T) 1 << (3 * triplets)) <= num) {
            triplets++;
        }
    }

    write_char('0');
    write_char('o');

    while (triplets) {
        byte digit = (num >> (3 * (triplets - 1))) & 7;
        write_char('0' + digit);
        triplets--;
    }
}

template<typename T>
void stream::write_text_hex_uint(T num) {
    if (!num) {
        write_char('0');
        return;
    }

    byte quads = 0;
    if (msb(num))
        quads = 2 * sizeof(num);
    else {
        while (((T) 1 << (4 * quads)) <= num) {
            quads++;
        }
    }

    write_char('0');
    write_char('x');

    while (quads) {
        byte digit = (num >> (4 * (quads - 1))) & 0xF;
        if (digit < 10)
            write_char('0' + digit);
        else
            write_char('A' + digit - 10);
        quads--;
    }
}

template<typename T>
void stream::write_text_uint(T num) {
    switch (modifier.int_mod) {
        case dec:
            write_text_dec_uint(num);
            return;
        case bin:
            write_text_bin_uint(num);
            return;
        case oct:
            write_text_oct_uint(num);
            return;
        case hex:
            write_text_hex_uint(num);
            return;
        default:
            // TODO: бросать исключение
            return;
    }
}

template<typename T>
void stream::write_binary_uint(T num) {
    while (num) {
        write_char(LSB(num));
        num >>= 8;
    }
}

template<typename T>
void stream::write_text_int(T num) {
    if (num < 0) {
        write_char('-');
        num = -num;
    }

    write_text_uint((qword)num);
}

template<typename T>
void stream::write_binary_int(T num) {
    while (num) {
        write_char(LSB(num));
        num >>= 8;
    }
}

template<typename T>
void stream::write_uint(T num) {
    if (modifier.str_mod == streammod::text) {
        write_text_uint(num);
        flush();
        return;
    }
    if (modifier.str_mod == streammod::binary) {
        write_binary_uint(num);
        flush();
        return;
    }

    // TODO: бросать исключение
}

template<typename T>
void stream::write_int(T num) {
    if (modifier.str_mod == streammod::text) {
        write_text_int(num);
        flush();
    }
    if (modifier.str_mod == streammod::binary) {
        write_binary_int(num);
        flush();
    }

    // TODO: бросать исключение
}

template void stream::write_uint<>(byte num);
template void stream::write_uint<>(word num);
template void stream::write_uint<>(dword num);
template void stream::write_uint<>(qword num);
template void stream::write_uint<>(size_t num);

template void stream::write_int<>(int num);

/* === Чтение числа === */

template<typename T>
T stream::read_text_dec_uint() {
    T num = 0;

    dword ch = read_char();
    while (ch >= '0' && ch <= '9') {
        num = 10 * num + (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

template<typename T>
T stream::read_text_bin_uint() {
    T num = 0;

    dword ch = read_char();
    while (ch == '1' || ch == '0') {
        num = (num << 1) | (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

template<typename T>
T stream::read_text_oct_uint() {
    T num = 0;

    dword ch = read_char();
    while (ch >= '0' && ch <= '7') {
        num = (num << 3) | (ch - '0');
        ch = read_char();
    }
    // TODO: unget(ch);

    return num;
}

template<typename T>
T stream::read_text_hex_uint() {
    T num = 0;

    dword ch = read_char();
    while ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
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

template<typename T>
T stream::read_text_uint() {
    switch (modifier.int_mod) {
        case dec: return read_text_dec_uint<T>();
        case bin: return read_text_bin_uint<T>();
        case oct: return read_text_oct_uint<T>();
        case hex: return read_text_hex_uint<T>();
        default:
            // TODO: бросать исключение
            return -1;
    }
}

template<typename T>
T stream::read_binary_uint() {
    T value = 0;
    dword size = sizeof ((T)0);

    for (dword i = 0; i < size; i++) {
        dword ch = read_char();
        value = (value << 8) | ch;
    }

    return value;
}

template<typename T>
T stream::read_uint() {
    if (modifier.str_mod == streammod::text) {
        return read_text_uint<T>();
    }
    if (modifier.str_mod == streammod::binary) {
        return read_binary_uint<T>();
    }

    // TODO: бросать исключение
    return (T)maxqword;
}

template byte stream::read_uint();
template word stream::read_uint();
template dword stream::read_uint();
template qword stream::read_uint();
template size_t stream::read_uint();

template<typename T>
T stream::read_text_int() {
    bool negative = false;

    dword ch = read_char();
    if (ch == '-') {
        negative = true;
    } else {
        // TODO: unget(ch);
    }

    T uint = (T)read_text_uint<T>();
    return negative ? -uint : uint;
}

template<typename T>
T stream::read_binary_int() {
    T value = 0;
    dword size = sizeof ((T)0);

    for (dword i = 0; i < size; i++) {
        dword ch = read_char();
        value = (value << 8) | ch;
    }

    return value;
}

template<typename T>
T stream::read_int() {
    if (modifier.str_mod == streammod::text) {
        return read_text_int<T>();
    }
    if (modifier.str_mod == streammod::binary) {
        return read_binary_int<T>();
    }

    // TODO: бросать исключение
    return (T)maxqword;
}

template char stream::read_int<>();
template short stream::read_int<>();
template int stream::read_int<>();
template long stream::read_int<>();
template long long stream::read_int<>();

/* === Запись и чтение строки === */

void stream::write_str(const char *str) {
    while (*str) {
        write_char(*str++);
    }
    flush();
}

void stream::write_str(const string& str) {
    for (char c : str) {
        write_char(c);
    }
    flush();
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

/* === Модификаторы === */

void stream::set_modifier(stream::data_modifier new_mod) {
    modifier = new_mod;
}


