#include "ide.hpp"
#ifndef PCI_CONFIG_ADDRESS
#include "../pci/pci.hpp"
#endif
#ifndef GRAPHICS_SIG
#include "../graphics/glyphs.hpp"
#endif
#ifndef PAGING_BASE
#include "../memmgr/paging.hpp"
#endif
#ifndef COM_MAX_BAUD_RATE
#include "../io/com.hpp"
#endif

bool pciMode1 = false;
bool pciMode2 = false;
bool dma = false;

uint32_t ideCon = 0;

PRD* prdt1 = nullptr;
PRD* prdt2 = nullptr;
PRD* prdt1base = nullptr;
PRD* prdt2base = nullptr;
bool prdt1read = false;
bool prdt2read = false;

bool transferring = false;

uint16_t *dbgPtr = (uint16_t*)0x100300;

void cleanPRDT1() {
    while ((uint32_t)prdt1 > 0x9500) {
        *prdt1-- = {0,0,0,0};
    }
    *prdt1 = {0,0,0,0};
    prdt1base = prdt1;
    outl(IDE_PRDT_ADDR_PRIMARY, (uint32_t)prdt1base);
}

void cleanPRDT2() {
    while ((uint32_t)prdt2 > 0x9700) {
        *prdt2-- = {0,0,0,0};
    }
    *prdt2 = {0,0,0,0};
    prdt2base = prdt2;
    outl(IDE_PRDT_ADDR_SECONDARY, (uint32_t)prdt2base);
}

// void stopDMAPrim() {
//     uint8_t cmd = inb(IDE_COMMAND_PRIMARY);
//     cmd &= 0xFE;
//     outb(IDE_COMMAND_PRIMARY, cmd);
// }

// void startDMAPrim() {
//     uint8_t cmd = inb(IDE_COMMAND_PRIMARY);
//     cmd |= 1;
//     outb(IDE_COMMAND_PRIMARY, cmd);
// }

bool initIDE() {
    kdebug("Начата инициализация IDE-контроллера.\nОпределение ключа контроллера... ");
    ideCon = pciFindDevice(0x1, 0x1);
    if (!ideCon) {
        kdebug("провал\nОШИБКА: Контроллер IDE не был обнаружен\n");
        return false;
    }
    kdebug("Ш%dУ%dФ%d\n", ideCon >> 16, (ideCon >> 8) & 0xFF, ideCon & 0xFF);
    uint8_t progIF = pciIdentifyFunc(ideCon) & 0xFF;
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
        prdt1 = (PRD*)0x9500;
        prdt1base = prdt1;
        prdt2 = (PRD*)0x9700;
        prdt2base = prdt2;
        for (uint16_t i = 0; i < 0x100; i++) {
            *(uint32_t*)(0x9500 + i * 4) = 0;
        }
        if (*(uint64_t*)prdt1 || *(uint64_t*)prdt2)
            kdebug("ВНИМАНИЕ: Зона для PRDT не была очищена\n");
        else 
            kdebug("Успешно созданы зоны для PRDT по адресам\n\tдля 1 канала - %x;\n\tдля 2 канала - %x.\n", (uint32_t)prdt1, (uint32_t)prdt2);
        outb(IDE_COMMAND_PRIMARY, 1);
        outb(IDE_STATUS_PRIMARY, 2);
        outb(IDE_STATUS_PRIMARY, 4);
        outl(IDE_PRDT_ADDR_PRIMARY, (uint32_t)prdt1);
        kdebug("Установлен адрес PRDT1 на %x.\nСтатус 1 канала - \t%b\nКоманда 1 каналу - \t%b\n", inl(IDE_PRDT_ADDR_PRIMARY), inb(IDE_STATUS_PRIMARY), inb(IDE_COMMAND_PRIMARY));

        outb(IDE_COMMAND_SECONDARY, 1);
        outb(IDE_STATUS_SECONDARY, 2);
        outb(IDE_STATUS_SECONDARY, 4);
        outl(IDE_PRDT_ADDR_SECONDARY, (uint32_t)prdt2);
        kdebug("Установлен адрес PRDT2 на %x.\nСтатус 2 канала - \t%b\nКоманда 2 каналу - \t%b\n", inl(IDE_PRDT_ADDR_SECONDARY), inb(IDE_STATUS_SECONDARY), inb(IDE_COMMAND_SECONDARY));

        dma = true;
    }
    outb(ATA_DRIVE_CONTROL_PRIMARY, 4);
    io_wait();
    outb(ATA_DRIVE_CONTROL_PRIMARY, 0);
    kdebug("1 канал ATA сброшен.\n");

    outb(ATA_DRIVE_CONTROL_SECONDARY, 4);
    io_wait();
    outb(ATA_DRIVE_CONTROL_SECONDARY, 0);
    kdebug("2 канал ATA сброшен.\n");

    kdebug("Инициализация контроллера IDE завершена успешно.\n\n");
    return true;
}

void readSectorsATA(uint32_t startLBA, uint8_t sectorsCount, uint8_t driveNo, uint8_t *out) {
    transferring = true;
    kdebug("Получена команда на считывание %d секторов с LBA %d с диска %d.\n", sectorsCount, startLBA, driveNo);
    kdebug("Начата подготовка к исполнению команды.\n");
    bool secondary = driveNo & 2;
    bool slave = driveNo & 1;
    if (!secondary && dma) {
        while (!prdt1read && prdt1 > (PRD*)0x9500) {if (prdt1read) break;}
        prdt1read = true;
        PRD prd = {getPhysAddr((uint32_t)out), (uint16_t)(sectorsCount * 512), 0, 0x80};
        kdebug("Собрана структура PRD:\n\tФиз. адрес передачи - %x\n\tЧисло байтов для передачи - %d\n\tВерхний байт - %x\n", prd.base, prd.count, prd.msb);
        if (prdt1 == (PRD*)0x9500) {
            *prdt1 = prd;
            prdt1 ++;
        } else {
            (--prdt1)->msb = 0;
            *(++prdt1) = prd;
            prdt1++;
        }
        kdebug("Теперь PRDT1 выглядит так:\n");
        uint8_t i = 0;
        PRD* dbgPtr = (PRD*)0x9500;
        do {
            kdebug("Вхождение %d:\n", i+1);
            kdebug("\tФиз. адрес: %x\n", dbgPtr->base);
            kdebug("\tЧисло байтов для считывания: %d\n", dbgPtr->count);
            kdebug("\tПоследний ли? ");
            kdebug(dbgPtr->msb ? "Да\n" : "Нет\n");
            if (dbgPtr->msb)
                break;
            dbgPtr ++;
            i ++;
        } while (true);
        kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_PRIMARY, slave ? 0xF0 : 0xE0);
        kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_PRIMARY));
        outb(ATA_SECTOR_COUNT_PRIMARY, sectorsCount);
        kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_PRIMARY));
        outb(ATA_LBA_LOW_PRIMARY, startLBA & 0xFF);
        kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_PRIMARY));
        outb(ATA_LBA_MID_PRIMARY, (startLBA >> 8) & 0xFF);
        kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_PRIMARY));
        outb(ATA_LBA_HIGH_PRIMARY, (startLBA >> 16) & 0xFF);
        kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_PRIMARY));
        kdebug("%x\n", (uint32_t)prdt1);
        if (prdt1 == (PRD*)0x9508) {
            kdebug("PRDT1 пуста. Контроллер переводится в режим чтения.\n");
            outb(IDE_COMMAND_PRIMARY, 0);
            outb(IDE_COMMAND_PRIMARY, 9);
            if (inb(IDE_COMMAND_PRIMARY) != 9)
                kdebug("ВНИМАНИЕ: Перевод провален\n");
            else
                kdebug("Перевод в режим чтения успешно совершён.\n");
        }
        outb(ATA_COMMAND_PRIMARY, 0xC8);
        kdebug("Выслана команда READ DMA.\n");
        if (inb(IDE_COMMAND_PRIMARY) & 1)
            kdebug("Режим DMA успешно активирован.\n");
        else
            kdebug("ВНИМАНИЕ: Режим DMA не был активирован или передача уже завершилась\n");
    }
    else if (secondary && dma) {
        while (!prdt2read && prdt2 > (PRD*)0x9700) {if (prdt2read) break;}
        prdt2read = true;
        PRD prd = {getPhysAddr((uint32_t)out), (uint16_t)(sectorsCount * 512), 0, 0x80};
        kdebug("Собрана структура PRD:\n\tФиз. адрес передачи - %x\n\tЧисло байтов для передачи - %d\n\tВерхний байт - %x\n", prd.base, prd.count, prd.msb);
        if (prdt2 == (PRD*)0x9700) {
            *prdt2 = prd;
            prdt2 ++;
        } else {
            (--prdt2)->msb = 0;
            *(++prdt2) = prd;
            prdt2++;
        }
        kdebug("Теперь PRDT2 выглядит так:\n");
        uint8_t i = 0;
        PRD* dbgPtr = (PRD*)0x9700;
        do {
            kdebug("Вхождение %d:\n", i+1);
            kdebug("\tФиз. адрес: %x\n", dbgPtr->base);
            kdebug("\tЧисло байтов для считывания: %d\n", dbgPtr->count);
            kdebug("\tПоследний ли? ");
            kdebug(dbgPtr->msb ? "Да\n" : "Нет\n");
            if (dbgPtr->msb)
                break;
            dbgPtr ++;
            i ++;
        } while (true);
        kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_SECONDARY, slave ? 0xF0 : 0xE0);
        kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_SECONDARY));
        outb(ATA_SECTOR_COUNT_SECONDARY, sectorsCount);
        kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_SECONDARY));
        outb(ATA_LBA_LOW_SECONDARY, startLBA & 0xFF);
        kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_SECONDARY));
        outb(ATA_LBA_MID_SECONDARY, (startLBA >> 8) & 0xFF);
        kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_SECONDARY));
        outb(ATA_LBA_HIGH_SECONDARY, (startLBA >> 16) & 0xFF);
        kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_SECONDARY));
        if (prdt2 == (PRD*)0x9708) {
            kdebug("PRDT2 пуста. Контроллер переводится в режим чтения.\n");
            outb(IDE_COMMAND_SECONDARY, 9);
            outb(IDE_COMMAND_SECONDARY, 9);
            if (inb(IDE_COMMAND_SECONDARY) != 9)
                kdebug("ВНИМАНИЕ: Перевод провален\n");
            else
                kdebug("Перевод в режим чтения успешно совершён.\n");
        }
        outb(ATA_COMMAND_SECONDARY, 0xC8);
        kdebug("Выслана команда READ DMA.\n");
        if (inb(IDE_COMMAND_SECONDARY) & 1)
            kdebug("Режим DMA успешно активирован.\n");
        else
            kdebug("ВНИМАНИЕ: Режим DMA не был активирован или передача уже завершилась\n");
    }
    while (transferring) {if (!transferring) break;}
    kdebug("Чтение с диска успешно завершено.\n\n");
}

void writeSectorsATA(uint32_t startLBA, uint8_t sectorsCount, uint8_t driveNo, uint8_t *out) {
    transferring = true;
    kdebug("Получена команда на запись %d секторов с LBA %d на диск %d.\n", sectorsCount, startLBA, driveNo);
    kdebug("Начинается подготовка к записи на диск.\n");
    bool slave = driveNo & 1;
    bool secondary = driveNo & 2;
    if (!secondary && dma) {
        while (prdt1read && prdt1 > (PRD*)0x9500) {if (!prdt1read) break;}
        prdt1read = false;
        PRD prd = {getPhysAddr((uint32_t)out), (uint16_t)(sectorsCount * 512), 0, 0x80};
        kdebug("Собрана структура PRD:\n\tФиз. адрес передачи: %x\n\tЧисло байтов для передачи: %d\n\tВерхний байт: %x\n", prd.base, prd.count, prd.msb);
        if (prdt1 == (PRD*)0x9500) {
            *prdt1 = prd;
            prdt1 ++;
        } else {
            (--prdt1)->msb = 0;
            *(++prdt1) = prd;
            prdt1++;
        }
        kdebug("Теперь PRDT1 выглядит так:\n");
        uint8_t i = 0;
        PRD* dbgPRDPtr = (PRD*)0x9500;
        while (true) {
            kdebug("Вхождение %d:\n", i+1);
            kdebug("\tФиз. адрес: %x\n", dbgPRDPtr->base);
            kdebug("\tЧисло байтов для записи: %d\n", dbgPRDPtr->count);
            kdebug("\tПоследний ли? ");
            kdebug(dbgPRDPtr->msb ? "Да\n" : "Нет\n");
            if (dbgPRDPtr->msb)
                break;
            if (dbgPRDPtr->base == 0 && dbgPRDPtr->count == 0 && dbgPRDPtr->msb == 0) {
                kdebug("Пустое вхождение.\n");
                break;
            }
            dbgPRDPtr ++;
            i ++;
        }
        kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_PRIMARY, slave ? 0xF0 : 0xE0);
        kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_PRIMARY));
        outb(ATA_SECTOR_COUNT_PRIMARY, sectorsCount);
        kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_PRIMARY));
        outb(ATA_LBA_LOW_PRIMARY, startLBA & 0xFF);
        kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_PRIMARY));
        outb(ATA_LBA_MID_PRIMARY, (startLBA >> 8) & 0xFF);
        kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_PRIMARY));
        outb(ATA_LBA_HIGH_PRIMARY, (startLBA >> 16) & 0xFF);
        kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_PRIMARY));
        if (prdt1 == (PRD*)0x9508) {
            kdebug("PRDT1 пуста. Контроллер переводится в режим записи.\n");
            outb(IDE_COMMAND_PRIMARY, 0);
            outb(IDE_COMMAND_PRIMARY, 1);
        }
        outb(ATA_COMMAND_PRIMARY, 0xCA);
        kdebug("Выслана команда WRITE DMA.\n");
        if (inb(IDE_COMMAND_PRIMARY) & 1)
            kdebug("Режим DMA успешно активирован.\n");
        else
            kdebug("ВНИМАНИЕ: Режим DMA не активирован\nОжидалось значение 1, получено %d\n", inb(IDE_COMMAND_PRIMARY));

    } else if (dma && secondary) {
        while (prdt2read && prdt2 > (PRD*)0x9500) {if (!prdt2read) break;}
        prdt2read = false;
        PRD prd = {getPhysAddr((uint32_t)out), (uint16_t)(sectorsCount * 512), 0, 0x80};
        kdebug("Собрана структура PRD:\n\tФиз. адрес передачи: %x\n\tЧисло байтов для передачи: %d\n\tВерхний байт: %x\n", prd.base, prd.count, prd.msb);
        if (prdt2 == (PRD*)0x9700) {
            *prdt2 = prd;
            prdt2 ++;
        } else {
            (--prdt2)->msb = 0;
            *(++prdt2) = prd;
            prdt2++;
        }
        kdebug("Теперь PRDT2 выглядит так:\n");
        uint8_t i = 0;
        PRD* dbgPRDPtr = (PRD*)0x9700;
        while (true) {
            kdebug("Вхождение %d:\n", i+1);
            kdebug("\tФиз. адрес: %x\n", dbgPRDPtr->base);
            kdebug("\tЧисло байтов для записи: %d\n", dbgPRDPtr->count);
            kdebug("\tПоследний ли? ");
            kdebug(dbgPRDPtr->msb ? "Да\n" : "Нет\n");
            if (dbgPRDPtr->msb)
                break;
            if (dbgPRDPtr->base == 0 && dbgPRDPtr->count == 0 && dbgPRDPtr->msb == 0) {
                kdebug("Пустое вхождение.\n");
                break;
            }
            dbgPRDPtr ++;
            i ++;
        }
        kdebug("Задаются параметры...\n");
        outb(ATA_DRIVE_HEAD_SECONDARY, slave ? 0xF0 : 0xE0);
        kdebug("Регистр выбора диска: %x\n", inb(ATA_DRIVE_HEAD_SECONDARY));
        outb(ATA_SECTOR_COUNT_SECONDARY, sectorsCount);
        kdebug("Регистр числа секторов: %d\n", inb(ATA_SECTOR_COUNT_SECONDARY));
        outb(ATA_LBA_LOW_SECONDARY, startLBA & 0xFF);
        kdebug("LBA низший: %x\n", inb(ATA_LBA_LOW_SECONDARY));
        outb(ATA_LBA_MID_SECONDARY, (startLBA >> 8) & 0xFF);
        kdebug("LBA средний: %x\n", inb(ATA_LBA_MID_SECONDARY));
        outb(ATA_LBA_HIGH_SECONDARY, (startLBA >> 16) & 0xFF);
        kdebug("LBA высший: %x\n", inb(ATA_LBA_HIGH_SECONDARY));
        if (prdt2 == (PRD*)0x9708) {
            kdebug("prdt2 пуста. Контроллер переводится в режим записи.\n");
            outb(IDE_COMMAND_SECONDARY, 0);
            outb(IDE_COMMAND_SECONDARY, 1);
        }
        outb(ATA_COMMAND_SECONDARY, 0xCA);
        kdebug("Выслана команда WRITE DMA.\n");
        if (inb(IDE_COMMAND_SECONDARY) & 1)
            kdebug("Режим DMA успешно активирован.\n");
        else
            kdebug("ВНИМАНИЕ: Режим DMA не активирован\nОжидалось значение 1, получено %d\n", inb(IDE_COMMAND_SECONDARY));
    }
    while (transferring) {if (!transferring) break;}
    kdebug("Запись на диск успешно завершена.\n\n");
}