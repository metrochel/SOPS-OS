#ifndef _INT_INCL
#define _INT_INCL
#include "../io/io.hpp"
#include "../libk/nums.hpp"

#define isr(name) __attribute__((interrupt)) void name([[maybe_unused]] IntFrame* frame)
#define irq(irq_no) isr(irq##irq_no)

struct IDT_Register {
    word size;
    byte* base;
} __attribute__((packed));

struct IntFrame {
    word IP;
    word CS;
    word Flags;
    word SP;
    word SS;
};

extern IDT_Register *idtr;
extern const byte irqOffset;

/// @brief Инициализирует прерывания, доступные процессору.
void initInts();

/// @brief Загружает новую таблицу дескрипторов прерываний.
/// @param r Регистр таблицы
inline void lidt(IDT_Register r) {
    __asm__ ("lidt %0" : : "m"(r));
}

/// @brief Обработчик ошибки деления на 0 (прерывание 0x00)
isr(zero_divide_err);

/// @brief Обработчик ошибки переполнения (прерывание 0x01)
isr(overflow_err);

/// @brief Обработчик ошибки провала индекса (out-of-bounds) (прерывание 0x05)
isr(bound_err);

/// @brief Обработчик ошибки неверной инструкции (прерывание 0x06)
isr(invalid_opcode_err);

/// @brief Обработчик ошибки недоступного устройства (прерывание 0x07)
isr(dev_unavailable_err);

/// @brief Обработчик двойного сбоя (прерывание 0x08)
isr(double_fault);

/// @brief Обработчик ошибки неправильного переключения на процесс (прерывание 0x0A)
isr(invalid_task_switch_err);

/// @brief Обработчик ошибки ненастоящего сегмента (прерывание 0x0B)
isr(seg_not_present_err);

/// @brief Обработчик ошибки сегмента стека (прерывание 0x0C)
isr(stack_seg_fault);

/// @brief Обработчик общей ошибки защиты (прерывание 0x0D)
isr(general_prot_fault);

/// @brief Обработчик ошибки страниц (прерывание 0x0E)
isr(page_fault);

/// @brief Обработчик ошибки дробных чисел (прерывание 0x10)
isr(float_exception);

/// @brief Обработчик ошибки нарушения ровнения (прерывание 0x11)
isr(align_check);

/// @brief Запрос-прерывание 0 - Таймер
irq(0);

/// @brief Запрос-прерывание 1 - Ввод с клавиатуры
irq(1);

/// @brief Запрос-прерывание 2 - Каскадное прерывание (не вызывается)
/// @attention Маскировать IRQ 2 нельзя! Иначе IRQ 8-15 не будут вызываться!
irq(2);

/// @brief Запрос-прерывание 3 - Событие порта COM2
irq(3);

/// @brief Запрос-прерывание 4 - Событие порта COM1
irq(4);

/// @brief Запрос-прерывание 5 - Событие порта LPT2
irq(5);

/// @brief Запрос-прерывание 6 - Событие дискетного контроллера
irq(6);

/// @brief Запрос-прерывание 7 - Событие порта LPT1 / "Спонтанное" прерывание
irq(7);

/// @brief Запрос-прерывание 8 - Событие CMOS-часов
irq(8);

/// @brief Запрос-прерывание 9 - не используется (пока что)
irq(9);

/// @brief Запрос-прерывание 10 - не используется (пока что)
irq(10);

/// @brief Запрос-прерывание 11 - не используется (пока что)
irq(11);

/// @brief Запрос-прерывание 12 - Событие мыши PS/2
irq(12);

/// @brief Запрос-прерывание 13 - Событие математического сопроцессора
irq(13);

/// @brief Запрос-прерывание 14 - Событие главного диска ATA
irq(14);

/// @brief Запрос-прерывание 15 - Событие побочного диска ATA
irq(15);

/// @brief Обработчик системных вызовов (прерывание 0xC0)
extern isr(syscall_int);

/// @brief Выход из прерывания, вызванного главным PIC
inline void int_exit_master() {
    outb(0x20, 0x20);
}

/// @brief Выход из прерывания, вызванного побочным PIC
inline void int_exit_slave() {
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void encode_idt_entry(void (*handle)(IntFrame*), byte int_num, byte perms);
void encode_idt_entry(void (*handle)(IntFrame*), byte int_num);

#endif