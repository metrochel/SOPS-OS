/*
 * pic.hpp - Драйвер для контроллера PIC
 */

#include "pic.hpp"
#include "libk/objs.hpp"
#include "io/cpu_io.hpp"
#include "../int.hpp"

NAMESPACE_BEGIN(interrupt::pic)

const byte cmd_eoi = 0x20;

byte master_mask = maxbyte;
byte slave_mask = maxbyte;

void mask_irq(byte irq) {
    if (irq >= 8) {
        irq -= 8;
        if (slave_mask & (1 << irq))
            return;

        slave_mask |= 1 << irq;
        outb(slave_data, slave_mask);
        return;
    }

    if (master_mask & (1 << irq))
        return;

    master_mask |= 1 << irq;
    outb(master_data, master_mask);
}

void unmask_irq(byte irq) {
    if (irq >= 8) {
        irq -= 8;

        if ((slave_mask & (1 << irq)) == 0)
            return;

        slave_mask &= ~(1 << irq);
        outb(slave_data, slave_mask);
        return;
    }

    if ((master_mask & (1 << irq)) == 0)
        return;

    master_mask &= ~(1 << irq);
    outb(master_data, master_mask);
}

void eoi(byte irq) {
    if (irq >= 8) {
        outb(slave_cmd, cmd_eoi);
        io_wait();
    }

    outb(master_cmd, cmd_eoi);
    io_wait();
}

word read_isr() {
    outb(master_cmd, 0x0A);
    outb(slave_cmd, 0x0A);

    byte master_isr = inb(master_data);
    byte slave_isr = inb(slave_data);

    return ((word)master_isr << 8) | slave_isr;
}

void init() {
    // ICW1
    outb(master_cmd, icw | icw4_present);
    io_wait();
    outb(slave_cmd, icw | icw4_present);
    io_wait();

    // ICW2
    outb(master_data, irq_offset);
    io_wait();
    outb(slave_data, irq_offset + 8);
    io_wait();

    // ICW3
    outb(master_data, 1 << slave_irq);
    io_wait();
    outb(slave_data, slave_irq);
    io_wait();

    // ICW4
    outb(master_data, mode_8086);
    io_wait();
    outb(slave_data, mode_8086);
    io_wait();

    // Начальные маски
    outb(master_data, master_mask);
    io_wait();
    outb(slave_data, slave_mask);
    io_wait();

    unmask_irq(2);
}

NAMESPACE_END()