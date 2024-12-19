#include "acpi.hpp"
#include "../memmgr/paging.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/io.hpp"
#include "../int/pic.hpp"
#include "../pci/pci.hpp"
#include "../dbg/dbg.hpp"

FADT *fadt;
FACS *facs;

bool hwReduced;
bool wbinvd;
bool resetRegSupport;

byte maxSleep = 0;

const dword wakeStrapAddr = 0x3000;

CPUContext *ctxBuf;

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
        kdebug("Вид системы - ");
        switch (_fadt->preferredPMP) {
            case 1: kdebug("компьютер.\n"); break;
            case 2: kdebug("мобильное устройство.\n"); break;
            case 3: kdebug("рабочая станция.\n"); break;
            case 4: kdebug("сервер.\n"); break;
            case 5: kdebug("сервер SOHO.\n"); break;
            case 6: kdebug("ПК.\n"); break;
            case 7: kdebug("производительный сервер.\n"); break;
            case 8: kdebug("планшет.\n"); break;
            default: kdebug("(%d).\n", _fadt->preferredPMP); break;
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

void prepareWakeFunction() {
    kdebug("Функция пробуждения расположена по адресу %x.\n", wakeStrapAddr);

    byte func[] = { 0xEA, 0x09, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0xC0, 0x8E, 0xD8, 
    0x8E, 0xD0, 0x8E, 0xC0, 0xBC, 0x00, 0x50, 0x89, 0xE5, 0xBF, 0x00, 0x15, 0xB8, 0x48, 0x00, 0xAB, 
    0x66, 0xB8, 0x00, 0x10, 0x00, 0x00, 0x66, 0xAB, 0x0F, 0x01, 0x16, 0x00, 0x15, 0xBF, 0x10, 0x15, 
    0xB8, 0x8F, 0x01, 0xAB, 0x66, 0xB8, 0x00, 0x00, 0x01, 0x00, 0x66, 0xAB, 0x0F, 0x01, 0x1E, 0x10, 
    0x15, 0x66, 0xB8, 0x00, 0x10, 0x10, 0x00, 0x0F, 0x22, 0xD8, 0x0F, 0x20, 0xC0, 0x66, 0x0D, 0x01, 
    0x00, 0x00, 0x80, 0x0F, 0x22, 0xC0, 0xEA, 0x58, 0x30, 0x08, 0x00, 0x66, 0xB8, 0x10, 0x00, 0x8E, 
    0xD8, 0x8E, 0xD0, 0x8E, 0xC0, 0xFF, 0x25, 0x05, 0x30, 0x00, 0x00, 0xFA, 0xF4 };

    byte *ptr = (byte*)wakeStrapAddr;

    memcpy(func, ptr, sizeof(func));
    dword *kernelWakeAddr = (dword*)0x3005;
    *kernelWakeAddr = (dword)&wake;
    ptr += sizeof(func);
    ctxBuf = (CPUContext*)ptr;

    kdebug("Функция пробуждения подготовлена.\n");
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

    byte majorVer = fadt->header.revision;
    byte minorVer = fadt->minorVer & 0xF;
    byte erratum  = fadt->minorVer >> 4;

    kdebug("Версия спецификации: %d.%d", majorVer, minorVer);
    if (erratum) kdebug((byte)(erratum + 0x41 - 1));
    kdebug(".\n");

    byte sciIntNo = irqOffset + fadt->sciInt;
    kdebug("Номер прерывания SCI: %x.\n", sciIntNo);
    encode_idt_entry(sciHandler, sciIntNo);
    unmaskIRQ(sciIntNo);

    if (getACPIObjAddr("_S1"))
        maxSleep = 1;
    if (getACPIObjAddr("_S2"))
        maxSleep = 2;
    if (getACPIObjAddr("_S3"))
        maxSleep = 3;
    
    kdebug("Максимальное поддерживаемое состояние сна: S%d.\n", maxSleep);
    prepareWakeFunction();

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

    FOR_ALL_DEVICES {
        byte len = *device++;
        kdebug("Рассматривается устройство ");
        logPath(device, len);
        kdebugnewl();

        byte dstate = 0;
        bool allowWake = false;
        char* sxw = (char*)"_SxW";
        sxw[2] = 0x30 + state;
        char* sxd = (char*)"_SxD";
        sxd[2] = 0x30 + state;
        byte deviceState = evaluateDeviceObj(device, len, (const char*)sxd);
        byte wakeState   = evaluateDeviceObj(device, len, (const char*)sxw);
        if (deviceState == maxbyte)
            deviceState = 0;
        if (wakeState == maxbyte)
            wakeState = 3;
        kdebug("Значение _S%dD: %d.\n", state, deviceState);
        kdebug("Значение _S%dW: %d.\n", state, wakeState);
        byte *prw = (byte*)evaluateDeviceObj(device, len, "_PRW");
        if ((dword)prw != maxdword) {
            prw += 10;
            byte *sstateAddr = (byte*)*(dword*)prw;
            byte maxWakeSState = getIntegerTerm(sstateAddr);
            kdebug("Максимальное состояние сна с пробуждением: S%d.\n", maxWakeSState);
            if (maxWakeSState < state) dstate = 3;
            else if (wakeState == 3) dstate = deviceState;
            else dstate = (deviceState + wakeState) / 2;
            allowWake = maxWakeSState >= state;
        } else {
            allowWake = false;
            dstate = 3;
        }

        if (allowWake) {
            evaluateDeviceObj(device, len, "_DSW", 1, state, dstate);
        }
        
        transitionToDState(device, len, dstate);

        device += len;
    }

    callMethod("\\_PTS", state);

    if (state != 5) {
        kdebug("Производится переход в состояние сна.\n");
        kdebug("// TODO: Записать контекст других процессоров в память\n");
        facs->wakingVector = wakeStrapAddr;
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

    callMethod("\\_GTS", state);

    if (!hwReduced) {
        word pm1aSts = readPM1aSts();
        kdebug("Значение PM1a_STS: %b.\n", pm1aSts);
        writePM1aSts(1 << 15);
        word pm1bSts = readPM1bSts();
        kdebug("Значение PM1b_STS: %b.\n", pm1bSts);
        writePM1bSts(1 << 15);

        word pm1aCtrl = readPM1aCtrl();
        kdebug("Значение PM1a_CTRL: %b.\n", pm1aCtrl);
        writePM1aCtrl(pm1aCtrl | (1 << 13) | (slp_typA << 10));
    }
    word pm1bCtrl = readPM1bCtrl();
    kdebug("Значение PM1b_CTRL: %b.\n", pm1bCtrl);
    writePM1bCtrl(pm1bCtrl | (1 << 13) | (slp_typB << 10));
}

void wake() {
    kprint("Доброе утро!\n");

    callMethod("\\_WAK");

    callMethod("\\_TTS", 0);

    CPUContext ctx = *ctxBuf;
    magicBreakpoint();
    ctx.restore();
    return;
}

void ksleep() {
    CPUContext ctx = CPUContext::store();
    *ctxBuf = ctx;

    kdebug("Запрошена процедура сна.\n");

    enterSleepState(maxSleep);
}

void kshutdown() {
    kdebug("Запрошена процедура отключения.\n");
    enterSleepState(5);
    exitDebugger();
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
        kdebug("============ ПЕРЕЗАГРУЗКА ============\n\n");

        byte tmp;
        do {
            tmp = inb(0x64);
        } while (tmp & 0x02);
        outb(0x64, 0xFE);
    }
}