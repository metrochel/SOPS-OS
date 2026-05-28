/*
 * stream.hpp - Класс stream
 *
 * Объявляет класс stream, задающий некоторый поток данных.
 */

#ifndef _STREAM_INCL
#define _STREAM_INCL

#include "../nums.hpp"
#include "string.hpp"

/// @c stream - это поток данных. Позволяет запись и чтение.
class stream {
public:
    /// Постоянная, равная размеру буфера, используемого функцией @c unget.
    const dword unget_buf_sz = 256;

    /// @c put_func_t - это тип функции, записывающей один символ для данного потока.
    typedef void (*put_func_t)(dword);
    /// @c get_func_t - это тип функции, считывающей один символ для данного потока.
    typedef dword (*get_func_t)();

    /// @c data_modifier - это перечень допустимых модификаторов для чтения/записи данных.
    typedef enum {
        normal = 0,
        bin,
        oct,
        hex,
    } data_modifier;

    /// @brief Создаёт поток с функцией записи @c put.
    /// @param put Функция записи
    stream(put_func_t put);
    /// @brief Создаёт поток с функцией чтения @c get.
    /// @param get Функция чтения
    stream(get_func_t get);
    /// @brief Создаёт поток с функциями записи put и чтения @c get.
    /// @param put Функция записи
    /// @param get Функция чтения
    stream(put_func_t put, get_func_t get);

private:
    /// Буфер символов, возвращённых функцией @c unget
    dword *const unget_buf;
    /// Индекс в буфере возвращённых символов
    dword unget_idx = 0;

    /// @brief Возвращает символ @c ch в поток.
    /// @param ch Возвращаемый символ
    void unget(dword ch);

    /// @brief Записывает число в поток в десятичной системе счисления.
    /// @param num Число
    void write_dec_uint(qword num);
    /// @brief Записывает число в поток в двоичной системе счисления.
    /// @param num Число
    void write_bin_uint(qword num);
    /// @brief Записывает число в поток в восьмеричной системе счисления.
    /// @param num Число
    void write_oct_uint(qword num);
    /// @brief Записывает число в поток в шестнадцатеричной системе счисления.
    /// @param num Число
    void write_hex_uint(qword num);

    /// @brief Считывает число из потока в десятичной системе счисления.
    /// @return Считанное число
    qword read_dec_uint();
    /// @brief Считывает число из потока в двоичной системе счисления.
    /// @return Считанное число
    qword read_bin_uint();
    /// @brief Считывает число из потока в восьмеричной системе счисления.
    /// @return Считанное число
    qword read_oct_uint();
    /// @brief Считывает число из потока в шестнадцатеричной системе счисления.
    /// @return Считанное число
    qword read_hex_uint();

protected:
    /// Текущий модификатор вывода
    data_modifier modifier = normal;

    /// Функция записи в данный поток
    put_func_t put;
    /// Функция чтения из данного потока
    get_func_t get;

    /// @brief Изменяет модификатор данных.
    void set_modifier(data_modifier new_mod);

    /// @brief Записывает в поток беззнаковое число.
    /// @param num Число
    void write_uint(qword num);

    /// @brief Записывает в поток знаковое число.
    /// @param num Число
    void write_int(long long num);

    /// @brief Записывает в поток символ.
    /// @param c Символ
    void write_char(dword c);

    /// @brief Записывает в поток строку.
    /// @param str Строка
    void write_str(const char *str);
    /// @brief Записывает в поток строку.
    /// @param str Строка
    void write_str(const string& str);

    /// @brief Считывает из потока число.
    /// @return Считанное число
    qword read_uint();
    /// @brief Считывает из потока знаковое число.
    /// @return Считанное число
    long long read_int();
    /// @brief Считывает из потока символ.
    /// @return Считанный символ
    dword read_char();
    /// @brief Считывает из потока строку.
    /// @return Считанная строка
    string read_str();
};

#endif //_STREAM_INCL
