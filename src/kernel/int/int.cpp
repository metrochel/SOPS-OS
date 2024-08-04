#include "int.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"
#ifndef KB_BUF_BASE
#include "../keyboard/keyboard.hpp"
#endif
#ifndef IDE_COMMAND_PRIMARY
#include "../disk/ide.hpp"
#endif
#ifndef PCI_CONFIG_ADDRESS
#include "../pci/pci.hpp"
#endif
#ifndef CMOS_REGISTER
#include "../timing/cmos.hpp"
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

    encode_idt_entry(irq0,  0x20);
    encode_idt_entry(irq1,  0x21);
    encode_idt_entry(irq2,  0x22);
    encode_idt_entry(irq3,  0x23);
    encode_idt_entry(irq4,  0x24);
    encode_idt_entry(irq8,  0x28);
    encode_idt_entry(irq14, 0x2E);
    encode_idt_entry(irq15, 0x2F);

    lidt(idtr);
    enableInts();
}

void encode_idt_entry(void (*handlePtr)(IntFrame*), uint8_t intNum) {
    uint16_t *entryPtr = (uint16_t*)idtr.base + 4 * intNum;
    uint16_t offset1 = (uint32_t)handlePtr & 0xFFFF;
    *entryPtr++ = offset1;
    uint16_t selector = 0x8;
    *entryPtr++ = selector;
    uint16_t flags = 0x8E00;
    *entryPtr++ = flags;
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
    uint32_t addr;
    __asm__ ("mov %%cr2, %%eax; mov %%eax, %d0" : "=m"(addr) : );
    kerror("\nОШИБКА: Страничный сбой\n");

    kerror("Адрес сбоя: %x\n", addr);
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
    disableInts();
    while (!(inb(0x64) & 1)) {io_wait();}
    uint8_t scancode = inb(0x60);
    io_wait();
    if (scancode == KB_ACK) {
        uint8_t ackdCmd = *(uint8_t*)KB_CMD_BUF_BASE;
        shiftKBCmdQueue();
        cmdAwaitingResponse = ackdCmd == KB_CMD_RESEND_LAST || ackdCmd == KB_CMD_RESET;
        if (!cmdAwaitingResponse)
            sendKBCmd();
        enableInts();
        int_exit_master();
        return;
    }
    if (scancode == KB_RESEND) {
        sendKBCmd();
        enableInts();
        int_exit_master();
        return;
    }
    if (cmdAwaitingResponse) {
        *kbBufPtr = inb(0x60);
        io_wait();
        cmdAwaitingResponse = false;
        shiftKBCmdQueue();
        sendKBCmd();
        enableInts();
        int_exit_master();
        return;
    }
    *kbBufPtr = scancode;
    kbBufPtr ++;
    if (scancode == 0xF0) {
        releaseScancode = true;
        enableInts();
        int_exit_master();
        return;
    }
    if (scancode == 0xE0 || scancode == 0xE1) {
        enableInts();
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
    enableInts();
    int_exit_master();
}

__attribute__((interrupt)) void irq12(IntFrame* frame) {
    kprint("IRQ 12\n");
    int_exit_slave();
}

__attribute__((interrupt)) void irq14(IntFrame* frame) {
    kdebug("\nВызвано IRQ 14.\nПрерывание вызвал ");
    uint8_t ideStatus = inb(IDE_STATUS_PRIMARY);
    uint8_t ataStatus = inb(ATA_STATUS_PRIMARY);
    if (!(ideStatus & 4)) {
        kdebug("не диск.\nВНИМАНИЕ: Прерывание вызвано не диском, обработка прервана\n\n");
        int_exit_slave();
        return;
    }
    kdebug("диск.\n");
    kdebug("Статус 1 канала: %b\n", ideStatus);
    kdebug("Статус контроллера: %b\n", pciConfigReadW(ideCon >> 16, ideCon >> 8, ideCon, 6));
    kdebug("Статус диска: %b\n", ataStatus);
    if ((ideStatus & 2) || (ataStatus & ATA_STATUS_ERROR)) {
        kdebug("ВНИМАНИЕ: Была зафиксирована ошибка\n");
        kdebug("Статус диска: %b\n", inb(ATA_ERROR_PRIMARY));
        uint32_t errSector = (inb(ATA_LBA_HIGH_PRIMARY) << 16) | (inb(ATA_LBA_MID_PRIMARY) << 8) | inb(ATA_LBA_LOW_PRIMARY);
        kdebug("Проблемный сектор: %d\n", errSector);
    }
    uint8_t cmd = inb(IDE_COMMAND_PRIMARY);
    kdebug("Команда 1 канала: %b\n", cmd);
    kdebug("Команда была на ");
    bool read = cmd & 8;
    kdebug(read ? "чтение.\n" : "запись.\n");
    PRD donePrd = *prdt1base++;
    kdebug("Обработанный PRD:\n\tФиз. адрес данных: %x\n\tРазмер блока данных: %d Б\n\tПоследний ли? ", donePrd.base, donePrd.count, donePrd.msb);
    kdebug((donePrd.msb & 0x80) ? "Да\n" : "Нет\n");
    if (prdt1base == prdt1) {
        kdebug("Обработка PRDT завершена. Производится очистка.\n");
        cleanPRDT1();
        prdt1read = read;
        outb(IDE_COMMAND_PRIMARY, 0);
    } else {
        kdebug("Обработка PRDT продолжается.\n");
    }
    outb(IDE_STATUS_PRIMARY, 4);
    kdebug("Новый статус: %b.\n", inb(IDE_STATUS_PRIMARY));
    kdebug("Обработка прерывания завершена успешно.\n\n");
    transferring = false;
    int_exit_slave();
}

__attribute__((interrupt)) void irq15(IntFrame* frame) {
    kdebug("\nВызвано IRQ 15.\nПрерывание вызвал ");
    uint8_t ideStatus = inb(IDE_STATUS_SECONDARY);
    if (!(ideStatus & 4)) {
        kdebug("не диск.\nВНИМАНИЕ: Прерывание вызвано не диском, обработка прервана\n\n");
        int_exit_slave();
        return;
    }
    kdebug("диск.\n");
    kdebug("Статус IDE-контроллера: %b\n", ideStatus);
    uint8_t cmd = inb(IDE_COMMAND_SECONDARY);
    kdebug("Команда была на ");
    bool read = cmd & 8;
    kdebug(read ? "чтение.\n" : "запись.");
    PRD donePrd = *prdt2base++;
    kdebug("Обработанный PRD:\n\tФиз. адрес данных: %x\n\tРазмер блока данных: %d Б\n\tПоследний ли? ", donePrd.base, donePrd.count, donePrd.msb);
    kdebug((donePrd.msb & 0x80) ? "Да\n" : "Нет\n");
    if (prdt2base == prdt2) {
        kdebug("Обработка PRDT завершена. Производится очистка.\n");
        cleanPRDT2();
        prdt2read = read;
        outb(IDE_COMMAND_SECONDARY, 0);
    }
    outb(IDE_STATUS_SECONDARY, 4);
    kdebug("Новый статус: %b.\n", inb(IDE_STATUS_SECONDARY));
    kdebug("Обработка прерывания завершена успешно.\n\n");
    transferring = false;
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
        inb(ioPort + 6);
    }
    enableInts();
    int_exit_master();
}

__attribute__ ((interrupt)) void irq8(IntFrame* frame) {
    uint8_t cmosStatusB = readCMOSReg(0x0B);
    cmosStatusB |= 16;
    writeCMOSReg(0x0B, cmosStatusB);

    Time newtime;
    newtime.seconds = readCMOSReg(0);
    newtime.minutes = readCMOSReg(2);
    uint8_t hours = readCMOSReg(4);
    if (hours & 0x80)
        hours = ((hours & 0x7F) + 12) % 24;
    newtime.hours = hours;
    newtime.day = readCMOSReg(7);
    newtime.month = readCMOSReg(8);
    newtime.year = readCMOSReg(50) * 100 + readCMOSReg(9);
    newtime.weekday = readCMOSReg(6);

    if (!(cmosStatusB & 4)) {
        newtime.seconds = ((newtime.seconds & 0xF0) >> 1) + ((newtime.seconds & 0xF0) >> 3) + (newtime.seconds & 0xf);
        newtime.minutes = ((newtime.minutes & 0xF0) >> 1) + ((newtime.minutes & 0xF0) >> 3) + (newtime.minutes & 0xf);
        newtime.hours = ((newtime.hours & 0xF0) >> 1) + ((newtime.hours & 0xF0) >> 3) + (newtime.hours & 0xf);
        newtime.day = ((newtime.day & 0xF0) >> 1) + ((newtime.day & 0xF0) >> 3) + (newtime.day & 0xf);
        newtime.month = ((newtime.month & 0xF0) >> 1) + ((newtime.month & 0xF0) >> 3) + (newtime.month & 0xf);
        uint8_t year = ((readCMOSReg(9) & 0xF0) >> 1) + ((readCMOSReg(9) & 0xF0) >> 3) + (readCMOSReg(9) & 0xf);
        uint8_t century = ((readCMOSReg(50) & 0xF0) >> 1) + ((readCMOSReg(50) & 0xF0) >> 3) + (readCMOSReg(50) & 0xf);
        newtime.year = century * 100 + year;
    }
    ksettime(newtime);

    readCMOSReg(0x0C);
    int_exit_slave();
}

__attribute__((interrupt)) void irq2(IntFrame* frame) {
    int_exit_master();
}