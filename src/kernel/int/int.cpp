/*
 * int.cpp - Библиотека прерываний
 *
 */

#include "int.hpp"
#include "libk/objs.hpp"

#include "handles/cpu_exceptions.hpp"
#include "handles/irq.hpp"

#include "pic/pic.hpp"

NAMESPACE_BEGIN(interrupt)

idt_entry32 idt[256];

idtr32 idtr;

void register_handle(handle routine, byte vector, byte cpl) {
    idt_entry32 entry;
    entry.offset1 = (ptrint)(routine) >> 16;
    entry.present = 1;
    entry.dpl = cpl;
    entry.gate = int_gate32;
    entry.selector = 0x08;
    entry.offset2 = (ptrint)(routine) & maxword;

    idt[vector] = entry;
}

void eoi(byte irq) {
    pic::eoi(irq);
}

void init_idt() {
    idtr.offset = (dword)idt;
    idtr.size = sizeof(idt) - 1;

    __asm__ volatile (
        "lidt %d0"
        :
        : "m"(idtr)
        :
    );
}

void mask_irq(byte irq) {
    pic::mask_irq(irq);
}

void unmask_irq(byte irq) {
    pic::unmask_irq(irq);
}

word read_isr() {
    return pic::read_isr();
}

#define register_irq(n) register_handle(irq##n, irq_offset + n, 0x00)

void init() {
    pic::init();

    register_handle(div_error,                  0x00, 0x00);
    register_handle(debug_error,                0x01, 0x00);
    register_handle(nmi_handle,                 0x02, 0x00);
    register_handle(breakpoint_handle,          0x03, 0x00);
    register_handle(overflow_error,             0x04, 0x00);
    register_handle(bound_error,                0x05, 0x00);
    register_handle(ud_opcode_error,            0x06, 0x00);
    register_handle(dev_not_available_error,    0x07, 0x00);
    register_handle(double_fault,               0x08, 0x00);
    register_handle(coproc_seg_overrun,         0x09, 0x00);
    register_handle(invalid_tss_handle,         0x0A, 0x00);
    register_handle(seg_not_present_error,      0x0B, 0x00);
    register_handle(stack_seg_fault,            0x0C, 0x00);
    register_handle(gpf_handle,                 0x0D, 0x00);
    register_handle(pf_handle,                  0x0E, 0x00);
    register_handle(x87_fpu_error,              0x10, 0x00);
    register_handle(align_check_error,          0x11, 0x00);
    register_handle(machine_check_error,        0x12, 0x00);
    register_handle(simd_fp_error,              0x13, 0x00);
    register_handle(virt_error,                 0x14, 0x00);
    register_handle(control_prot_error,         0x15, 0x00);

    register_irq(0);
    register_irq(1);
    register_irq(2);
    register_irq(3);
    register_irq(4);
    register_irq(5);
    register_irq(6);
    register_irq(7);
    register_irq(8);
    register_irq(9);
    register_irq(10);
    register_irq(11);
    register_irq(12);
    register_irq(13);
    register_irq(14);
    register_irq(15);

    init_idt();
    sti();
}

NAMESPACE_END()