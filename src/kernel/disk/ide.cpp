#include "ide.hpp"
#include "../pci/pci.hpp"
#include "../graphics/glyphs.hpp"
#include "../memmgr/paging.hpp"
#include "../io/com.hpp"
#include "../dbg/dbg.hpp"

const dword prdt1start = 0x9500;
const dword prdt2start = 0x9700;
const word  prdtSize   = 0x200;

bool pciMode1 = false;
bool pciMode2 = false;
bool dma = false;

dword ideCon = 0;

PRD* prdt1 = nullptr;
PRD* prdt2 = nullptr;
PRD* prdt1base = nullptr;
PRD* prdt2base = nullptr;
bool prdt1read = false;
bool prdt2read = false;

bool transferring = false;

void cleanPRDT1() {
    while ((dword)prdt1 > prdt1start) {
        *prdt1-- = {0,0,0,0};
    }
    *prdt1 = {0,0,0,0};
    prdt1base = prdt1;
    outl(IDE_PRDT_ADDR_PRIMARY, (dword)prdt1base);
}

void cleanPRDT2() {
    while ((dword)prdt2 > prdt2start) {
        *prdt2-- = {0,0,0,0};
    }
    *prdt2 = {0,0,0,0};
    prdt2base = prdt2;
    outl(IDE_PRDT_ADDR_SECONDARY, (dword)prdt2base);
}

bool initIDE() {
    kdebug("Начата инициализация IDE-контроллера.\nОпределение ключа контроллера... ");
    ideCon = pciFindDevice(0x1, 0x1);
    if (!ideCon) {
        kdebug("провал\nОШИБКА: Контроллер IDE не был обнаружен\n");
        return false;
    }
    kdebug("Ш%dУ%dФ%d\n", ideCon >> 16, (ideCon >> 8) & 0xFF, ideCon & 0xFF);
    byte progIF = pciIdentifyFunc(ideCon) & 0xFF;
    kdebug("Интерфейс программирования контроллера: %x (%b)\n", progIF, progIF);
    if (progIF & 0x02)
        progIF |= 0x01;
    if (progIF & 0x08)
        progIF |= 0x04;
    pciMode1 = progIF & 0x01;
    pciMode2 = progIF & 0x04;
    if (pciMode1) {
        pciConfigWriteBAR(ideCon, 0, 0x1F0);
        pciConfigWriteBAR(ideCon, 1, 0x3F6);
        kdebug("Режим работы 1 канала IDE - нативный PCI\n");
    } else
        kdebug("Режим работы 1 канала IDE - совместимый с ISA\n");
    if (pciMode2) {
        pciConfigWriteBAR(ideCon, 0, 0x170);
        pciConfigWriteBAR(ideCon, 1, 0x376);
        kdebug("Режим работы 2 канала IDE - нативный PCI\n");
    } else
        kdebug("Режим работы 2 канала IDE - совместимый с ISA\n");
    if (progIF & 0x80) {
        kdebug("Доступен режим DMA. Идёт инициализация...\n");
        pciConfigWriteBAR(ideCon, 4, 0x100);

        kdebug("Установлен BAR4 = %x.\n", pciConfigReadBAR(ideCon, 4));
        prdt1 = (PRD*)prdt1start;
        prdt1base = prdt1;
        prdt2 = (PRD*)prdt2start;
        prdt2base = prdt2;
        for (word i = 0; i < prdtSize / sizeof(PRD); i++) {
            prdt1[i] = {0,0,0,0};
        }
        for (word i = 0; i < prdtSize / sizeof(PRD); i++) {
            prdt2[i] = {0,0,0,0};
        }
        if (*(qword*)prdt1 || *(qword*)prdt2)
            kdebug("ВНИМАНИЕ: Зона для PRDT не была очищена\n");
        else 
            kdebug("Успешно созданы зоны для PRDT по адресам\n\tдля 1 канала - %x;\n\tдля 2 канала - %x.\n", (dword)prdt1, (dword)prdt2);
        outb(IDE_COMMAND_PRIMARY, 1);
        outb(IDE_STATUS_PRIMARY, 2);
        outb(IDE_STATUS_PRIMARY, 4);
        outl(IDE_PRDT_ADDR_PRIMARY, (dword)prdt1);
        kdebug("Установлен адрес PRDT1 на %x.\nСтатус 1 канала - \t%b\nКоманда 1 каналу - \t%b\n", inl(IDE_PRDT_ADDR_PRIMARY), inb(IDE_STATUS_PRIMARY), inb(IDE_COMMAND_PRIMARY));

        outb(IDE_COMMAND_SECONDARY, 1);
        outb(IDE_STATUS_SECONDARY, 2);
        outb(IDE_STATUS_SECONDARY, 4);
        outl(IDE_PRDT_ADDR_SECONDARY, (dword)prdt2);
        kdebug("Установлен адрес PRDT2 на %x.\nСтатус 2 канала - \t%b\nКоманда 2 каналу - \t%b\n", inl(IDE_PRDT_ADDR_SECONDARY), inb(IDE_STATUS_SECONDARY), inb(IDE_COMMAND_SECONDARY));

        createPage(prdt1start, prdt1start);
        createPage(prdt2start, prdt2start);

        dma = true;
    }

    outb(ATA_DRIVE_CONTROL_PRIMARY,   0x04);
    io_wait();
    io_wait();
    outb(ATA_DRIVE_CONTROL_PRIMARY,   0x00);

    outb(ATA_DRIVE_CONTROL_SECONDARY, 0x04);
    io_wait();
    io_wait();
    outb(ATA_DRIVE_CONTROL_SECONDARY, 0x00);

    kdebug("Инициализация контроллера IDE завершена успешно.\n\n");
    return true;
}

void readSectorsATA(dword startLBA, byte sectorsCount, byte driveNo, byte *out) {
    transferring = true;
    // kdebug("Получена команда на считывание %d секторов с LBA %d с диска %d.\n", sectorsCount, startLBA, driveNo);
    // kdebug("Начата подготовка к исполнению команды.\n");
    bool secondary = driveNo & 2;
    bool slave = driveNo & 1;
    if (!secondary && dma) {
        while (!prdt1read && prdt1 > (PRD*)prdt1start) {io_wait();}
        prdt1read = true;
        PRD prd = {getPhysAddr((dword)out), (word)(sectorsCount * 512), 0, 0x80};
        // kdebug("Собрана структура PRD:\n\tФиз. адрес передачи - %x\n\tЧисло байтов для передачи - %d\n\tВерхний байт - %x\n", prd.base, prd.count, prd.msb);
        if (prdt1 == (PRD*)prdt1start) {
            *prdt1 = prd;
            prdt1 ++;
        } else {
            (--prdt1)->msb = 0;
            *(++prdt1) = prd;
            prdt1++;
        }
        // kdebug("Теперь PRDT1 выглядит так:\n");
        // byte i = 0;
        // PRD* dbgPtr = (PRD*)prdt1start;
        // do {
        //     kdebug("Вхождение %d:\n", i+1);
        //     kdebug("\tФиз. адрес: %x\n", dbgPtr->base);
        //     kdebug("\tЧисло байтов для считывания: %d\n", dbgPtr->count);
        //     kdebug("\tПоследний ли? ");
        //     kdebug(dbgPtr->msb ? "Да\n" : "Нет\n");
        //     if (dbgPtr->msb)
        //         break;
        //     dbgPtr ++;
        //     i ++;
        // } while (true);

        outb(IDE_STATUS_PRIMARY, 2);
        outb(IDE_STATUS_PRIMARY, 4);

        // kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_PRIMARY, slave ? 0xF0 : 0xE0);
        // kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_PRIMARY));
        outb(ATA_SECTOR_COUNT_PRIMARY, sectorsCount);
        // kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_PRIMARY));
        outb(ATA_LBA_LOW_PRIMARY, startLBA & 0xFF);
        // kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_PRIMARY));
        outb(ATA_LBA_MID_PRIMARY, (startLBA >> 8) & 0xFF);
        // kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_PRIMARY));
        outb(ATA_LBA_HIGH_PRIMARY, (startLBA >> 16) & 0xFF);
        // kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_PRIMARY));
        if (prdt1 == (PRD*)(prdt1start + sizeof(PRD))) {
            // kdebug("PRDT1 пуста. Контроллер переводится в режим чтения.\n");
            outb(IDE_COMMAND_PRIMARY, 0);
            outb(IDE_COMMAND_PRIMARY, 9);
            // if (inb(IDE_COMMAND_PRIMARY) != 9)
            //     kdebug("ВНИМАНИЕ: Перевод провален\n");
            // else
            //     kdebug("Перевод в режим чтения успешно совершён.\n");
        }
        inb(ATA_STATUS_PRIMARY);
        outb(ATA_COMMAND_PRIMARY, 0xC8);
        // kdebug("Выслана команда READ DMA.\n");
        // if (inb(IDE_COMMAND_PRIMARY) & 1)
            // kdebug("Режим DMA успешно активирован.\n");
        // else
            // kdebug("ВНИМАНИЕ: Режим DMA не был активирован или передача уже завершилась\n");
    }
    else if (secondary && dma) {
        while (!prdt2read && prdt2 > (PRD*)prdt2start) {io_wait();}
        prdt2read = true;
        PRD prd = {getPhysAddr((dword)out), (word)(sectorsCount * 512), 0, 0x80};
        // kdebug("Собрана структура PRD:\n\tФиз. адрес передачи - %x\n\tЧисло байтов для передачи - %d\n\tВерхний байт - %x\n", prd.base, prd.count, prd.msb);
        if (prdt2 == (PRD*)prdt2start) {
            *prdt2 = prd;
            prdt2 ++;
        } else {
            (--prdt2)->msb = 0;
            *(++prdt2) = prd;
            prdt2++;
        }
        // kdebug("Теперь PRDT2 выглядит так:\n");
        // byte i = 0;
        // PRD* dbgPtr = (PRD*)prdt2start;
        // do {
        // kdebug("Вхождение %d:\n", i+1);
        //     kdebug("\tФиз. адрес: %x\n", dbgPtr->base);
        //     kdebug("\tЧисло байтов для считывания: %d\n", dbgPtr->count);
        //     kdebug("\tПоследний ли? ");
        //     kdebug(dbgPtr->msb ? "Да\n" : "Нет\n");
        //     if (dbgPtr->msb)
        //         break;
        //     dbgPtr ++;
        //     i ++;
        // } while (true);
        // kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_SECONDARY, slave ? 0xF0 : 0xE0);
        // kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_SECONDARY));
        outb(ATA_SECTOR_COUNT_SECONDARY, sectorsCount);
        // kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_SECONDARY));
        outb(ATA_LBA_LOW_SECONDARY, startLBA & 0xFF);
        // kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_SECONDARY));
        outb(ATA_LBA_MID_SECONDARY, (startLBA >> 8) & 0xFF);
        // kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_SECONDARY));
        outb(ATA_LBA_HIGH_SECONDARY, (startLBA >> 16) & 0xFF);
        // kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_SECONDARY));
        if (prdt2 == (PRD*)(prdt2start + sizeof(PRD))) {
            // kdebug("PRDT2 пуста. Контроллер переводится в режим чтения.\n");
            outb(IDE_COMMAND_SECONDARY, 9);
            outb(IDE_COMMAND_SECONDARY, 9);
            // if (inb(IDE_COMMAND_SECONDARY) != 9)
            //     kdebug("ВНИМАНИЕ: Перевод провален\n");
            // else
            //     kdebug("Перевод в режим чтения успешно совершён.\n");
        }
        outb(ATA_COMMAND_SECONDARY, 0xC8);
        // kdebug("Выслана команда READ DMA.\n");
        // if (inb(IDE_COMMAND_SECONDARY) & 1)
            // kdebug("Режим DMA успешно активирован.\n");
        // else
            // kdebug("ВНИМАНИЕ: Режим DMA не был активирован или передача уже завершилась\n");
    }
    while (transferring) {io_wait();}
    // kdebug("Чтение с диска успешно завершено.\n");
}

void writeSectorsATA(dword startLBA, byte sectorsCount, byte driveNo, byte *out) {
    transferring = true;
    // kdebug("Получена команда на запись %d секторов с LBA %d на диск %d.\n", sectorsCount, startLBA, driveNo);
    // kdebug("Начинается подготовка к записи на диск.\n");
    bool slave = driveNo & 1;
    bool secondary = driveNo & 2;
    if (!secondary && dma) {
        while (prdt1read && prdt1 > (PRD*)prdt1start) {if (!prdt1read) break;}
        prdt1read = false;
        PRD prd = {getPhysAddr((dword)out), (word)(sectorsCount * 512), 0, 0x80};
        // kdebug("Собрана структура PRD:\n\tФиз. адрес передачи: %x\n\tЧисло байтов для передачи: %d\n\tВерхний байт: %x\n", prd.base, prd.count, prd.msb);
        if (prdt1 == (PRD*)prdt1start) {
            *prdt1 = prd;
            prdt1 ++;
        } else {
            (--prdt1)->msb = 0;
            *(++prdt1) = prd;
            prdt1++;
        }
        // kdebug("Теперь PRDT1 выглядит так:\n");
        // byte i = 0;
        // PRD* dbgPRDPtr = (PRD*)prdt1start;
        // while (true) {
        //     kdebug("Вхождение %d:\n", i+1);
        //     kdebug("\tФиз. адрес: %x\n", dbgPRDPtr->base);
        //     kdebug("\tЧисло байтов для записи: %d\n", dbgPRDPtr->count);
        //     kdebug("\tПоследний ли? ");
        //     kdebug(dbgPRDPtr->msb ? "Да\n" : "Нет\n");
        //     if (dbgPRDPtr->msb)
        //         break;
        //     if (dbgPRDPtr->base == 0 && dbgPRDPtr->count == 0 && dbgPRDPtr->msb == 0) {
        //         kdebug("Пустое вхождение.\n");
        //         break;
        //     }
        //     dbgPRDPtr ++;
        //     i ++;
        // }
        // kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_PRIMARY, slave ? 0xF0 : 0xE0);
        // kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_PRIMARY));
        outb(ATA_SECTOR_COUNT_PRIMARY, sectorsCount);
        // kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_PRIMARY));
        outb(ATA_LBA_LOW_PRIMARY, startLBA & 0xFF);
        // kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_PRIMARY));
        outb(ATA_LBA_MID_PRIMARY, (startLBA >> 8) & 0xFF);
        // kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_PRIMARY));
        outb(ATA_LBA_HIGH_PRIMARY, (startLBA >> 16) & 0xFF);
        // kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_PRIMARY));
        if (prdt1 == (PRD*)(prdt1start + sizeof(PRD))) {
            // kdebug("PRDT1 пуста. Контроллер переводится в режим записи.\n");
            outb(IDE_COMMAND_PRIMARY, 0);
            outb(IDE_COMMAND_PRIMARY, 1);
        }
        outb(ATA_COMMAND_PRIMARY, 0xCA);
        // kdebug("Выслана команда WRITE DMA.\n");
        // if (inb(IDE_COMMAND_PRIMARY) & 1)
        //     kdebug("Режим DMA успешно активирован.\n");
        // else
        //     kdebug("ВНИМАНИЕ: Режим DMA не активирован\nОжидалось значение 1, получено %d\n", inb(IDE_COMMAND_PRIMARY));

    } else if (dma && secondary) {
        while (prdt2read && prdt2 > (PRD*)prdt1start) {if (!prdt2read) break;}
        prdt2read = false;
        PRD prd = {getPhysAddr((dword)out), (word)(sectorsCount * 512), 0, 0x80};
        // kdebug("Собрана структура PRD:\n\tФиз. адрес передачи: %x\n\tЧисло байтов для передачи: %d\n\tВерхний байт: %x\n", prd.base, prd.count, prd.msb);
        if (prdt2 == (PRD*)prdt2start) {
            *prdt2 = prd;
            prdt2 ++;
        } else {
            (--prdt2)->msb = 0;
            *(++prdt2) = prd;
            prdt2++;
        }
        // kdebug("Теперь PRDT2 выглядит так:\n");
        // byte i = 0;
        // PRD* dbgPRDPtr = (PRD*)prdt2start;
        // while (true) {
        //     kdebug("Вхождение %d:\n", i+1);
        //     kdebug("\tФиз. адрес: %x\n", dbgPRDPtr->base);
        //     kdebug("\tЧисло байтов для записи: %d\n", dbgPRDPtr->count);
        //     kdebug("\tПоследний ли? ");
        //     kdebug(dbgPRDPtr->msb ? "Да\n" : "Нет\n");
        //     if (dbgPRDPtr->msb)
        //         break;
        //     if (dbgPRDPtr->base == 0 && dbgPRDPtr->count == 0 && dbgPRDPtr->msb == 0) {
        //         kdebug("Пустое вхождение.\n");
        //         break;
        //     }
        //     dbgPRDPtr ++;
        //     i ++;
        // }
        // kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_SECONDARY, slave ? 0xF0 : 0xE0);
        // kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_SECONDARY));
        outb(ATA_SECTOR_COUNT_SECONDARY, sectorsCount);
        // kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_SECONDARY));
        outb(ATA_LBA_LOW_SECONDARY, startLBA & 0xFF);
        // kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_SECONDARY));
        outb(ATA_LBA_MID_SECONDARY, (startLBA >> 8) & 0xFF);
        // kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_SECONDARY));
        outb(ATA_LBA_HIGH_SECONDARY, (startLBA >> 16) & 0xFF);
        // kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_SECONDARY));
        if (prdt2 == (PRD*)(prdt2start + sizeof(PRD))) {
            // kdebug("prdt2 пуста. Контроллер переводится в режим записи.\n");
            outb(IDE_COMMAND_SECONDARY, 0);
            outb(IDE_COMMAND_SECONDARY, 1);
        }
        outb(ATA_COMMAND_SECONDARY, 0xCA);
        // kdebug("Выслана команда WRITE DMA.\n");
        // if (inb(IDE_COMMAND_SECONDARY) & 1)
            // kdebug("Режим DMA успешно активирован.\n");
        // else
            // kdebug("ВНИМАНИЕ: Режим DMA не активирован\nОжидалось значение 1, получено %d\n", inb(IDE_COMMAND_SECONDARY));
    }
    while (transferring) {io_wait();}
    // kdebug("Запись на диск успешно завершена.\n");
}