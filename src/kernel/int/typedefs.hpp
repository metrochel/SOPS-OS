/*
 * typedefs.hpp - Объявления типов для прерываний
 *
 * Объявляет вспомогательные типы для библиотеки прерываний.
 */

#ifndef _INTS_TYPEDEFS_INCL
#define _INTS_TYPEDEFS_INCL

#include "libk/nums.hpp"

namespace interrupt {
    typedef void int_frame;

    typedef void (*handle)(int_frame*);

    /// @c idt_entry32 описывает одну метку в таблице дескрипторов прерываний (IDT).
    struct idt_entry32 {
        qword offset2       : 16;
        qword selector      : 16;
        qword __reserved2   : 8;
        qword gate          : 4;
        qword __reserved    : 1;
        qword dpl           : 2;
        qword present       : 1;
        qword offset1       : 16;
    };

    enum idt_gate {
        task_gate = 0b0101,
        int_gate16 = 0b0110,
        int_trap16 = 0b0111,
        int_gate32 = 0b1110,
        int_trap32 = 0b1111
    };

    struct idtr32 {
        word size;
        dword offset;
    } __attribute__((packed));
}

#endif //_INTS_TYPEDEFS_INCL
