#include "acpi.hpp"
#include "../memmgr/paging.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/io.hpp"
#include "../int/pic.hpp"
#include "../pci/pci.hpp"

dword *globalLock = nullptr;
dword *apics = (dword*)0x13000;

FADT *fadt;
FACS *facs;

bool hwReduced;
bool wbinvd;
bool resetRegSupport;

bool verifyRSDP(RSDP rsdp) {
    byte sum = 0;
    byte *ptr = (byte*)&rsdp;
    for (byte i = 0; i < sizeof(RSDP); i++) {
        sum += *ptr++;
    }
    return sum == 0;
}

bool verifyHeader(ACPITableHeader *header) {
    byte sum = 0;
    for (dword i = 0; i < header->length; i++) {
        sum += ((byte*)header)[i];
    }
    return sum == 0;
}

bool checkACPI() {
    return fadt->smiCmdPort == 0 || (fadt->acpiEnable == 0 && fadt->acpiDisable == 0) || inw(fadt->pm1aCtrlBlock) & 1;
}

RSDP findRSDP() {
    kdebug("Начат поиск RSDP.\n");
    qword *ptr = (qword*)0xE0000;
    while (ptr < (qword*)0xFFFFF) {
        if (*ptr == 0x2052545020445352) {
            kdebug("Найдена подпись RSDP по адресу %x.\n", (dword)ptr);
            RSDP rsdp = *(RSDP*)ptr;
            return rsdp;
        }
        ptr ++;
        ptr ++;
    }
    kdebug("ОШИБКА: Не удалось найти RSDP.\n");
    return {{0,0,0,0,0,0,0,0},1,{0,0,0,0,0,0},0,0};
}

bool parseTable(dword *table) {
    kdebug("Адрес таблицы: %x.\n", (dword)table);
    if (*table == 'PCAF') {
        kdebug("Тип таблицы - FADT.\n");
        FADT* _fadt = (FADT*)table;
        fadt = _fadt;
        if (!verifyHeader(&(_fadt->header))) {
            kdebug("ОШИБКА: FADT повреждена.\n");
            return false;
        }
        kdebug("Вид системы ");
        switch (_fadt->preferredPMP) {
            case 1: kdebug("- компьютер.\n"); break;
            case 2: kdebug("- мобильное устройство.\n"); break;
            case 3: kdebug("- рабочая станция.\n"); break;
            case 4: kdebug("- сервер.\n"); break;
            case 5: kdebug("- сервер SOHO.\n"); break;
            case 6: kdebug("- ПК.\n"); break;
            case 7: kdebug("- производительный сервер.\n"); break;
            case 8: kdebug("- планшет.\n"); break;
            default: kdebug("не определён (%d).\n", _fadt->preferredPMP); break;
        }
        kdebug("Прерывания SCI настроены на номер %d.\n", _fadt->sciInt);
        kdebug("Адрес FACS: %x.\n", _fadt->facsAddr);
        createPages(_fadt->facsAddr, _fadt->facsAddr, 4);
        parseTable((dword*)_fadt->facsAddr);
        kdebug("Адрес DSDT: %x.\n", _fadt->dsdtAddr);
        dword dsdtLen = ((ACPITableHeader*)(_fadt->dsdtAddr))->length + ((_fadt->dsdtAddr) & 0xFFF);
        createPages(_fadt->dsdtAddr, _fadt->dsdtAddr, ((dsdtLen + 1023) / 1024) + 1);
        return parseTable((dword*)_fadt->dsdtAddr);
    }
    else if (*table == 'SCAF') {
        kdebug("Обработка FACS.\n");
        FACS* _facs = (FACS*)table;
        facs = _facs;
        globalLock = &(_facs->globalLock);
        kdebug("Адрес глобального замка установлен на %x.\n", (dword)globalLock);
        return true;
    }
    else if (*table == 'CIPA') {
        kdebug("Тип таблицы - MADT.\n");
        MADT* madt = (MADT*)table;
        byte *ctrl = (byte*)table + 44;
        while ((dword)ctrl - (dword)madt < madt->header.length) {
            switch (dword apicAddr; *ctrl) {
                case 0:
                    kdebug("Обнаружен локальный APIC ЦП.\n");
                    kdebug("\tИдентификатор процессора: %d.\n", *(ctrl + 2));
                    break;
                case 1:
                    kdebug("Обнаружен I/O APIC.\n");
                    apicAddr = *((dword*)(ctrl + 4));
                    kdebug("\tАдрес APICа: %x.\n", apicAddr);
                    *apics++ = apicAddr;
                    break;
                case 2:
                    kdebug("Обнаружена перегрузка источника прерывания.\n");
                    kdebug("\tНомер IRQ: %d.\n", *(ctrl + 3));
                    break;
                default:
                    break;
            }
            ctrl += *(ctrl+1);
        }
        return true;
    }
    else if (*table == 'TDSD') {
        kdebug("Обработка DSDT.\n");
        DSDT *dsdt = (DSDT*)table;
        if (!verifyHeader(&(dsdt->header))) {
            kdebug("ОШИБКА: DSDT повреждена.\n");
            return false;
        }
        kdebug("В таблице %d байтов AML по адресу %x.\n", dsdt->header.length, dsdt);
        kprint("%x %d\n", dsdt, dsdt->header.length);
        parseDefBlock((byte*)dsdt);
        return true;
    }
    else if (*table == 'TDSS') {
        kdebug("Тип таблицы - SSDT.\n");
        SSDT *ssdt = (SSDT*)table;
        if (!verifyHeader(&(ssdt->header))) {
            kdebug("ОШИБКА: SSDT повреждена.\n");
            return false;
        }
        kdebug("В таблице %d байтов AML по адресу %x.\n", ssdt->header.length, ssdt);
        return true;
    }
    else if (*table == 'TEPH') {
        kdebug("Тип таблицы - HPET.\n");
        HPETT *hpet = (HPETT*)table;
        if (!verifyHeader(&(hpet->header))) {
            kdebug("ОШИБКА: HPET повреждена.\n");
            kdebug("ВНИМАНИЕ: Это не значит, что таймер HPET не работоспособен!\n");
            return false;
        }
        kdebug("Основа адресов HPETа: %x.\n", hpet->baseAddr.addr);
        return true;
    }
    else if (*table == 'TEAW') {
        kdebug("Тип таблицы - WAET.\n");
        WAET* waet = (WAET*)table;
        if (waet->emDevFlags & 1)
            kdebug("RTC пропатчен.\n");
        else
            kdebug("RTC не пропатчен.\n");
        if (waet->emDevFlags & 2)
            kdebug("Таймер PM ACPI пропатчен.\n");
        else
            kdebug("Таймер PM ACPI не пропатчен.\n");
        return true;
    }
    else {
        kdebug("Вид таблицы не определён.\nПодпись таблицы: ");
        for (byte i = 0; i < 4; i++) {
            kdebug(((byte*)table)[i]);
        }
        kdebug(".\n");
        return true;
    }
}

word readPM1aSts() {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1a_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1aEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            return *(word*)(fadt->x_pm1aEventBlock.addr);
        }
        else {
            kdebug("I/O.\n");
            return inw(fadt->x_pm1aEventBlock.addr);
        }
    } else {
        kdebug("Используется поле PM1a_EVT.\n");
        return inw(fadt->pm1aEventBlock);
    }
}

void writePM1aSts(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1a_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1aEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1aEventBlock.addr) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1aEventBlock.addr, val);
        }
    } else {
        kdebug("Используется поле PM1a_EVT.\n");
        outw(fadt->pm1aEventBlock, val);
    }
}

void writePM1aEnable(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1a_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1aEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1aEventBlock.addr + fadt->pm1EventLength / 2) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1aEventBlock.addr + fadt->pm1EventLength / 2, val);
        }
    } else {
        kdebug("Используется поле PM1a_EVT.\n");
        outw(fadt->pm1aEventBlock + fadt->pm1EventLength / 2, val);
    }
}

word readPM1bSts() {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1b_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1bEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            return *(word*)(fadt->x_pm1bEventBlock.addr);
        }
        else {
            kdebug("I/O.\n");
            return inw(fadt->x_pm1bEventBlock.addr);
        }
    } else {
        kdebug("Используется поле PM1b_EVT.\n");
        return inw(fadt->pm1bEventBlock);
    }
}

void writePM1bSts(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1b_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1bEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1bEventBlock.addr) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1bEventBlock.addr, val);
        }
    } else {
        kdebug("Используется поле PM1b_EVT.\n");
        outw(fadt->pm1bEventBlock, val);
    }
}

void writePM1bEnable(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1b_EVT.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1bEventBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1bEventBlock.addr + fadt->pm1EventLength / 2) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1bEventBlock.addr + fadt->pm1EventLength / 2, val);
        }
    } else {
        kdebug("Используется поле PM1b_EVT.\n");
        outw(fadt->pm1bEventBlock + fadt->pm1EventLength / 2, val);
    }
}

word readPM1aCtrl() {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1a_CTRL.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1aCtrlBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            return *(word*)(fadt->x_pm1aCtrlBlock.addr);
        }
        else {
            kdebug("I/O.\n");
            return inw(fadt->x_pm1aCtrlBlock.addr);
        }
    } else {
        kdebug("Используется поле PM1a_CTRL.\n");
        return inw(fadt->pm1aCtrlBlock);
    }
}

void writePM1aCtrl(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1a_CTRL.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1aCtrlBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1aCtrlBlock.addr) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1aCtrlBlock.addr, val);
        }
    } else {
        kdebug("Используется поле PM1a_CTRL.\n");
        outw(fadt->pm1aCtrlBlock, val);
    }
}

word readPM1bCtrl() {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1b_CTRL.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1bCtrlBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            return *(word*)(fadt->x_pm1bCtrlBlock.addr);
        }
        else {
            kdebug("I/O.\n");
            return inw(fadt->x_pm1bCtrlBlock.addr);
        }
    } else {
        kdebug("Используется поле PM1b_CTRL.\n");
        return inw(fadt->pm1bCtrlBlock);
    }
}

void writePM1bCtrl(word val) {
    if (fadt->header.revision >= 2) {
        kdebug("Используется поле X_PM1b_CTRL.\n");
        kdebug("Пространство поля: ");
        if (fadt->x_pm1bCtrlBlock.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            *(word*)(fadt->x_pm1bCtrlBlock.addr) = val;
        }
        else {
            kdebug("I/O.\n");
            outw(fadt->x_pm1bCtrlBlock.addr, val);
        }
    } else {
        kdebug("Используется поле PM1b_EVT.\n");
        outw(fadt->pm1bCtrlBlock, val);
    }
}

bool initACPI() {
    kdebug("Начата инициализация ACPI.\n");
    RSDP rsdp = findRSDP();
    if (!verifyRSDP(rsdp)) {
        if (*(rsdp.signature) != 0)
            kdebug("ОШИБКА: RSDP повреждена.\n\n");
        return false;
    }
    kdebug("Найден целый RSDP:\n");
    kdebug("\tПодпись: \"");
    for (byte i = 0; i < 8; i++)
        kdebug((byte)rsdp.signature[i]);
    kdebug("\"\n");
    kdebug("\tКонтрольная сумма: %x\n", rsdp.checksum);
    kdebug("\tИдентификатор OEM: \"");
    for (byte i = 0; i < 6; i++)
        kdebug((byte)rsdp.oemID[i]);
    kdebug("\"\n");
    kdebug("\tРевизия: %d\n", rsdp.revision);
    kdebug("\tАдрес RSDT: %x\n", rsdp.rsdtAddr);

    createPages(rsdp.rsdtAddr, rsdp.rsdtAddr, 4);
    dword *rsdt = (dword*)rsdp.rsdtAddr;
    if (!verifyHeader((ACPITableHeader*)rsdt)) {
        kdebug("ОШИБКА: RSDT повреждена.\n\n");
        return false;
    }

    byte tables = ((*(ACPITableHeader*)rsdt).length - sizeof(ACPITableHeader)) / 4;
    rsdt += sizeof(ACPITableHeader) / 4;
    kdebug("Найдено %d таблиц.\n", tables);
    for (byte i = 0; i < tables; i++) {
        kdebug("Обработка таблицы %d...\n", i+1);
        createPages(rsdt[i], rsdt[i], 4);
        if (!parseTable((dword*)rsdt[i])) return false;
    }

    if (checkACPI()) {
        kdebug("Интерфейс ACPI уже активен.\n\n");
        return true;
    }

    outb(fadt->smiCmdPort, fadt->acpiEnable);
    while (inw(fadt->pm1aCtrlBlock) & 1 == 0) {io_wait();}

    if (!checkACPI()) {
        kdebug("ОШИБКА: ACPI не активировался.\n\n");
        return false;
    }
    kdebug("Режим ACPI успешно активирован.\n");

    writePM1aEnable(0b1100100000);
    writePM1bEnable(0b1100100000);

    wbinvd = fadt->flags & 1;
    kdebug("Поддерживается ли инструкция WBINVD? ");
    kdebug(wbinvd ? "Да\n" : "Нет\n");

    hwReduced = fadt->flags & (1 << 20);
    kdebug("Является ли платформа HW-редуцированной? ");
    kdebug(hwReduced ? "Да\n" : "Нет\n");

    resetRegSupport = fadt->flags & (1 << 10);
    kdebug("Поддерживается ли регистр RESET? ");
    kdebug(resetRegSupport ? "Да\n" : "Нет\n");

    byte sciIntNo = irqOffset + fadt->sciInt;
    kdebug("Номер прерывания SCI: %x.\n", sciIntNo);
    encode_idt_entry(sciHandler, sciIntNo);
    unmaskIRQ(sciIntNo);

    kdebug("Инициализация ACPI завершена успешно.\n\n");
    return true;
}

void enterSleepState(byte state) {
    kdebug("Запрошен переход в состояние сна S%d.\n", state);
    dword name;
    byte *ptr = (byte*)&name;
    ptr[0] = '_';
    ptr[1] = 'S';
    ptr[2] = state + 0x30;
    ptr[3] = '_';
    kdebug("Имя пакета: ");
    logName(name);
    kdebug(".\n");
    byte *addr = getACPIObjAddr(&name, 1);
    addr += 2;
    dword addr1 = *(dword*)addr;
    addr += 4;
    byte slp_typA = getIntegerTerm((byte*)addr1);
    dword addr2 = *(dword*)addr;
    byte slp_typB = getIntegerTerm((byte*)addr1);
    kdebug("Значение SLP_TYPa: %b.\n", slp_typA);
    kdebug("Значение SLP_TYPb: %b.\n", slp_typB);

    callMethod("\\_TTS", state);

    // TODO: Перевод всех устройств в нужное состояние сна

    callMethod("\\_PTS", state);

    if (state != 5) {
        kdebug("Производится переход в состояние сна.\n");
        kdebug("// TODO: Записать контекст других ядер в память\n");
        facs->wakingVector = 0x3000;
        kdebug("Вектор пробуждения установлен на %x.\n", facs->wakingVector);
        if (state < 4) {
            kdebug("Производится очистка кэша ЦП.\n");
            if (wbinvd) {
                __asm__ ("wbinvd");
            } else {
                kdebug("// TODO: Ручная очистка кэша ЦП");
            }
        }
    }

    if (!hwReduced) {
        word pm1aSts = readPM1aSts();
        kdebug("Значение PM1a_STS: %b.\n", pm1aSts);
        writePM1aSts(pm1aSts | (1 << 15));
        word pm1bSts = readPM1bSts();
        kdebug("Значение PM1b_STS: %b.\n", pm1bSts);
        writePM1bSts(pm1bSts | (1 << 15));

        word pm1aCtrl = readPM1aCtrl();
        kdebug("Значение PM1a_CTRL: %b.\n", pm1aCtrl);
        writePM1aCtrl(pm1aCtrl | (1 << 13) | (slp_typA << 10));
    }
    word pm1bCtrl = readPM1bCtrl();
    kdebug("Значение PM1b_CTRL: %b.\n", pm1bCtrl);
    writePM1bCtrl(pm1bCtrl | (1 << 13) | (slp_typB << 10));
}

void kshutdown() {
    kdebug("Запрошена процедура отключения.\n");
    exitDebugger();
    enterSleepState(5);
}

void krestart() {
    kdebug("Запрошена процедура перезагрузки.\n");

    if (resetRegSupport) {
        kdebug("Используется RESET_REGISTER.\n");   
        kdebug("Адрес регистра сброса: %x.\n", fadt->resetRegister.addr);
        kdebug("Пространство регистра сброса: ");
        if (fadt->resetRegister.addrSpace == 0) {
            kdebug("ОЗУ.\n");
            createPage(fadt->resetRegister.addr, fadt->resetRegister.addr);
            *(byte*)fadt->resetRegister.addr = fadt->resetValue;
        }
        else if (fadt->resetRegister.addrSpace == 1) {
            kdebug("I/O.\n");
            outb(fadt->resetRegister.addr, fadt->resetValue);
        }
        else if (fadt->resetRegister.addrSpace == 2) {
            kdebug("PCI.\n");
            byte device = (fadt->resetRegister.addr >> 32) & 0xFF;
            byte func = (fadt->resetRegister.addr >> 16) & 0xFF;
            byte offset = (fadt->resetRegister.addr) & 0xFF;
            pciConfigWriteB(0, device, func, offset, fadt->resetValue);
        }
    }
    else {
        kdebug("RESET_REG не поддерживается.\n");
        kdebug("// TODO: Ручной сброс всех периферийных устройств\n");

        byte tmp;
        do {
            tmp = inb(0x64);
        } while (tmp & 0x02);
        outb(0x64, 0xFE);
    }
}

void ksleep() {
    kdebug("Запрошена процедура сна.\n");

    byte maxSleep = 0;
    if (getACPIObjAddr("_S1"))
        maxSleep = 1;
    if (getACPIObjAddr("_S2"))
        maxSleep = 2;
    if (getACPIObjAddr("_S3"))
        maxSleep = 3;
    if (getACPIObjAddr("_S4"))
        maxSleep = 4;
    
    kdebug("Максимальное состояние сна: %d.\n", maxSleep);
    enterSleepState(maxSleep);
}