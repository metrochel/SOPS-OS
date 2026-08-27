/*
 * doublebuffer.hpp - Класс double_buffer
 *
 * Объявляет класс double_buffer, служащий для реализации двойного буфера.
 */

#ifndef _DOUBLEBUFFER_INCL
#define _DOUBLEBUFFER_INCL

#include "../nums.hpp"

/// @c double_buffer - класс, реализующий @a двойной @a буфер.
/// @Принцип
/// Двойной буфер - буфер, реализованный посредством двух указателей - головки записи и головки чтения. @n @n
/// Когда производится запись в буфер (т.е. посредством @c write(ptr,n) ), содержимое копируется в буфер, и
/// записывающая головка смещается на @c n байт. @n @n
/// Когда производится чтение из буфера (т.е. посредством @c read(ptr,n) ), содержимое копируется из буфера с позиции
/// читающей головки, и читающая головка перемещается на @c n байт.
class double_buffer {
    /// Размер буфера
    dword sz;

    /// Указатель на начало буфера
    byte *ptr;

    /// Читающая головка
    byte *read_ptr;
    /// Записывающая головка
    byte *write_ptr;

    /// Функция ожидания опустошения буфера
    void (*wait_func)();

public:
    /// Создаёт буфер размером @c sz байт.
    /// @param sz Размер буфера, Б
    /// @note Буфер создаётся @a неожидающим, то есть при переполнении буфера будет выброшено исключение.
    double_buffer(dword sz);

    /// Создаёт буфер размером @c sz байт с функцией ожидания @c wait.
    /// @param sz Размер буфера, Б
    /// @param wait Функция ожидания опустошения буфера
    /// @note Буфер создаётся @a ожидающим, то есть если буфер переполнится, то до записи будет ожидаться его
    /// опустошение посредством последовательных вызовов @c wait.
    double_buffer(dword sz, void (*wait)());

    /// Создаёт пустой буфер.
    double_buffer() : double_buffer(0, nullptr) {}

    /// Копирует буфер @c ref.
    /// @param ref Копия буфера
    double_buffer(const double_buffer &ref);

    /// Перемещает буфер @c ref.
    /// @param ref Исходный буфер
    double_buffer(const double_buffer &&ref);

    /// Копирует буфер @c ref.
    /// @param ref Копия буфера
    double_buffer& operator=(double_buffer &ref);

    /// Перемещает буфер @c ref.
    /// @param ref Исходный буфер
    double_buffer& operator=(double_buffer &&ref);

    /// Разрушает буфер.
    ~double_buffer();

    /// Производит чтение из буфера.
    /// @param data Указатель на приёмник данных
    /// @param size Размер считываемых данных, Б
    void read(byte *data, dword size);

    /// Производит чтение из буфера.
    /// @param bt Однобайтовый приёмник данных
    void read(byte &bt);

    /// Производит запись в буфер.
    /// @param data Указатель на источник данных
    /// @param size Размер записываемых данных, Б
    void write(const byte *data, dword size);

    /// Производит запись в буфер.
    /// @param bt Однобайтовый источник данных
    void write(const byte bt);

    /// Вычисляет, насколько записывающая головка впереди считывающей.
    dword diff();

    /// Возвращает размер буфера в байтах.
    dword size() const { return sz; }

    /// Выделяет память для буфера так, чтобы уместился буфер размером @c new_sz.
    /// @param new_sz Новый размер буфера
    void reallocate(size_t new_sz);
};

#endif //_DOUBLEBUFFER_INCL
