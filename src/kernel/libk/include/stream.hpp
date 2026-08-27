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

enum streammod {
    text = 0,
    binary
};

/// @c stream - это поток данных. Позволяет запись и чтение.
class stream {
public:
    /// Постоянная, равная размеру буфера, используемого функцией @c unget.
    const dword unget_buf_sz = 256;

    /// @c data_modifier - это перечень допустимых модификаторов для чтения/записи данных.
    typedef struct {
        intmod int_mod;
        streammod str_mod;
        bool send_utf8;
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

    template<typename T>
    void write_text_bin_uint(T num);

    template<typename T>
    void write_text_oct_uint(T num);

    template<typename T>
    void write_text_dec_uint(T num);

    template<typename T>
    void write_text_hex_uint(T num);

    template<typename T>
    void write_text_uint(T num);

    template<typename T>
    void write_binary_uint(T num);

    /// @brief Записывает в поток знаковое число.
    /// @param num Число
    template<typename T>
    void write_text_int(T num);

    /// @brief Записывает в поток знаковое число.
    /// @param num Число
    template<typename T>
    void write_binary_int(T num);

    /// @brief Считывает число из потока в десятичной системе счисления.
    /// @return Считанное число
    template<typename T>
    T read_text_dec_uint();
    /// @brief Считывает число из потока в двоичной системе счисления.
    /// @return Считанное число
    template<typename T>
    T read_text_bin_uint();
    /// @brief Считывает число из потока в восьмеричной системе счисления.
    /// @return Считанное число
    template<typename T>
    T read_text_oct_uint();
    /// @brief Считывает число из потока в шестнадцатеричной системе счисления.
    /// @return Считанное число
    template<typename T>
    T read_text_hex_uint();

    template<typename T>
    T read_text_uint();

    template<typename T>
    T read_binary_uint();

    template<typename T>
    T read_text_int();

    template<typename T>
    T read_binary_int();

protected:
    /// @brief Создаёт пустой поток.
    stream();

    /// Текущий модификатор вывода
    data_modifier modifier = { dec, text, false };

    /// @brief Записывает символ в поток.
    /// @param ch Символ
    virtual void put(dword ch) = 0;

    /// @brief Считывает символ из потока.
    /// @return Считанный символ
    virtual dword get() = 0;

    /// @brief Записывает все символы буферизированного потока на устройство.
    virtual void flush() = 0;

    /// @brief Изменяет модификатор данных.
    void set_modifier(data_modifier new_mod);

    /// @brief Изменяет модификатор численных данных.
    void set_int_modifier(intmod new_mod) { modifier.int_mod = new_mod; }

    void set_stream_modifier(streammod new_mod) { modifier.str_mod = new_mod; }

    void set_send_utf8(bool new_send) { modifier.send_utf8 = new_send; }

    template<typename T>
    void write_uint(T num);

    template<typename T>
    void write_int(T num);

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
    template<typename T>
    T read_uint();

    /// @brief Считывает из потока знаковое число.
    /// @return Считанное число
    template<typename T>
    T read_int();

    /// @brief Считывает из потока символ.
    /// @return Считанный символ
    dword read_char();
    /// @brief Считывает из потока строку.
    /// @return Считанная строка
    string read_str();
};

#endif //_STREAM_INCL
