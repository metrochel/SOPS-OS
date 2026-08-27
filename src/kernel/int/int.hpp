/*
 * int.hpp - Библиотека прерываний
 *
 * Объявляет функции для работы с прерываниями.
 */

#ifndef _INT_INCL
#define _INT_INCL

#include "typedefs.hpp"

namespace interrupt {
    extern idt_entry32 idt[];

    const byte irq_offset = 0x20;

    /// Вызывает прерывание @c int_no.
    /// @param int_no Номер вызываемого прерывания
    inline void raise(byte int_no) {
        __asm__ volatile (
            "int %b0"
            :
            : "i"(int_no)
            :
        );
    }

    /// @brief Устанавливает флаг прерываний (asm-инструкция @c STI ).
    inline void sti() {
        __asm__ volatile ("sti");
    }

    /// @brief Очищает флаг прерываний (asm-инструкция @c CLI ).
    inline void cli() {
        __asm__ volatile ("cli");
    }

    void register_handle(handle int_routine, byte vector, byte cpl);

    void eoi(byte irq);

    void unmask_irq(byte irq);

    void mask_irq(byte irq);

    word read_isr();

    void init();
}

#endif //_INT_INCL