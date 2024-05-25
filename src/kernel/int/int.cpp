#include "int.hpp"
#include "../graphics/glyphs.hpp"

IDT_Register idtr{ 50*8-1, (void*)0x10000  };

void initInts() {
    encode_idt_entry(zero_divide_err, 0);
    encode_idt_entry(overflow_err, 1);
    encode_idt_entry(bound_err, 5);
    encode_idt_entry(invalid_opcode_err, 6);
    encode_idt_entry(dev_unavailable_err, 7);
    encode_idt_entry(double_fault, 8);
    encode_idt_entry(invalid_task_switch_err, 0xA);
    encode_idt_entry(seg_not_present_err, 0x0B);
    encode_idt_entry(stack_seg_fault, 0x0C);
    encode_idt_entry(general_prot_fault, 0x0D);
    encode_idt_entry(page_fault, 0x0E);
    encode_idt_entry(float_exception, 0x10);
    encode_idt_entry(align_check, 0x11);
    *(uint8_t*)0x100340 = sizeof(IDT_Register);
    lidt(idtr);
}

void encode_idt_entry(void (*handlePtr)(IntFrame*), uint8_t intNum) {
    uint16_t *entryPtr = (uint16_t*)idtr.base + 4 * intNum;
    uint16_t offset1 = (uint32_t)handlePtr & 0xFFFF;
    *entryPtr = offset1;
    entryPtr++;
    uint16_t selector = 0x8;
    *entryPtr = selector;
    entryPtr++;
    uint16_t flags = 0x8E00;
    *entryPtr = flags;
    entryPtr++;
    uint16_t offset2 = (uint32_t)handlePtr >> 16;
    *entryPtr = offset2;
}

__attribute__((interrupt)) void zero_divide_err(IntFrame* frame) {
    kerror("ОШИБКА: Деление на ноль");
    int_exit_master();
}

__attribute__((interrupt)) void overflow_err(IntFrame* frame) {
    kerror("ОШИБКА: Переполнение");
    int_exit_master();
}

__attribute__((interrupt)) void bound_err(IntFrame* frame) {
    kerror("ОШИБКА: Индекс превышает длину массива");
    int_exit_master();
}

__attribute__((interrupt)) void invalid_opcode_err(IntFrame* frame) {
    kerror("ОШИБКА: Невозможная инструкция");
    int_exit_master();
}

__attribute__((interrupt)) void dev_unavailable_err(IntFrame* frame) {
    kerror("ОШИБКА: Устройство недоступно");
    int_exit_master();
}

__attribute__((interrupt)) void double_fault(IntFrame* frame) {
    kerror("ОШИБКА: Двойной сбой");
    kwarn("Возврат невозможен!");
    __asm__ ("hlt");
}

__attribute__((interrupt)) void invalid_task_switch_err(IntFrame* frame) {
    kerror("ОШИБКА: Неправильный переход на процесс");
    int_exit_master();
}

__attribute__((interrupt)) void seg_not_present_err(IntFrame* frame) {
    kerror("ОШИБКА: Сегмент ненастоящий");
    int_exit_master();
}

__attribute__((interrupt)) void stack_seg_fault(IntFrame* frame) {
    kerror("ОШИБКА: Сбой сегмента стека");
    int_exit_master();
}

__attribute__((interrupt)) void general_prot_fault(IntFrame* frame) {
    kerror("ОШИБКА: Общий сбой защиты");
    int_exit_master();
}

__attribute__((interrupt)) void page_fault(IntFrame* frame) {
    kerror("ОШИБКА: Страничный сбой");
    int_exit_master();
}

__attribute__((interrupt)) void float_exception(IntFrame* frame) {
    kerror("ОШИБКА: Сбой математического сопроцессора");
    int_exit_master();
}

__attribute__((interrupt)) void align_check(IntFrame* frame) {
    kerror("ОШИБКА: Сбой ровнения");
    int_exit_master();
}