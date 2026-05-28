/*
 * list.hpp - Список
 *
 * Объявляет шаблон класса list<T> - список элементов типа T.
 */

#ifndef _LIST_INCL
#define _LIST_INCL

#include "../nums.hpp"

/// @c list - это шаблон обыкновенного списка.
template<typename T>
class list {
private:
    /// Массив элементов
    T* array;
    /// Длина списка
    size_t len;
    /// Ёмкость списка
    size_t capacity;

    /// Выделяет память под список так, чтобы поместился список размером @c new_sz элементов.
    /// @param new_sz Необходимый размер
    void reallocate(size_t new_sz);

public:
    /// @c iterator - тип итератора.
    typedef T* iterator;
    /// @c const_iterator - тип итератора, неспособного изменять данные.
    typedef const T* const_iterator;

    /// Инициализирует пустой список.
    list();
    /// Инициализирует список из массива @c arr длиной @c length.
    /// @param arr Исходный массив
    /// @param length Длина исходного массива
    list(const T* arr, size_t length);
    /// Создаёт копию списка @c copy_list.
    list(const list<T>& copy_list);

    /// Удаляет список.
    /// @attention Удаление списка приведёт к удалению @b всех его элементов!
    ~list();

    /// Возвращает элементы списка в виде массива.
    /// @return Массив элементов
    const T* data() const { return array; }
    /// Возвращает длину списка в элементах.
    /// @return Длина списка
    size_t length() const { return len; }

    /// Добавляет элемент @c element в конец списка.
    /// @param element Добавляемый элемент
    void add(const T &element);
    /// Расширяет список элементами списка @c add_list.
    /// @param add_list Добавляемый список
    void extend(const list<T> &add_list);
    /// Вставляет элемент @c element по индексу @c index.
    /// @param element Добавляемый элемент
    /// @param index Индекс вставки
    void insert(int index, const T &element);
    /// Удаляет элемент по индексу @c index.
    /// @param index Индекс удаляемого элемента
    void remove(int index);

    /// Возвращает итератор в начале.
    /// @return Неизменный итератор, установленный в начало строки
    const_iterator begin() const { return array; }
    /// Возвращает итератор в конце.
    /// @return Неизменный итератор, установленный в конец строки
    const_iterator end() const { return array + len - 1; }

    T& operator[](int index);
    const T& operator[](int index) const;
};

#endif //_LIST_INCL
