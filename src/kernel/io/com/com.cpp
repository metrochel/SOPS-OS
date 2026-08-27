/*
 * com.cpp - Драйвер последовательного порта COM
 */

#include "com.hpp"

#include "graphics/graphics.hpp"
#include "int/int.hpp"
#include "dbg/dbgutil.hpp"

NAMESPACE_BEGIN(com)

/* === Переменные === */

byte com_port::ports_count = 0;
list<com_port*> com_ports;

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
void template_write_reg(word port, byte reg) {
    outb(port + offset, reg);
}

#define decl_read_reg(class_name, reg_name, offset)             \
    class_name& com_port::read_##reg_name () {                  \
        reg_name = template_read_reg<offset>(io_port);          \
        return (class_name&)reg_name;                           \
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
        template_write_reg<offset>(io_port, (byte)reg_name);                \
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

com_port::com_port(word io_port) : read_buf(0, io_wait), write_buf(0, io_wait) {
    this->io_port = io_port;

    set_baud_rate(default_baud_rate);

    ier.data_available = ier.modem_status = ier.rec_line_status = ier.trans_hold_reg_empty = 0;
    write_ier();

    lcr.data = data_bits::len8;
    lcr.parity = parity_bits::none;
    lcr.stop = stop_bits::bits1;
    write_lcr();

    fcr.trigger_level = fifo_int_level::byte1;
    fifo_sz = 1;
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

    port_no = ++ports_count;
    available = true;

    ier.data_available = ier.modem_status = ier.rec_line_status = 1;
    write_ier();

    if (port_no == 1 || port_no == 3) {
        irq_no = 4;
    } else if (port_no == 2 || port_no == 4) {
        irq_no = 3;
    } else {
        irq_no = maxbyte;
    }

    read_buf.reallocate(default_buffer_sz);
    write_buf.reallocate(default_buffer_sz);

    set_modifier({ dec, text, false });

    ier.data_available = ier.modem_status = ier.rec_line_status = 1;
    ier.trans_hold_reg_empty = 0;
    write_ier();
}

com_port& com_port::operator=(com_port const& ref) {
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
    write_buf.write(ch);
}

dword com_port::get() {
    byte ch;
    read_buf.read(ch);
    return ch;
}

void com_port::flush() {
    if (!ier.trans_hold_reg_empty) {
        ier.trans_hold_reg_empty = 1;
        write_ier();
    }
}

/* === Класс com_port - Обработчик прерывания === */

void com_port::handle_interrupt() {
    read_iir();

    if (iir.int_pending)
        return;

    if (iir.int_state == com_int::modem_status) {
        read_msr();
    }

    if (iir.int_state == com_int::trans_hold_reg_empty) {
        dword diff = write_buf.diff();
        for (byte i = 0; i < fifo_sz; i++) {
            if (!diff) break;
            byte data;
            write_buf.read(data);
            outb(io_port, data);
            diff--;
        }

        if (!diff) {
            ier.trans_hold_reg_empty = 0;
            write_ier();
        }
    }

    if (iir.int_state == com_int::rec_data_available) {
        byte data = inb(io_port);
        read_buf.write(data);
        cout << data << " ";
    }

    if (iir.int_state == com_int::rec_line_status) {
        read_lsr();
        if (lsr.framing_err)
            cerr << "F";
        if (lsr.overrun_err)
            cerr << "O";
        if (lsr.impending_err)
            cerr << "I";
        if (lsr.break_ind)
            cwrn << "B";
    }
}

/* === Общий обработчик прерывания === */

void handle_com_irq(byte irq) {
//    debug::magic_breakpoint();

    if (com_ports.length() == 0)
        return;

    for (com_port *port : com_ports) {
        if (port->get_irq() == irq) {
            port->handle_interrupt();
        }
    }
}

/* === Функция инициализации === */

void init() {
    for (dword std_io_port : std_io_ports) {
        com_port *port = new com_port(std_io_port);
        if (port->is_available()) {
            com_ports.add(port);
        } else {
            delete port;
        }
    }

    if (com_ports.length()) {
        interrupt::unmask_irq(3);
        interrupt::unmask_irq(4);
    }
}

NAMESPACE_END(com)