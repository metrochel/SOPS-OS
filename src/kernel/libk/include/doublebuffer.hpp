/*
 * doublebuffer.hpp - Класс double_buffer
 *
 * Объявляет класс double_buffer, служащий для реализации двойного буфера.
 */

#ifndef _DOUBLEBUFFER_INCL
#define _DOUBLEBUFFER_INCL

#include "../nums.hpp"

class double_buffer {
    dword size;

    byte *ptr;

    byte *read_ptr;
    byte *write_ptr;

public:
    double_buffer(dword sz);

    double_buffer(const double_buffer&);

    double_buffer(const double_buffer&&);

    double_buffer& operator=(double_buffer&);

    double_buffer& operator=(double_buffer&&);

    ~double_buffer();

    void read(byte *data, dword size);

    void write(const byte *data, dword size);
};

#endif //_DOUBLEBUFFER_INCL
