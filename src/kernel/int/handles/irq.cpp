/*
 * irq.hpp - Обработчики IRQ
 */

#include "irq.hpp"

#include "int/int.hpp"
#include "libk/objs.hpp"
#include "io/com/com.hpp"

NAMESPACE_BEGIN(interrupt::handles)

irq(0) {
    eoi(0);
}

irq(1) {
    eoi(1);
}

irq(2) {
    eoi(2);
}

irq(3) {
    com::handle_com_irq(3);

    eoi(3);
}

irq(4) {
    com::handle_com_irq(4);

    eoi(4);
}

irq(5) {
    eoi(5);
}

irq(6) {
    eoi(6);
}

irq(7) {
    word isr = read_isr();
    if (!test_bit(isr, 7))
        return;

    eoi(7);
}

irq(8) {
    eoi(8);
}

irq(9) {
    eoi(9);
}

irq(10) {
    eoi(10);
}

irq(11) {
    eoi(11);
}

irq(12) {
    eoi(12);
}

irq(13) {
    eoi(13);
}

irq(14) {
    eoi(14);
}

irq(15) {
    word isr = read_isr();
    if (!test_bit(isr, 15))
        return;

    eoi(15);
}

NAMESPACE_END()