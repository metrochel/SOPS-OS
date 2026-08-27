/*
 * com.hpp - Взаимодействие с последовательным портом COM
 *
 * Предоставляет функции для взаимодействия с последовательным портом COM через протокол RS232.
 */

#ifndef _COM_INCL
#define _COM_INCL

#include "libk/objs.hpp"
#include "../cpu_io.hpp"

/// @c com - это пространство имён, связанное с COM-портами системы.
namespace com {

#define decl_byte_assign(struct_name)   \
    volatile struct_name& operator=(byte n) volatile {    \
        byte *ptr = (byte*)this;        \
        *ptr = n;                       \
        return *this;                   \
    }

#define decl_byte_conv(struct_name) \
    explicit operator byte() volatile { return *(byte*)this; }

#define decl_volatile_assign(struct_name)                                       \
    struct_name& operator=(struct_name&) = default;                             \
    volatile struct_name& operator=(const volatile struct_name &ref) volatile { \
        *this = *(byte*)(&ref);                                                   \
        return *this;                                                           \
    }

#define decl_reg_struct_ops(struct_name) \
    decl_byte_assign(struct_name)        \
    decl_byte_conv(struct_name)          \
    decl_volatile_assign(struct_name)

    /// Стандартные порты шины ЦП для COM
    const dword std_io_ports[] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5E8, 0x4F8, 0x5E8, 0x4E8 };

    /// @c line_control_register описывает состояние регистра контроля соединения порта.
    struct line_control_register {
        byte data           : 2;
        byte stop           : 1;
        byte parity         : 3;
        byte break_enable   : 1;
        byte div_access     : 1;

        decl_reg_struct_ops(line_control_register)
    } __attribute__((packed));

    struct interrupt_enable_register {
        byte data_available         : 1;
        byte trans_hold_reg_empty   : 1;
        byte rec_line_status        : 1;
        byte modem_status           : 1;
        byte __reserved             : 4;

        decl_reg_struct_ops(interrupt_enable_register)
    } __attribute__((packed));

    struct fifo_control_register {
        byte enable_fifo    : 1;
        byte clear_rx_fifo  : 1;
        byte clear_tx_fifo  : 1;
        byte dma_select     : 1;
        byte __reserved     : 2;
        byte trigger_level  : 2;

        decl_reg_struct_ops(fifo_control_register)
    } __attribute__((packed));

    struct interrupt_id_register {
        byte int_pending            : 1;
        byte int_state              : 2;
        byte timeout_int_pending    : 1;
        byte __reserved             : 2;
        byte fifo_state             : 2;

        decl_reg_struct_ops(interrupt_id_register)
    } __attribute__((packed));

    struct modem_ctrl_register {
        byte dtr        : 1;
        byte rts        : 1;
        byte out1       : 1;
        byte out2       : 1;
        byte loop       : 1;
        byte __unused   : 3;

        decl_reg_struct_ops(modem_ctrl_register)
    } __attribute__((packed));

    struct line_status_register {
        byte data_ready             : 1;
        byte overrun_err            : 1;
        byte parity_err             : 1;
        byte framing_err            : 1;
        byte break_ind              : 1;
        byte trans_hold_reg_empty   : 1;
        byte trans_empty            : 1;
        byte impending_err          : 1;

        decl_reg_struct_ops(line_status_register)
    } __attribute__((packed));

    struct modem_status_register {
        byte delta_cts      : 1;
        byte delta_dsr      : 1;
        byte trail_edge_ri  : 1;
        byte delta_dcd      : 1;
        byte invs_cts       : 1;
        byte invs_dsr       : 1;
        byte invs_ri        : 1;
        byte invs_dcd       : 1;

        decl_reg_struct_ops(modem_status_register)
    } __attribute__((packed));

    enum data_bits {
        len5 = 0,
        len6,
        len7,
        len8
    };

    enum stop_bits {
        bits1 = 0,
        bits2 = 1
    };

    enum parity_bits {
        none = 0,
        odd = 1,
        even = 3,
        mark = 5,
        space = 7
    };

    enum fifo_int_level {
        byte1 = 0,
        byte4,
        byte8,
        byte14
    };

    enum com_int {
        modem_status = 0,
        trans_hold_reg_empty,
        rec_data_available,
        rec_line_status
    };


    /// @c com_port - это класс, определяющий один последовательный COM-порт.
    class com_port : public iostream {
        const dword default_baud_rate = 38400;

        const dword default_buffer_sz = 0x10000;

        /// Количество активных портов в системе
        static byte ports_count;

        /// Регистр контроля линии (Line Control Register)
        volatile line_control_register lcr;
        /// Регистр активации прерываний (Interrupt Enable Register)
        volatile interrupt_enable_register ier;
        /// Регистр управления буферами FIFO (FIFO Control Register)
        volatile fifo_control_register fcr;
        /// Регистр установления прерывания (Interrupt ID Register)
        volatile interrupt_id_register iir;
        /// Регистр управления модемом (Modem Control Register)
        volatile modem_ctrl_register mcr;
        /// Регистр статуса линии (Line Status Register)
        volatile line_status_register lsr;
        /// Регистр статуса модема (Modem Status Register)
        volatile modem_status_register msr;

        /// Номер порта COM
        byte port_no;
        /// Номер порта ввода/вывода для данного порта
        word io_port;
        /// Номер IRQ для данного порта
        byte irq_no;
        /// Размер буфера FIFO
        byte fifo_sz;
        /// Флаг; если @c true, то порт доступен
        bool available = false;
        /// Скорость порта, бод
        dword baud_rate;

        /// Буфер чтения
        double_buffer read_buf;
        /// Буфер записи
        double_buffer write_buf;

        /// Проверяет работоспособность порта.
        /// @return @c true, если порт работает @n @c false, если порт не работает
        bool try_com_port();

    private:
        void write_lcr();
        void write_ier();
        void write_fcr();
        void write_mcr();

    public:
        /// @brief Конструирует новый объект порта, для которого основа шины ввода-вывода - @c io_port.
        /// @param io_port Основа шины ввода-выводя для порта
        explicit com_port(word io_port);

        com_port& operator=(com_port const&);

        /// @brief Записывает символ на COM-порт.
        void put(dword ch) override;

        /// @brief Считывает символ с COM-порта.
        dword get() override;

        /// @brief Сливает буфер записи на COM-порт.
        void flush() override;

        /// @brief Возвращает количество последовательных портов, доступных в системе.
        static byte get_total_ports() { return ports_count; }

        /// @brief Возвращает номер IRQ, генерируемого данным портом.
        byte get_irq() { return irq_no; }

        /// @brief Обрабатывает прерывание, вызванное COM-портом.
        /// @note Этот метод должен вызываться только в обработчике IRQ данного порта.
        void handle_interrupt();

        /// @brief Изменяет скорость порта.
        /// @param new_rate Новая скорость порта, бод
        void set_baud_rate(dword new_rate);

        bool is_available() { return available; }

        interrupt_enable_register& read_ier();
        interrupt_id_register& read_iir();
        line_control_register& read_lcr();
        modem_ctrl_register& read_mcr();
        line_status_register& read_lsr();
        modem_status_register& read_msr();
    };

    void handle_com_irq(byte irq);

    extern list<com_port*> com_ports;

    void init();
}

#undef decl_byte_assign
#undef decl_byte_conv
#undef decl_reg_struct_ops

#endif //_COM_INCL
