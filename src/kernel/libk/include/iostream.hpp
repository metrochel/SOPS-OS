/*
 * iostreams.hpp - Классы потоков ввода/вывода
 *
 * Объявляет классы, более удобные для взаимодействия с потоками.
 */

#ifndef _IOSTREAMS_INCL
#define _IOSTREAMS_INCL

#include "stream.hpp"

/// @c istream - это класс потока ввода.
class istream : virtual public stream {
public:
    /// @brief Создаёт новый поток ввода с функцией чтения @c get.
    /// @param get Функция чтения
    istream(stream::get_func_t get) : stream(get) {}
    istream() : stream(nullptr, nullptr) {}

    istream& operator>>(byte &number);
    istream& operator>>(word &number);
    istream& operator>>(dword &number);
    istream& operator>>(qword &number);
    istream& operator>>(char &ch);
    istream& operator>>(string &str);
    istream& operator>>(intmod mod);

    istream& operator=(const istream&&);
};

/// @c ostream - это класс потока вывода.
class ostream : virtual public stream {
public:
    /// @brief Создаёт новый поток вывода с функцией чтения @c put.
    /// @param put Функция записи
    ostream(stream::put_func_t put) : stream(put) {}
    ostream() : stream(nullptr, nullptr) {}

    ostream& operator<<(byte number);
    ostream& operator<<(word number);
    ostream& operator<<(dword number);
    ostream& operator<<(qword number);
    ostream& operator<<(int number);
    ostream& operator<<(intmod mod);
    ostream& operator<<(char ch);
    ostream& operator<<(const char *str);
    ostream& operator<<(const string& str);

    ostream& operator=(const ostream&&);
};

/// @c iostream - это класс потока, способного как считывать, так и выводить данные.
class iostream : public istream, public ostream {
    /// @brief Создаёт новый поток ввода/вывода с функцией чтения @c get и функцией записи @c put.
    /// @param put Функция записи
    /// @param get Функция чтения
    iostream(stream::get_func_t get, stream::put_func_t put)
        : stream(put, get), istream(get), ostream(put) {}

    iostream() : stream(nullptr, nullptr), istream(), ostream() {}

    iostream& operator=(const iostream&&);
};

#endif //_IOSTREAMS_INCL
