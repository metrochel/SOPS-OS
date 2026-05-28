/*
 * string.cpp - Класс string
 *
 * Реализует методы класса string.
 */

#include "../include/string.hpp"
#include "../../memmgr/memmgr.hpp"
#include "../util.hpp"

extern size_t strlen(const char *str);

string::string() {
    symbols = nullptr;
    len = 0;
    capacity = 0;
}

string::string(const char *cstr, size_t n) {
    capacity = ((n >> 3) + 1) << 3;
    symbols = (const char*)kmalloc(capacity);
    memcpy(cstr, symbols, n);
    if (!symbols) {
        // TODO: бросать исключение
    }
    len = n;
}

string::string(const char *cstr) : string(cstr, strlen(cstr)) {}

string::string(string &&str) : string(str.data(), str.length()) {}

string::~string() {
    if (symbols) kfree((void*)symbols);
}

void string::reallocate(size_t new_len) {
    if (capacity >= new_len) {
        return;
    }

    size_t new_cap = ((capacity >> 3) + 1) << 3;

    void *new_symbols = (void*)symbols;
    krealloc(new_symbols, new_cap);
    if (!new_symbols) {
        // TODO: бросать исключение
        return;
    }

    symbols = (const char*)new_symbols;
    capacity = new_cap;
    len = 0;
}

char string::operator[](int idx) const {
    if (idx >= len || idx < 0) {
        // TODO: бросать исключение
        return -1;
    }
    return symbols[idx];
}

string& string::operator=(const string& str) {
    reallocate(str.len);
    memcpy(symbols, str.data(), str.length());
    return *this;
}

void string::operator+=(const string& str) {
    size_t new_len = len + str.len;
    reallocate(new_len);
    memcpy(symbols + len, str.symbols, str.len);
    len = new_len;
}

void string::operator+=(const char c) {
    reallocate(len + 1);
    ((char*)(symbols))[len] = c;
    len++;
}

bool string::operator==(const string& str) {
    if (len != str.len)
        return false;

    for (int i = 0; i < len; i++) {
        if (symbols[i] != str[i])
            return false;
    }

    return true;
}

bool string::operator!=(const string& str) {
    return !operator==(str);
}

bool string::operator>(const string& str) {
    if (len > str.len)
        return true;
    if (len < str.len)
        return false;

    for (int i = 0; i < len; i++) {
        if (symbols[i] > str[i])
            return true;
        if (symbols[i] < str[i])
            return false;
    }

    return false;
}

bool string::operator<(const string& str) {
    if (len < str.len)
        return true;
    if (len > str.len)
        return false;

    for (int i = 0; i < len; i++) {
        if (symbols[i] < str[i])
            return true;
        if (symbols[i] > str[i])
            return false;
    }

    return false;
}

bool string::operator>=(const string& str) {
    return operator>(str) || operator==(str);
}

bool string::operator<=(const string& str) {
    return operator<(str) || operator==(str);
}