/*
 * stream.hpp - Класс stream
 *
 * Объявляет класс stream, задающий некоторый поток данных.
 */

#ifndef _STREAM_INCL
#define _STREAM_INCL

#include "../nums.hpp"
#include "string.hpp"

enum intmod {
    dec = 0,
    bin,
    oct,
    hex
};

/// @c stream - это поток данных. Позволяет запись и чтение.
class stream {
public:
    /// Постоянная, равная размеру буфера, используемого функцией @c unget.
    const dword unget_buf_sz = 256;

    /// @c data_modifier - это перечень допустимых модификаторов для чтения/записи данных.
    typedef struct {
        intmod int_mod;
    } data_modifier;

private:
    /// Буфер символов, возвращённых функцией @c unget
    dword *unget_buf;
    /// Индекс в буфере возвращённых символов
    dword unget_idx = 0;

    /// Буфер вывода для функции @c write_char
    byte write_buffer[4] = {0, 0, 0, 0};
    /// Количество оставшихся символов для @c write_char
    byte write_bytes_remaining = 0;

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
    /// @brief Создаёт пустой поток.
    stream();

    /// Текущий модификатор вывода
    data_modifier modifier = { dec };

    /// @brief Записывает символ в поток.
    /// @param ch Символ
    virtual void put(dword ch) = 0;

    /// @brief Считывает символ из потока.
    /// @return Считанный символ
    virtual dword get() = 0;

    /// @brief Изменяет модификатор данных.
    void set_modifier(data_modifier new_mod);

    /// @brief Изменяет модификатор численных данных.
    void set_int_modifier(intmod new_mod);

    /// @brief Записывает в поток беззнаковое число.
    /// @param num Число
    void write_uint(qword num);

    /// @brief Записывает в поток знаковое число.
    /// @param num Число
    void write_int(long long num);

    /// @brief Записывает в поток символ.
    /// @param c Символ
    void write_char(byte c);

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
