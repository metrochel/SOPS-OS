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
protected:
    /// @brief Создаёт новый поток ввода.
    istream() {}
public:
    virtual void put(dword ch) override;
    virtual void flush() override;

    istream& operator>>(byte &number);
    istream& operator>>(word &number);
    istream& operator>>(dword &number);
    istream& operator>>(qword &number);
    istream& operator>>(size_t &number);
    istream& operator>>(char &ch);
    istream& operator>>(string &str);
    istream& operator>>(intmod mod);
    istream& operator>>(void *&ptr);

    template<typename T>
    istream& operator>>(T *&ptr) {
        return operator>>((void*&)ptr);
    }

    istream& operator=(const istream&&);
};

/// @c ostream - это класс потока вывода.
class ostream : virtual public stream {
protected:
    /// @brief Создаёт новый поток вывода.
    ostream() {}
public:
    virtual dword get() override;

    ostream& operator<<(byte number);
    ostream& operator<<(word number);
    ostream& operator<<(dword number);
    ostream& operator<<(qword number);
    ostream& operator<<(size_t number);
    ostream& operator<<(int number);
    ostream& operator<<(intmod mod);
    ostream& operator<<(char ch);
    ostream& operator<<(const char *str);
    ostream& operator<<(const string& str);
    ostream& operator<<(void *ptr);

    template<typename T>
    ostream& operator<<(T *ptr) {
        return operator<<((void*)ptr);
    }

    ostream& operator=(const ostream&&);
};

/// @c iostream - это класс потока, способного как считывать, так и выводить данные.
class iostream : public istream, public ostream {
protected:
    /// @brief Создаёт новый поток ввода/вывода.
    iostream() {}
public:
    iostream& operator=(const iostream&&);
};

#endif //_IOSTREAMS_INCL
