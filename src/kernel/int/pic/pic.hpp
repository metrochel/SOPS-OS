/*
 *  pic.hpp - Драйвер для чипа PIC
 *
 *  Предоставляет функции, управляющие чипом PIC 8259.
 */

#ifndef _PIC_INCL
#define _PIC_INCL

#include "libk/nums.hpp"

namespace interrupt::pic {
    enum icw1 {
        icw4_present            = 0x01,
        single_mode             = 0x02,
        call_addr_interval_4    = 0x04,
        level_trig_mode         = 0x08,
        icw                     = 0x10
    };

    enum icw4 {
        mode_8086               = 0x01,
        auto_eoi                = 0x02,
        buf_slave               = 0x08,
        buf_master              = 0x0C,
        special_fully_nest_mode = 0x10
    };

    const word master_cmd = 0x20;
    const word master_data = 0x21;

    const word slave_cmd = 0xA0;
    const word slave_data = 0xA1;

    const byte slave_irq = 2;

    void mask_irq(byte irq);

    void unmask_irq(byte irq);

    void eoi(byte irq);

    word read_isr();

    void init();
}

#endif //_PIC_INCL
