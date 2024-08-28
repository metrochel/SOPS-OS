#include "pic.hpp"

void setPICOffsets(byte offset1, byte offset2) {
    outb(PIC1_CMD, 0x10 | 0x1);
    io_wait();
    outb(PIC2_CMD, 0x10 | 0x1);
    io_wait();

    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, 0x1);
    io_wait();
    outb(PIC2_DATA, 0x1);
    io_wait();

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void maskIRQ(byte irq) {
    word ioPort = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    byte mask = inb(ioPort);
    mask |= (1 << (irq % 8));
    outb(ioPort, mask);
}

void unmaskIRQ(byte irq) {
    word ioPort = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    byte mask = inb(ioPort);
    mask &= ~(1 << (irq % 8));
    outb(ioPort, mask);
}

word getISR() {
    outb(PIC1_CMD, 0x0B);
    outb(PIC2_CMD, 0x0B);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

word getIRR() {
    outb(PIC1_CMD, 0x0A);
    outb(PIC2_CMD, 0x0A);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

word getIRQMask() {
    return (inb(PIC2_DATA) << 8) | inb(PIC1_DATA);
}