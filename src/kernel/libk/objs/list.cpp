/*
 * list.cpp - Шаблон класса list
 *
 * Реализует методы шаблона класса list.
 */

#include "../include/list.hpp"
#include "../../memmgr/memmgr.hpp"
#include "../util.hpp"

#define cap_from_len(x) ((((x) >> 3) + 1) << 3)

template<typename T>
list<T>::list() {
    array = nullptr;
    len = 0;
    capacity = 0;
}

template<typename T>
list<T>::list(const T* arr, size_t length) {
    capacity = cap_from_len(length);

    array = kmalloc(capacity * sizeof(*arr));
    if (!array) {
        // TODO: бросать исключение
    }
    memcpy(arr, array, length * sizeof(*arr));

    len = length;
}

template<typename T>
list<T>::list(const list<T> &copy_list) {
    capacity = cap_from_len(copy_list.length());

    size_t cap_bytes = sizeof(*copy_list.array);
    array = kmalloc(cap_bytes);
    if (!array) {
        // TODO: бросать исключение
    }
    memcpy(copy_list.array, array, cap_bytes);

    len = copy_list.len;
}

template<typename T>
list<T>::~list() {
    if (array) kfree((void*)array);
}

template<typename T>
void list<T>::reallocate(size_t new_sz) {
    if (capacity >= new_sz) return;
    size_t new_cap_b = cap_from_len(new_sz) * sizeof (*array);
    void *new_arr = (void*)array;
    krealloc(new_arr, new_cap_b);
    if (!new_arr) {
        // TODO: бросать исключение
        return;
    }

    array = (T*)new_arr;
    capacity = cap_from_len(new_sz);
}

template<typename T>
void list<T>::add(const T &element) {
    reallocate(len + 1);
    array[len++] = element;
}

template<typename T>
void list<T>::extend(const list<T> &add_list) {
    reallocate(len + add_list.len);
    for (dword i = 0; i < add_list.len; i++) {
        dword index = len + i;
        T &element = add_list[index];
        array[index] = element;
    }
}

template<typename T>
void list<T>::insert(int index, const T &element) {
    if (index < 0) {
        index += len;
        if (index < 0) {
            // TODO: бросать исключение
        }
    }
    if (index > len) {
        // TODO: бросать исключение
    }

    reallocate(len + 1);
    for (int i = len; i > index; i++) {
        array[i] = array[i - 1];
    }

    array[index] = element;
}

template<typename T>
void list<T>::remove(int index) {
    if (index < 0) {
        index += len;
    }

    if (index >= len) {
        // TODO: Бросать исключение
        return;
    }

    for (int i = index + 1; i < len; i++) {
        array[i - 1] = array[i];
    }

    len--;
}

template<typename T>
T& list<T>::operator[](int index) {
    if (index < 0)
        index += len;

    if (index < 0 || index >= len) {
        // TODO: бросать исключение
        return (T)null;
    }

    return array[index];
}

template<typename T>
const T& list<T>::operator[](int index) const {
    if (index < 0)
        index += len;

    if (index < 0 || index >= len) {
        // TODO: бросать исключение
        return (const T)null;
    }

    return array[index];
}