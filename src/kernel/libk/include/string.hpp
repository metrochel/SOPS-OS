/*
 * string.hpp - Класс string
 *
 * Определяет класс string, представляющий собой строку.
 */

#ifndef _STRING_INCL
#define _STRING_INCL

#include "../nums.hpp"

/// @c string - это класс, представляющий строку.
class string {
    /// Символы строки
    const char *symbols;
    /// Длина строки
    size_t len;
    /// Ёмкость массива символов
    size_t capacity;

    /// @brief Выделяет память под строку так, чтобы в неё поместилась строка длиной @c new_len.
    /// @param new_len Длина строки, которую нужно вместить
    void reallocate(size_t new_len);

public:
    /// @c iterator - тип итератора.
    typedef char* iterator;
    /// @c const_iterator - тип итератора, неспособного изменять данные.
    typedef const char* const_iterator;

    /// Создаёт пустую строку.
    string();
    /// Создаёт строку, копируя C-строку @c cstr.
    /// @param cstr Исходная C-строка
    string(const char *cstr);
    /// Создаёт строку, копируя @c n символов из C-строки @c cstr.
    /// @param cstr C-строка
    /// @param n Количество копируемых символов
    string(const char *cstr, size_t n);
    /// Копирует строку @c str.
    /// @param str Исходная строка
    string(string &&str);

    /// Удаляет строку.
    ~string();

    /// Возвращает строку в формате C-строки.
    /// @return C-строка
    const char* data() const { return symbols; }
    /// Возвращает длину строки.
    /// @return Длина строки в байтах
    size_t length() const { return len; }

    /// Возвращает итератор в начале.
    /// @return Неизменный итератор, установленный в начало строки
    const_iterator begin() const { return (const_iterator)(symbols); }
    /// Возвращает итератор в конце.
    /// @return Неизменный итератор, установленный в конец строки
    const_iterator end() const { return (const_iterator)(symbols + len - 1); }

    char operator[](int) const;
    string& operator=(const string&);
    void operator+=(const string&);
    void operator+=(const char);
    bool operator==(const string&);
    bool operator!=(const string&);
    bool operator>(const string&);
    bool operator<(const string&);
    bool operator>=(const string&);
    bool operator<=(const string&);
};

#endif
