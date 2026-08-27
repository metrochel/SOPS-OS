/*
 * doublebuffer.cpp - Реализация класса double_buffer
 */

#include "../include/doublebuffer.hpp"
#include "memmgr/memmgr.hpp"
#include "../util.hpp"

double_buffer::double_buffer(dword sz, void (*wait)()) {
    if (sz == 0) {
        read_ptr = write_ptr = ptr = nullptr;
        wait_func = wait;
        return;
    }

    ptr = (byte*)kmalloc(sz);

    if (!ptr) {
        return;
    }

    read_ptr = write_ptr = ptr;
    wait_func = wait;
}

double_buffer::double_buffer(dword sz) : double_buffer(sz, nullptr) {}

double_buffer::double_buffer(const double_buffer &ref) {
    sz = ref.sz;

    ptr = (byte*)kmalloc(ref.sz);
    if (!ptr) {
        return;
    }

    memcpy(ref.ptr, ptr, sz);

    read_ptr = write_ptr = ptr;
    wait_func = ref.wait_func;
}

double_buffer::double_buffer(const double_buffer &&ref) {
    sz = ref.sz;

    ptr = ref.ptr;

    read_ptr = write_ptr = ptr;
    wait_func = ref.wait_func;
}

double_buffer::~double_buffer() {
    if (ptr) kfree(ptr);
}

double_buffer& double_buffer::operator=(double_buffer &ref) {
    sz = ref.sz;

    ptr = kmalloc(ref.sz);
    if (!ptr) {
        return *this;
    }
    memcpy(ref.ptr, ptr, sz);

    read_ptr = write_ptr = ptr;
    wait_func = ref.wait_func;
    return *this;
}

double_buffer& double_buffer::operator=(double_buffer &&ref) {
    sz = ref.sz;

    ptr = ref.ptr;

    read_ptr = write_ptr = ptr;
    wait_func = ref.wait_func;
    return *this;
}

dword double_buffer::diff() {
    if (read_ptr > write_ptr) {
        return write_ptr + sz - read_ptr;
    }

    return write_ptr - read_ptr;
}

void double_buffer::read(byte *data, dword n) {
    if (n >= sz) {
        // TODO: бросать исключение
        return;
    }

    if (n >= sz - diff()) {
        if (!wait_func) {
            // TODO: бросать исключение
            return;
        }

        while (n >= sz - diff()) {
            wait_func();
        }
    }

    if (read_ptr + n < ptr + sz) {
        memcpy(read_ptr, data, n);
        read_ptr += n;
    } else {
        dword read_batch1 = ptr + sz - read_ptr;
        memcpy(read_ptr, data, read_batch1);
        read_ptr = ptr;
        data += read_batch1;

        dword read_batch2 = n - read_batch1;
        memcpy(read_ptr, data, read_batch2);
        read_ptr += read_batch2;
    }
}

void double_buffer::read(byte &bt) {
    read(&bt, 1);
}

void double_buffer::write(const byte *data, dword n) {
    if (n >= sz) {
        // TODO: бросать исключение
        return;
    }

    if (n >= sz - diff()) {
        if (!wait_func) {
            // TODO: бросать исключение
            return;
        }

        while (n >= sz - diff()) {
            wait_func();
        }
    }

    if (write_ptr + n < ptr + sz) {
        memcpy(data, write_ptr, n);
        write_ptr += n;
    } else {
        dword write_batch1 = ptr + sz - write_ptr;
        memcpy(data, write_ptr, write_batch1);
        write_ptr = ptr;
        data += write_batch1;

        dword write_batch2 = n - write_batch1;
        memcpy(data, write_ptr, write_batch2);
        write_ptr += write_batch2;
    }
}

void double_buffer::write(const byte bt) {
    write(&bt, 1);
}

void double_buffer::reallocate(size_t new_sz) {
    if (ptr) {
        void *cur_ptr = ptr;
        dword read_diff = read_ptr - ptr;
        dword write_diff = write_ptr - ptr;

        krealloc(cur_ptr, new_sz);

        byte *new_ptr = (byte*)cur_ptr;

        write_ptr = new_ptr + write_diff;
        read_ptr = new_ptr + read_diff;
        ptr = new_ptr;
    } else {
        byte *new_ptr = new byte[new_sz];

        write_ptr = read_ptr = new_ptr;
        ptr = new_ptr;
    }

    sz = new_sz;
}