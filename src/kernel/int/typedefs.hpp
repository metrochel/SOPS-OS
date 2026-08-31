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
    } __attribute__((packed));;

    enum idt_gate32 {
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

    struct idt_entry64 {
        qword offset1       : 32;
        qword __reserved    : 32;

        qword offset2       : 16;
        qword selector      : 16;
        qword __reserved2   : 8;
        qword gate          : 4;
        qword __reserved3   : 1;
        qword dpl           : 2;
        qword present       : 1;
        qword offset3       : 16;
    } __attribute__((packed));

    struct idtr64 {
        word size;
        qword offset;
    } __attribute__((packed));

    enum idt_gate64 {
        int_gate64 = 0xE,
        int_trap64 = 0xF
    };

#ifdef __x86_64__
    typedef idtr64 idtr;
    typedef idt_entry64 idt_entry;
    typedef idt_gate64 idt_gate;
#else
    typedef idtr32 idtr;
    typedef idt_entry32 idt_entry;
    typedef idt_gate32 idt_gate;
#endif
}

#endif //_INTS_TYPEDEFS_INCL
