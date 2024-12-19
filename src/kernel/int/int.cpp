#include "int.hpp"
#include "pic.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"
#include "../memmgr/paging.hpp"
#include "../keyboard/keyboard.hpp"
#include "../disk/ide.hpp"
#include "../pci/pci.hpp"
#include "../timing/cmos.hpp"
#include "../timing/time.hpp"
#include "../dbg/dbg.hpp"
#include "../util/util.hpp"

IDT_Register *idtr;
const byte irqOffset = 0x20;

void initInts() {
    idtr = (IDT_Register*)0x1510;
    idtr->size = 399;
    idtr->base = (byte*)0x10000;
    for (int i = 0; i < idtr->size; i++)
        *(idtr->base + i) = 0;

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

    encode_idt_entry(irq0,  irqOffset + 0x0);
    encode_idt_entry(irq1,  irqOffset + 0x1);
    encode_idt_entry(irq2,  irqOffset + 0x2);
    encode_idt_entry(irq3,  irqOffset + 0x3);
    encode_idt_entry(irq4,  irqOffset + 0x4);
    encode_idt_entry(irq7,  irqOffset + 0x7);
    encode_idt_entry(irq8,  irqOffset + 0x8);
    encode_idt_entry(irq14, irqOffset + 0xE);
    encode_idt_entry(irq15, irqOffset + 0xF);

    lidt(*idtr);
    enableInts();
}

void encode_idt_entry(void (*handlePtr)(IntFrame*), byte intNum) {
    word *entryPtr = (word*)(idtr->base) + 4 * intNum;
    word offset1 = (dword)handlePtr & 0xFFFF;
    *entryPtr++ = offset1;
    word selector = 0x8;
    *entryPtr++ = selector;
    word flags = 0x8E00;
    *entryPtr++ = flags;
    word offset2 = (dword)handlePtr >> 16;
    *entryPtr = offset2;
}

__attribute__((interrupt)) void zero_divide_err(IntFrame* frame) {
    kerror("\nОШИБКА: Деление на ноль\n");
    traceStack();
}

__attribute__((interrupt)) void overflow_err(IntFrame* frame) {
    kerror("\nОШИБКА: Переполнение\n");
    traceStack();
}

__attribute__((interrupt)) void bound_err(IntFrame* frame) {
    kerror("\nОШИБКА: Индекс превышает длину массива\n");
    traceStack();
}

__attribute__((interrupt)) void invalid_opcode_err(IntFrame* frame) {
    kerror("\nОШИБКА: Невозможная инструкция\n");
    traceStack();
}

__attribute__((interrupt)) void dev_unavailable_err(IntFrame* frame) {
    kerror("\nОШИБКА: Устройство недоступно\n");
    traceStack();
}

__attribute__((interrupt)) void double_fault(IntFrame* frame) {
    kerror("\nОШИБКА: Двойной сбой\n");
    kwarn("Возврат невозможен!\n");
    traceStack();
    __asm__ ("hlt");
}

__attribute__((interrupt)) void invalid_task_switch_err(IntFrame* frame) {
    kerror("\nОШИБКА: Неправильный переход на процесс\n");
    traceStack();
}

__attribute__((interrupt)) void seg_not_present_err(IntFrame* frame) {
    kerror("\nОШИБКА: Сегмент ненастоящий\n");
    traceStack();
}

__attribute__((interrupt)) void stack_seg_fault(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой сегмента стека\n");
    traceStack();
}

__attribute__((interrupt)) void general_prot_fault(IntFrame* frame) {
    dword errcode = (dword)frame;
    kerror("\nОШИБКА: Общий сбой защиты\n");
    kerror("Код ошибки: %b\n", errcode);
    if (errcode & 1)
        kerror("Ошибка вызвана не процессором\n");
    else
        kerror("Ошибка вызвана процессором\n");
    switch ((errcode & 0b110) >> 1) {
        case 0b00:
            kerror("Сегмент ошибки в GDT\n");
            break;
        case 0b01:
            kerror("Сегмент ошибки в IDT\n");
            break;
        case 0b10:
            kerror("Сегмент ошибки в LDT\n");
            break;
        case 0b11:
            kerror("Сегмент ошибки в IDT\n");
            break;
    }
    kerror("Номер сегмента: %d\n", (errcode >> 3) & 0xFFFF);
    traceStack();
    enableInts();
    while (true) {io_wait();}
}

__attribute__((interrupt)) void page_fault(IntFrame* frame) {
    dword faultAddr;
    __asm__ ("mov %%cr2, %%eax; mov %%eax, %d0" : "=m"(faultAddr) : );
    kerror("ОШИБКА: Страничный сбой\n");
    kerror("Адрес сбоя: %x\n", faultAddr);
    kerror("Код ошибки: %b\n", (dword)frame);
    traceStack();
}

__attribute__((interrupt)) void float_exception(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой математического сопроцессора\n");
    traceStack();
}

__attribute__((interrupt)) void align_check(IntFrame* frame) {
    kerror("\nОШИБКА: Сбой ровнения\n");
    traceStack();
}

__attribute__((interrupt)) void irq0(IntFrame* frame) {
    updateCursor();
    int_exit_master();
}

__attribute__((interrupt)) void irq1(IntFrame* frame) {
    disableInts();
    while (!(inb(0x64) & 1)) {io_wait();}
    byte scancode = inb(0x60);
    if (scancode == KB_ACK) {
        byte ackdCmd = *(byte*)KB_CMD_BUF_BASE;
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
        *kbBufPtr = inb(0x60);
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
    // kdebug("\nВызвано IRQ 14.\nПрерывание вызвал ");
    byte ideStatus = inb(IDE_STATUS_PRIMARY);
    byte ataStatus = inb(ATA_STATUS_PRIMARY);
    if (!(ideStatus & 4)) {
        // kdebug("не диск.\nВНИМАНИЕ: Прерывание вызвано не диском, обработка прервана\n\n");
        int_exit_slave();
        return;
    }
    // kdebug("диск.\n");
    // kdebug("Статус 1 канала: %b\n", ideStatus);
    // kdebug("Статус контроллера: %b\n", pciConfigReadW(ideCon >> 16, ideCon >> 8, ideCon, 6));
    // kdebug("Статус диска: %b\n", ataStatus);
    if ((ideStatus & 2) || (ataStatus & ATA_STATUS_ERROR)) {
        kdebug("ВНИМАНИЕ: Была зафиксирована ошибка диска\n");
        kdebug("Статус диска: %b\n", inb(ATA_ERROR_PRIMARY));
        dword errSector = (inb(ATA_LBA_HIGH_PRIMARY) << 16) | (inb(ATA_LBA_MID_PRIMARY) << 8) | inb(ATA_LBA_LOW_PRIMARY);
        kdebug("Проблемный сектор: %d\n", errSector);
        outb(IDE_STATUS_PRIMARY, 2);
    }
    byte cmd = inb(IDE_COMMAND_PRIMARY);
    // kdebug("Команда 1 канала: %b\n", cmd);
    // kdebug("Команда была на ");
    bool read = cmd & 8;
    // kdebug(read ? "чтение.\n" : "запись.\n");
    // PRD donePrd = *prdt1base++;
    prdt1base++;
    // kdebug("Обработанный PRD:\n\tФиз. адрес данных: %x\n\tРазмер блока данных: %d Б\n\tПоследний ли? ", donePrd.base, donePrd.count, donePrd.msb);
    // kdebug((donePrd.msb & 0x80) ? "Да\n" : "Нет\n");
    if (prdt1base == prdt1) {
        // kdebug("Обработка PRDT завершена. Производится очистка.\n");
        cleanPRDT1();
        prdt1read = read;
        outb(IDE_COMMAND_PRIMARY, 0);
    } else {
        // kdebug("Обработка PRDT продолжается.\n");
    }
    outb(IDE_STATUS_PRIMARY, 4);
    // kdebug("Новый статус: %b.\n", inb(IDE_STATUS_PRIMARY));
    // kdebug("Обработка прерывания завершена успешно.\n\n");
    transferring = false;
    int_exit_slave();
}

__attribute__((interrupt)) void irq15(IntFrame* frame) {
    // kdebug("\nВызвано IRQ 15.\nПрерывание вызвал ");
    byte ideStatus = inb(IDE_STATUS_SECONDARY);
    if (!(ideStatus & 4)) {
        // kdebug("не диск.\nВНИМАНИЕ: Прерывание вызвано не диском, обработка прервана\n\n");
        int_exit_slave();
        return;
    }
    // kdebug("диск.\n");
    // kdebug("Статус IDE-контроллера: %b\n", ideStatus);
    byte cmd = inb(IDE_COMMAND_SECONDARY);
    // kdebug("Команда была на ");
    bool read = cmd & 8;
    // kdebug(read ? "чтение.\n" : "запись.");
    // PRD donePrd = *prdt2base++;
    prdt2base++;
    // kdebug("Обработанный PRD:\n\tФиз. адрес данных: %x\n\tРазмер блока данных: %d Б\n\tПоследний ли? ", donePrd.base, donePrd.count, donePrd.msb);
    // kdebug((donePrd.msb & 0x80) ? "Да\n" : "Нет\n");
    if (prdt2base == prdt2) {
        // kdebug("Обработка PRDT завершена. Производится очистка.\n");
        cleanPRDT2();
        prdt2read = read;
        outb(IDE_COMMAND_SECONDARY, 0);
    }
    outb(IDE_STATUS_SECONDARY, 4);
    // kdebug("Новый статус: %b.\n", inb(IDE_STATUS_SECONDARY));
    // kdebug("Обработка прерывания завершена успешно.\n\n");
    transferring = false;
    int_exit_slave();
}

__attribute__((interrupt)) void irq3(IntFrame* frame) {
    disableInts();
    word ioPort = getIOPort(2);
    byte iir = inb(ioPort + 2);
    byte intState = iir & 6;

    if (intState == COM_IIR_RECEIVE_LINE_STATUS) {
        byte lsr = inb(ioPort + 5);
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
        byte msr = inb(ioPort + 6);
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
    word ioPort = getIOPort(1);
    byte iir = inb(ioPort + 2);
    byte intState = iir & 6;

    if (intState == COM_IIR_RECEIVE_LINE_STATUS) {
        byte lsr = inb(ioPort + 5);
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
    byte cmosStatusB = readCMOSReg(0x0B);
    cmosStatusB |= 16;
    writeCMOSReg(0x0B, cmosStatusB);

    Time newtime;
    newtime.seconds = readCMOSReg(0);
    newtime.minutes = readCMOSReg(2);
    byte hours = readCMOSReg(4);
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
        byte year = ((readCMOSReg(9) & 0xF0) >> 1) + ((readCMOSReg(9) & 0xF0) >> 3) + (readCMOSReg(9) & 0xf);
        byte century = ((readCMOSReg(50) & 0xF0) >> 1) + ((readCMOSReg(50) & 0xF0) >> 3) + (readCMOSReg(50) & 0xf);
        newtime.year = century * 100 + year;
    }
    ksettime(newtime);

    readCMOSReg(0x0C);
    int_exit_slave();
}

__attribute__((interrupt)) void irq2(IntFrame* frame) {
    int_exit_master();
}

__attribute__((interrupt)) void irq7(IntFrame* frame) {
    if (!(getISR() & 0x80))
        return;

    int_exit_master();
}