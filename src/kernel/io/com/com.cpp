/*
 * com.cpp - Драйвер последовательного порта COM
 */

#include "com.hpp"

NAMESPACE_BEGIN(com)

/* === Переменные === */

byte com_port::ports_count = 0;
list<com_port> com_ports;

/* === Класс com_port - Функция проверки порта === */

bool com_port::try_com_port() {
    const byte test_value = 0x69;

    outb(io_port + 7, test_value);
    if (inb(io_port + 7) != test_value)
        return false;

    mcr.loop = 1;
    write_mcr();
    outb(io_port, test_value);
    if (inb(io_port) != test_value)
        return false;

    mcr.loop = 0;
    write_mcr();
    return true;
}

/* === Класс com_port - Функции чтения и записи портов === */

template <byte offset>
byte template_read_reg(word port) {
    return inb(port + offset);
}

template <byte offset>
byte template_write_reg(word port, byte reg) {
    outb(port + offset, reg);
    return reg;
}

#define decl_read_reg(class_name, reg_name, offset)     \
    const class_name& com_port::read_##reg_name () {    \
        reg_name = template_read_reg<offset>(io_port);  \
        return reg_name;                                \
    }

decl_read_reg(interrupt_enable_register, ier, 1)
decl_read_reg(interrupt_id_register, iir, 2)
decl_read_reg(line_control_register, lcr, 3)
decl_read_reg(modem_ctrl_register, mcr, 4)
decl_read_reg(line_status_register, lsr, 5)
decl_read_reg(modem_status_register, msr, 6)

#undef decl_read_reg

#define decl_write_reg(reg_name, offset)                                    \
    void com_port::write_##reg_name () {                                    \
        reg_name = template_write_reg<offset>(io_port, (byte)reg_name);     \
    }

decl_write_reg(ier, 1)
decl_write_reg(fcr, 2)
decl_write_reg(lcr, 3)
decl_write_reg(mcr, 4)

#undef decl_write_reg

void com_port::set_baud_rate(dword new_rate) {
    read_lcr();
    lcr.div_access = 1;
    write_lcr();

    word div = (word)(115200.0 / new_rate);
    outb(io_port, BYTE0(div));
    outb(io_port + 1, BYTE1(div));

    lcr.div_access = 0;
    write_lcr();
}

/* === Класс com_port - Конструктор === */

com_port::com_port(word io_port) {
    __asm__ volatile ("xchgw %bx, %bx;");
    this->io_port = io_port;

    set_baud_rate(default_baud_rate);

    ier.data_available = ier.modem_status = ier.rec_line_status = ier.trans_hold_reg_empty = 0;
    write_ier();

    lcr.data = data_bits::len8;
    lcr.parity = parity_bits::none;
    lcr.stop = stop_bits::bits1;
    write_lcr();

    fcr.trigger_level = fifo_int_level::byte1;
    fcr.enable_fifo = 1;
    fcr.clear_rx_fifo = fcr.clear_tx_fifo = 1;
    write_fcr();
    fcr.clear_rx_fifo = fcr.clear_tx_fifo = 0;
    write_fcr();

    mcr.dtr = 0;
    mcr.rts = 1;
    mcr.out1 = 1;
    mcr.out2 = 1;
    write_mcr();

    bool port_works = try_com_port();

    if (!port_works) {
        return;
    }

    fcr.clear_rx_fifo = fcr.clear_tx_fifo = 1;
    write_fcr();
    fcr.clear_rx_fifo = fcr.clear_tx_fifo = 0;
    write_fcr();

    port_no = ++ports_count;
    available = true;

    ier.data_available = ier.modem_status = ier.rec_line_status = 1;
    write_ier();

    if (port_no == 1 || port_no == 3) {
        irq_no = 3;
    } else if (port_no == 2 || port_no == 4) {
        irq_no = 4;
    } else {
        irq_no = maxbyte;
    }
}

com_port& com_port::operator=(const com_port &ref) {
    lcr = ref.lcr;
    ier = ref.ier;
    fcr = ref.fcr;
    iir = ref.iir;
    mcr = ref.mcr;
    lsr = ref.lsr;
    msr = ref.msr;

    port_no = ref.port_no;
    io_port = ref.io_port;
    irq_no = ref.irq_no;
    available = ref.available;
    baud_rate = ref.baud_rate;

    return *this;
}

/* === Класс com_port - Функции чтения/записи === */

void com_port::put(dword ch) {

}

dword com_port::get() {
    return maxdword;
}

/* === Класс com_port - Обработчик прерывания === */

void com_port::handle_interrupt() {

}

/* === Общий обработчик прерывания === */

void handle_com_irq(byte irq) {
    for (com_port &port : com_ports) {
        if (port.get_irq() == irq) {
            port.handle_interrupt();
        }
    }
}

/* === Функция инициализации === */

void init() {
    for (dword std_io_port : std_io_ports) {
        com_port port(std_io_port);
        if (!port.is_available()) continue;

        com_ports.add(port);
    }
}

NAMESPACE_END(com)