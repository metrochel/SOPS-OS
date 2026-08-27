//
//  newdelete.cpp - Операторы new и delete
//
//  Реализует код для операторов new, new[], delete и delete[].
//

#include "libk/nums.hpp"
#include "memmgr/memmgr.hpp"

/// Оператор @c new выделяет динамическую память для объекта размером @c sz Б.
/// @param sz Размер объекта, Б
/// @return Указатель на выделенную память
void* operator new(size_t sz) {
    return kmalloc(sz);
}

/// Оператор @c new[] выделяет динамическую память для массива объектов размером @c sz Б
/// @param sz Размер массива, Б
/// @return Указатель на выделенную память
void* operator new[](size_t sz) {
    return kmalloc(sz);
}

/// Оператор @c delete освобождает динамическую память, занятую объектом @c ptr.
/// @param ptr Указатель на освобождаемый объект
void operator delete(void *ptr) {
    kfree(ptr);
}

/// Оператор @c delete освобождает динамическую память, занятую объектом @c ptr.
/// @param ptr Указатель на освобождаемый объект
void operator delete(void *ptr, size_t) {
    kfree(ptr);
}

/// Оператор @c delete[] освобождает динамическую память, занятую массивом @c ptr.
/// @param ptr Указатель на освобождаемый массив
void operator delete[](void *ptr) {
    kfree(ptr);
}