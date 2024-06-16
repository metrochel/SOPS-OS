#include "int.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"
#ifndef KB_BUF_BASE
#include "../keyboard/keyboard.hpp"
#endif

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

    encode_idt_entry(irq0, 0x20);
    encode_idt_entry(irq1, 0x21);
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
    uint16_t flags = 0x8E00;
    *entryPtr = flags;
    entryPtr++;
    uint16_t offset2 = (uint32_t)handlePtr >> 16;
    *entryPtr = offset2;
}

__attribute__((interrupt)) void zero_divide_err(IntFrame* frame) {
    kerror("\nОШИБКА: Деление на ноль\n");
}

__attribute__((interrupt)) void overflow_err(IntFrame* frame) {
    kerror("\nОШИБКА: Переполнение\n");
}

__attribute__((interrupt)) void bound_err(IntFrame* frame) {
    kerror("\nОШИБКА: Индекс превышает длину массива\n");
}

__attribute__((interrupt)) void invalid_opcode_err(IntFrame* frame) {
    kerror("\nОШИБКА: Невозможная инструкция\n");
}

__attribute__((interrupt)) void dev_unavailable_err(IntFrame* frame) {
    kerror("\nОШИБКА: Устройство недоступно\n");
}

__attribute__((interrupt)) void double_fault(IntFrame* frame) {
    kerror("\nОШИБКА: Двойной сбой\n");
    kwarn("Возврат невозможен!\n");
    __asm__ ("hlt");
}

__attribute__((interrupt)) void invalid_task_switch_err(IntFrame* frame) {
    kerror("\nОШИБКА: Неправильный переход на процесс\n");
}

__attribute__((interrupt)) void seg_not_present_err(IntFrame* frame) {
    kerror("\nОШИБКА: Сегмент ненастоящий\n");
}

__attribute__((interrupt)) void stack_seg_fault(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой сегмента стека\n");
}

__attribute__((interrupt)) void general_prot_fault(IntFrame* frame) {
    kerror("\nОШИБКА: Общий сбой защиты\n");
    kerror("Адрес сбоя: %x\n", frame->IP);
    __asm__ ("hlt");
}

__attribute__((interrupt)) void page_fault(IntFrame* frame) {
    uint32_t errorcode;
    __asm__ ("pop %l0" : "=r"(errorcode) : );
    kerror("\nОШИБКА: Страничный сбой\n");
    kerror("Адрес сбоя: %x\n", frame->IP);
}

__attribute__((interrupt)) void float_exception(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой математического сопроцессора\n");
}

__attribute__((interrupt)) void align_check(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой ровнения\n");
}

__attribute__((interrupt)) void irq0(IntFrame* frame) {
    updateCursor();
    int_exit_master();
}

__attribute__((interrupt)) void irq1(IntFrame* frame) {
    while (!(inb(0x64) & 1)) {io_wait();}
    uint8_t scancode = inb(0x60);
    for (int i = 0; i < 100; i++) {io_wait();}
    if (scancode == KB_ACK) {
        uint8_t ackdCmd = *(uint8_t*)KB_CMD_BUF_BASE;
        uint8_t bufLen = (uint32_t)kbCmdBufPtr - KB_CMD_BUF_BASE;
        for (uint8_t i = 0; i < bufLen; i++) {
            *(uint16_t*)(KB_CMD_BUF_BASE + i) >>= 8;
        }
        bufLen --;
        if (ackdCmd == KB_CMD_KEYSET || ackdCmd == KB_CMD_SET_LEDS || ackdCmd == KB_CMD_SET_TYPEMATIC) {
            for (uint8_t i = 0; i < bufLen - 1; i++) {
                *(uint16_t*)(KB_CMD_BUF_BASE + i) >>= 8;
            }
            bufLen --;
        }
        uint8_t nextCmd = *(uint8_t*)KB_CMD_BUF_BASE;
        if (nextCmd == 0) {
            int_exit_master();
            return;
        }
        sendPS2DevCommand(1, nextCmd);
        if (nextCmd == KB_CMD_KEYSET || nextCmd == KB_CMD_SET_LEDS || nextCmd == KB_CMD_SET_TYPEMATIC) {
            uint8_t arg = *(uint8_t*)(KB_CMD_BUF_BASE + 1);
            sendPS2DevCommand(1, arg);
        }
        int_exit_master();
        return;
    }
    // if (scancode == KB_RESEND) {
    //     cmdNeedsResending = true;
    //     int_exit_master();
    //     return;
    // }
    if (cmdAwaitingResponse) {
        *kbBufPtr = inb(0x60);
        io_wait();
        return;
    }
    *kbBufPtr = scancode;
    kbBufPtr ++;
    if (scancode == 0xF0) {
        releaseScancode = true;
        int_exit_master();
        return;
    }
    if (scancode == 0xE0 || scancode == 0xE1) {
        int_exit_master();
        return;
    }
    while (inb(0x64) & 1) {
        io_wait();
        *kbBufPtr = inb(0x60);
        io_wait();
        if (*kbBufPtr == 0xF0)
            releaseScancode = true;
        kbBufPtr ++;
    }
    updateKB();
    int_exit_master();
}

__attribute__((interrupt)) void irq12(IntFrame* frame) {
    kprint("IRQ 12\n");
    int_exit_slave();
}

__attribute__((interrupt)) void irq3(IntFrame* frame) {
    disableInts();
    uint16_t ioPort = getIOPort(2);
    uint8_t iir = inb(ioPort + 2);
    uint8_t intState = iir & 6;

    if (intState == COM_IIR_RECEIVE_LINE_STATUS) {
        uint8_t lsr = inb(ioPort + 5);
        if (lsr & COM_LSR_OVERRUN_ERR)
            kerror("ОШИБКА: COM2: утрачены данные\n");
        if (lsr & COM_LSR_PARITY_ERR)
            kerror("ОШИБКА: COM2: нарушена целостность данных\n");
        if (lsr & COM_LSR_FRAMING_ERR)
            kerror("ОШИБКА: COM2: отсутствует бит остановки\n");
        if (lsr & COM_LSR_BREAK_IN)
            kwarn("ВНИМАНИЕ: COM2: ввод данных прерван\n");
        if (lsr & COM_LSR_IMPEND_ERR)
            kerror("ОШИБКА: COM2: неправильные передаваемые данные\n");
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
            kwarn("ВНИМАНИЕ: COM2: модем не готов к работе\n");
        if (msr & COM_MSR_RING_INDICATOR)
            kprint("COM2: поступил звонок\n");
        if (msr & COM_MSR_DATA_CARRIER_DETECT)
            kwarn("ВНИМАНИЕ: COM2: модем разорвал соединение\n");
        if (msr & COM_MSR_CLEAR_TO_SEND)
            kwarn("ВНИМАНИЕ: COM2: устройство не готово к чтению\n");
        else
            kprint("COM2: устройство готово к чтению\n");
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
            kerror("ОШИБКА: COM1: утрачены данные\n");
        if (lsr & COM_LSR_PARITY_ERR)
            kerror("ОШИБКА: COM1: нарушена целостность данных\n");
        if (lsr & COM_LSR_FRAMING_ERR)
            kerror("ОШИБКА: COM1: отсутствует бит остановки\n");
        if (lsr & COM_LSR_BREAK_IN)
            kwarn("ВНИМАНИЕ: COM1: ввод данных прерван\n");
        if (lsr & COM_LSR_IMPEND_ERR)
            kerror("ОШИБКА: COM1: неправильные передаваемые данные\n");
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
            kwarn("ВНИМАНИЕ: COM1: модем не готов к работе\n");
        if (msr & COM_MSR_RING_INDICATOR)
            kprint("COM1: поступил звонок\n");
        if (msr & COM_MSR_DATA_CARRIER_DETECT)
            kwarn("ВНИМАНИЕ: COM1: модем разорвал соединение\n");
        if (msr & COM_MSR_CLEAR_TO_SEND)
            kwarn("ВНИМАНИЕ: COM1: устройство не готово к чтению\n");
    }
    enableInts();
    int_exit_master();
}