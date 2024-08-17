#include "acpi.hpp"
#include "../io/com.hpp"
#include "../memmgr/paging.hpp"
#include "../io/io.hpp"
#include "aml.hpp"

uint32_t *globalLock = nullptr;
uint32_t *apics = (uint32_t*)0x13000;

FADT fadt;

bool verifyRSDP(RSDP rsdp) {
    uint8_t sum = 0;
    uint8_t *ptr = (uint8_t*)&rsdp;
    for (uint8_t i = 0; i < sizeof(RSDP); i++) {
        sum += *ptr++;
    }
    return sum == 0;
}

bool verifyHeader(ACPITableHeader *header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < header->length; i++) {
        sum += ((uint8_t*)header)[i];
    }
    return sum == 0;
}

bool checkACPI() {
    return fadt.smiCmdPort == 0 || (fadt.acpiEnable == 0 && fadt.acpiDisable == 0) || inw(fadt.pm1aCtrlBlock) & 1;
}

RSDP findRSDP() {
    kdebug("Начат поиск RSDP.\n");
    uint64_t *ptr = (uint64_t*)0xE0000;
    while (ptr < (uint64_t*)0xFFFFF) {
        if (*ptr == 0x2052545020445352) {
            kdebug("Найдена подпись RSDP по адресу %x.\n", (uint32_t)ptr);
            RSDP rsdp = *(RSDP*)ptr;
            return rsdp;
        }
        ptr ++;
        ptr ++;
    }
    kdebug("ОШИБКА: Не удалось найти RSDP.\n");
    return {{0,0,0,0,0,0,0,0},1,{0,0,0,0,0,0},0,0};
}

bool parseTable(uint32_t *table) {
    kdebug("Адрес таблицы: %x.\n", (uint32_t)table);
    if (*table == 'PCAF') {
        kdebug("Тип таблицы - FADT.\n");
        FADT* _fadt = (FADT*)table;
        fadt = *_fadt;
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
        parseTable((uint32_t*)_fadt->facsAddr);
        kdebug("Адрес DSDT: %x.\n", _fadt->dsdtAddr);
        createPages(_fadt->dsdtAddr, _fadt->dsdtAddr, 4);
        return parseTable((uint32_t*)_fadt->dsdtAddr);
    }
    else if (*table == 'SCAF') {
        kdebug("Обработка FACS.\n");
        FACS* facs = (FACS*)table;
        facs->wakingVector = 0x12000;
        kdebug("Адрес вектора пробуждения установлен на %x.\n", facs->wakingVector);
        globalLock = &(facs->globalLock);
        kdebug("Адрес глобального замка установлен на %x.\n", (uint32_t)globalLock);
        return true;
    }
    else if (*table == 'CIPA') {
        kdebug("Тип таблицы - MADT.\n");
        MADT* madt = (MADT*)table;
        uint8_t *ctrl = (uint8_t*)table + 44;
        while ((uint32_t)ctrl - (uint32_t)madt < madt->header.length) {
            switch (uint32_t apicAddr; *ctrl) {
                case 0:
                    kdebug("Обнаружен локальный APIC ЦП.\n");
                    kdebug("\tИдентификатор процессора: %d.\n", *(ctrl + 2));
                    break;
                case 1:
                    kdebug("Обнаружен I/O APIC.\n");
                    apicAddr = *((uint32_t*)(ctrl + 4));
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
        parseDefBlock((uint8_t*)dsdt);
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
        for (uint8_t i = 0; i < 4; i++) {
            writeCom(((uint8_t*)table)[i], 1);
        }
        kdebug(".\n");
        return true;
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
    for (uint8_t i = 0; i < 8; i++)
        writeCom(rsdp.signature[i], 1);
    kdebug("\"\n");
    kdebug("\tКонтрольная сумма: %x\n", rsdp.checksum);
    kdebug("\tИдентификатор OEM: \"");
    for (uint8_t i = 0; i < 6; i++)
        writeCom(rsdp.oemID[i], 1);
    kdebug("\"\n");
    kdebug("\tРевизия: %d\n", rsdp.revision);
    kdebug("\tАдрес RSDT: %x\n", rsdp.rsdtAddr);

    createPages(rsdp.rsdtAddr, rsdp.rsdtAddr, 4);
    uint32_t *rsdt = (uint32_t*)rsdp.rsdtAddr;
    if (!verifyHeader((ACPITableHeader*)rsdt)) {
        kdebug("ОШИБКА: RSDT повреждена.\n\n");
        return false;
    }

    uint8_t tables = ((*(ACPITableHeader*)rsdt).length - sizeof(ACPITableHeader)) / 4;
    rsdt += sizeof(ACPITableHeader) / 4;
    kdebug("Найдено %d таблиц.\n", tables);
    for (uint8_t i = 0; i < tables; i++) {
        kdebug("Обработка таблицы %d...\n", i+1);
        createPages(rsdt[i], rsdt[i], 4);
        if (!parseTable((uint32_t*)rsdt[i])) return false;
    }

    if (checkACPI()) {
        kdebug("Интерфейс ACPI уже активен.\n\n");
        return true;
    }

    outb(fadt.smiCmdPort, fadt.acpiEnable);
    while (inw(fadt.pm1aCtrlBlock) & 1 == 0) {io_wait();}

    if (!checkACPI()) {
        kdebug("ОШИБКА: ACPI не активировался.\n\n");
        return false;
    }
    kdebug("Режим ACPI успешно активирован.\n\n");
    return true;
}