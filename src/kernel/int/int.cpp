#include "int.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"

IDT_Register idtr{ 50*8-1, (uint8_t*)0x10000  };

void initInts() {
    for (int i = 0; i < idtr.size; i++)
        *(idtr.base + i) = 0;

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

    encode_idt_entry(irq3, 0x23);
    encode_idt_entry(irq4, 0x24);

    lidt(idtr);
    enableInts();
}

void encode_idt_entry(void (*handlePtr)(IntFrame*), uint8_t intNum) {
    uint16_t *entryPtr = (uint16_t*)idtr.base + 4 * intNum;
    uint16_t offset1 = (uint32_t)handlePtr & 0xFFFF;
    *entryPtr = offset1;
    entryPtr++;
    uint16_t selector = 0x8;
    *entryPtr = selector;
    entryPtr++;
    uint16_t flags = 0x8F00;
    *entryPtr = flags;
    entryPtr++;
    uint16_t offset2 = (uint32_t)handlePtr >> 16;
    *entryPtr = offset2;
}

__attribute__((interrupt)) void zero_divide_err(IntFrame* frame) {
    kerror("ОШИБКА: Деление на ноль");
}

__attribute__((interrupt)) void overflow_err(IntFrame* frame) {
    kerror("ОШИБКА: Переполнение");
}

__attribute__((interrupt)) void bound_err(IntFrame* frame) {
    kerror("ОШИБКА: Индекс превышает длину массива");
}

__attribute__((interrupt)) void invalid_opcode_err(IntFrame* frame) {
    kerror("ОШИБКА: Невозможная инструкция");
}

__attribute__((interrupt)) void dev_unavailable_err(IntFrame* frame) {
    kerror("ОШИБКА: Устройство недоступно");
}

__attribute__((interrupt)) void double_fault(IntFrame* frame) {
    kerror("ОШИБКА: Двойной сбой");
    kwarn("Возврат невозможен!");
    __asm__ ("hlt");
}

__attribute__((interrupt)) void invalid_task_switch_err(IntFrame* frame) {
    kerror("ОШИБКА: Неправильный переход на процесс");
}

__attribute__((interrupt)) void seg_not_present_err(IntFrame* frame) {
    kerror("ОШИБКА: Сегмент ненастоящий");
}

__attribute__((interrupt)) void stack_seg_fault(IntFrame* frame) {
    kerror("ОШИБКА: Сбой сегмента стека");
}

__attribute__((interrupt)) void general_prot_fault(IntFrame* frame) {
    kerror("ОШИБКА: Общий сбой защиты");
}

__attribute__((interrupt)) void page_fault(IntFrame* frame) {
    kerror("ОШИБКА: Страничный сбой");
}

__attribute__((interrupt)) void float_exception(IntFrame* frame) {
    kerror("ОШИБКА: Сбой математического сопроцессора");
}

__attribute__((interrupt)) void align_check(IntFrame* frame) {
    kerror("ОШИБКА: Сбой ровнения");
}

__attribute__((interrupt)) void irq3(IntFrame* frame) {
    disableInts();
    uint16_t ioPort = getIOPort(2);
    uint8_t iir = inb(ioPort + 2);
    uint8_t intState = iir & 6;

    if (intState == COM_IIR_RECEIVE_LINE_STATUS) {
        uint8_t lsr = inb(ioPort + 5);
        if (lsr & COM_LSR_OVERRUN_ERR)
            kerror("ОШИБКА: COM2: утрачены данные");
        if (lsr & COM_LSR_PARITY_ERR)
            kerror("ОШИБКА: COM2: нарушена целостность данных");
        if (lsr & COM_LSR_FRAMING_ERR)
            kerror("ОШИБКА: COM2: отсутствует бит остановки");
        if (lsr & COM_LSR_BREAK_IN)
            kwarn("ВНИМАНИЕ: COM2: ввод данных прерван");
        if (lsr & COM_LSR_IMPEND_ERR)
            kerror("ОШИБКА: COM2: неправильные передаваемые данные");
    }
    else if (intState == COM_IIR_RECEIVE_DATA_AVL) {
        comIn(2);
    }
    else if (intState == COM_IIR_TRANS_HOL_REG_EMPTY) {
        comSend(2);
    }
    else if (intState == COM_IIR_MODEM_STATUS) {
        uint8_t msr = inb(ioPort + 6);
        if (msr & COM_MSR_DATA_SET_READY)
            kwarn("ВНИМАНИЕ: COM2: модем не готов к работе");
        if (msr & COM_MSR_RING_INDICATOR)
            kprint("COM2: поступил звонок");
        if (msr & COM_MSR_DATA_CARRIER_DETECT)
            kwarn("ВНИМАНИЕ: COM2: модем разорвал соединение");
        if (msr & COM_MSR_CLEAR_TO_SEND)
            kwarn("ВНИМАНИЕ: COM2: устройство не готово к чтению");
        else
            kprint("COM2: устройство готово к чтению");
    }
    enableInts();
    int_exit_master();
}

__attribute__((interrupt)) void irq4(IntFrame* frame) {
    disableInts();
    uint16_t ioPort = getIOPort(1);
    uint8_t iir = inb(ioPort + 2);
    uint8_t intState = iir & 6;

    if (intState == COM_IIR_RECEIVE_LINE_STATUS) {
        uint8_t lsr = inb(ioPort + 5);
        if (lsr & COM_LSR_OVERRUN_ERR)
            kerror("ОШИБКА: COM1: утрачены данные");
        if (lsr & COM_LSR_PARITY_ERR)
            kerror("ОШИБКА: COM1: нарушена целостность данных");
        if (lsr & COM_LSR_FRAMING_ERR)
            kerror("ОШИБКА: COM1: отсутствует бит остановки");
        if (lsr & COM_LSR_BREAK_IN)
            kwarn("ВНИМАНИЕ: COM1: ввод данных прерван");
        if (lsr & COM_LSR_IMPEND_ERR)
            kerror("ОШИБКА: COM1: неправильные передаваемые данные");
    }
    else if (intState == COM_IIR_RECEIVE_DATA_AVL) {
        comIn(1);
    }
    else if (intState == COM_IIR_TRANS_HOL_REG_EMPTY) {
        comSend(1);
    }
    else if (intState == COM_IIR_MODEM_STATUS) {
        uint8_t msr = inb(ioPort + 6);
        if (msr & COM_MSR_DATA_SET_READY)
            kwarn("ВНИМАНИЕ: COM1: модем не готов к работе");
        if (msr & COM_MSR_RING_INDICATOR)
            kprint("COM1: поступил звонок");
        if (msr & COM_MSR_DATA_CARRIER_DETECT)
            kwarn("ВНИМАНИЕ: COM1: модем разорвал соединение");
        if (msr & COM_MSR_CLEAR_TO_SEND)
            kwarn("ВНИМАНИЕ: COM1: устройство не готово к чтению");
    }
    enableInts();
    int_exit_master();
}