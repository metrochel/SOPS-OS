#include <stdint.h>

#ifndef _INT_INCL
#define _INT_INCL
#include "../io/io.hpp"

struct IDT_Register {
    uint16_t size;
    uint8_t* base;
} __attribute__((packed));

struct IntFrame {
    uint16_t IP;
    uint16_t CS;
    uint16_t Flags;
    uint16_t SP;
    uint16_t SS;
};

extern IDT_Register idtr;

/// @brief Инициализирует прерывания, доступные процессору.
void initInts();

/// @brief Загружает новую таблицу дескрипторов прерываний.
/// @param r Регистр таблицы
inline void lidt(IDT_Register r) {
    __asm__ ("lidt %0" : : "m"(r));
}

/// @brief Включает прерывания.
inline void enableInts() {
    __asm__ ("sti");
}

/// @brief Выключает прерывания.
inline void disableInts() {
    __asm__ ("cli");
}

/// @brief Обработчик ошибки деления на 0 (прерывание 0x00)
void zero_divide_err(IntFrame* frame);

/// @brief Обработчик ошибки переполнения (прерывание 0x01)
void overflow_err(IntFrame* frame);

/// @brief Обработчик ошибки провала индекса (out-of-bounds) (прерывание 0x05)
void bound_err(IntFrame* frame);

/// @brief Обработчик ошибки неверной инструкции (прерывание 0x06)
void invalid_opcode_err(IntFrame* frame);

/// @brief Обработчик ошибки недоступного устройства (прерывание 0x07)
void dev_unavailable_err(IntFrame* frame);

/// @brief Обработчик двойного сбоя (прерывание 0x08)
void double_fault(IntFrame* frame);

/// @brief Обработчик ошибки неправильного переключения на процесс (прерывание 0x0A)
void invalid_task_switch_err(IntFrame* frame);

/// @brief Обработчик ошибки ненастоящего сегмента (прерывание 0x0B)
void seg_not_present_err(IntFrame* frame);

/// @brief Обработчик ошибки сегмента стека (прерывание 0x0C)
void stack_seg_fault(IntFrame* frame);

/// @brief Обработчик общей ошибки защиты (прерывание 0x0D)
void general_prot_fault(IntFrame* frame);

/// @brief Обработчик ошибки страниц (прерывание 0x0E)
void page_fault(IntFrame* frame);

/// @brief Обработчик ошибки дробных чисел (прерывание 0x10)
void float_exception(IntFrame* frame);

/// @brief Обработчик ошибки нарушения ровнения (прерывание 0x11)
void align_check(IntFrame* frame);

/// @brief Запрос-прерывание 0 - Таймер
void irq0(IntFrame* frame);

/// @brief Запрос-прерывание 1 - Ввод с клавиатуры
void irq1(IntFrame* frame);

/// @brief Запрос-прерывание 2 - Каскадное прерывание (не вызывается)
/// @attention Маскировать IRQ 2 нельзя! Иначе IRQ 8-15 не будут вызываться!
void irq2(IntFrame* frame);

/// @brief Запрос-прерывание 3 - Событие порта COM2
void irq3(IntFrame* frame);

/// @brief Запрос-прерывание 4 - Событие порта COM1
void irq4(IntFrame* frame);

/// @brief Запрос-прерывание 5 - Событие порта LPT2
void irq5(IntFrame* frame);

/// @brief Запрос-прерывание 6 - Событие дискетного контроллера
void irq6(IntFrame* frame);

/// @brief Запрос-прерывание 7 - Событие порта LPT1 / "Спонтанное" прерывание
void irq7(IntFrame* frame);

/// @brief Запрос-прерывание 8 - Событие CMOS-часов
void irq8(IntFrame* frame);

/// @brief Запрос-прерывание 9 - не используется (пока что)
void irq9(IntFrame* frame);

/// @brief Запрос-прерывание 10 - не используется (пока что)
void irq10(IntFrame* frame);

/// @brief Запрос-прерывание 11 - не используется (пока что)
void irq11(IntFrame* frame);

/// @brief Запрос-прерывание 12 - Событие мыши PS/2
void irq12(IntFrame* frame);

/// @brief Запрос-прерывание 13 - Событие математического сопроцессора
void irq13(IntFrame* frame);

/// @brief Запрос-прерывание 14 - Событие главного диска ATA
void irq14(IntFrame* frame);

/// @brief Запрос-прерывание 15 - Событие побочного диска ATA
void irq15(IntFrame* frame);

/// @brief Выход из прерывания, вызванного главным PIC
inline void int_exit_master() {
    outb(0x20, 0x20);
}

/// @brief Выход из прерывания, вызванного побочным PIC
inline void int_exit_slave() {
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void encode_idt_entry(void (*handlePtr)(IntFrame*), uint8_t intNum);

#endif