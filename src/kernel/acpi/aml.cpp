#include "aml.hpp"
#include "../io/com.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"

const uint32_t parsingPathBase = 0x13000;
AMLName *parsingPath = (AMLName*)parsingPathBase;

const uint32_t acpiNamespaceBase = 0x13100;
uint32_t *acpiNamespace = (uint32_t*)acpiNamespace;

const uint32_t acpiDataBase = 0x14000;
uint8_t* acpiData = (uint8_t*)acpiDataBase;

const uint32_t acpiFuncsBase = 0x16000;
uint8_t *acpiFuncs = (uint8_t*)acpiFuncsBase;

inline uint32_t getParsingPathLen() {
    return ((uint32_t)parsingPath - parsingPathBase) / sizeof(AMLName);
}

inline uint8_t getPkgBytes(uint8_t *pkg) {
    return (*pkg >> 6) + 1;
}

uint32_t getPkgLength(uint8_t *pkg) {
    uint8_t numBytes = getPkgBytes(pkg);
    if (numBytes == 1) {
        return *pkg & 0x3F;
    }
    uint32_t len = *pkg++ & 0x0F;
    uint8_t shift = 4;
    for (uint8_t i = 1; i < numBytes; i++) {
        len |= (*pkg++) << shift;
        shift += 8;
    }
    return len;
}

uint64_t getIntegerTerm(uint8_t* aml) {
    uint64_t res;
    switch (*aml) {
        case 0x0A:
            res = *(++aml);
            break;
        case 0x0B:
            res = *(uint16_t*)(++aml);
            break;
        case 0x0C:
            res = *(uint32_t*)(++aml);
            break;
        case 0x0E:
            res = *(uint64_t*)(++aml);
            break;
        default:
            res = 0;
            break;
    }
    return res;
}

uint8_t getIntegerTermBytes(uint8_t* aml) {
    switch (*aml) {
        case 0x0A: return 2;
        case 0x0B: return 3;
        case 0x0C: return 5;
        case 0x0E: return 9;
    }
    return 0;
}

uint32_t parseBuffer(uint8_t *aml) {
    uint32_t bufLen = getPkgLength(aml);
    kdebug("Длина буфера: %d Б.\n", bufLen);
    aml += getPkgBytes(aml);
    uint64_t numBytes = getIntegerTerm(aml);
    aml += getIntegerTermBytes(aml);
    uint8_t element = 1;
    for (uint8_t i = 0; i < numBytes; i++) {
        uint16_t elLen;
        uint8_t type;
        bool isLarge = aml[i] & 0x80;
        if (isLarge) {
            type = aml[i] & 0x7F;
            elLen = *(uint16_t*)(aml + i + 1);
            i += 3;
        }
        else {
            type = (aml[i] & 0x78) >> 3;
            elLen = aml[i] & 0x7;
            i ++;
        }

        kdebug("Длина элемента %d: %d Б.\n", element, elLen);
        kdebug("Тип элемента %d: %x.\n", element, type);
        kdebug("Большой ли? ");
        kdebug(isLarge ? "Да\n" : "Нет\n");
        if (!isLarge && type == 0x04) {
            kdebug("Тип элемента - IRQ.\n");
            uint8_t irq = 0;
            uint16_t irqField = *(uint16_t*)(aml + i);
            i += 2;
            for (uint16_t j = 0; j < 0x10; j++) {
                if (irqField & (1 << j)) {
                    irq = j;
                    break;
                }
            }
            kdebug("\tНомера IRQ: %d", irq);
            for (uint16_t j = irq+1; j < 0x10; j++) {
                if (irqField & (1 << j)) {
                    kdebug(", %d", j);
                }
            }
            kdebug("\n");
            if (elLen == 3) {
                uint8_t irqFlags = aml[i];
                i++;
                kdebug("\tРежим прерывания: ");
                kdebug((irqFlags & 1) ? "По изменении\n" : "По уровню\n");
                kdebug("\tПолярность прерывания: ");
                kdebug((irqFlags & 8) ? "ЛОГ 0\n" : "ЛОГ 1\n");
                kdebug("\tДеление прерывания: ");
                kdebug((irqFlags & 16) ? "Делится\n" : "Не делится\n");
                kdebug("\tПробуждение: ");
                kdebug((irqFlags & 32) ? "Да\n" : "Нет\n");
            } else {
                kdebug("\tРежим прерывания: ");
                kdebug("По изменении\n");
                kdebug("\tПолярность прерывания: ");
                kdebug("ЛОГ 1\n");
                kdebug("\tДеление прерывания: ");
                kdebug("Не делится\n");
                kdebug("\tПробуждение: ");
                kdebug("Нет\n");
            }
            i --;
        } else if (!isLarge && type == 0x05) {
            kdebug("Тип элемента - DMA.\n");
            uint8_t mask = aml[i];
            uint8_t chan;
            for (uint8_t i = 0; i < 8; i++) {
                if (mask & (1 << i)) {
                    chan = i;
                    break;
                }
            }
            i ++;
            kdebug("\tКанал: %d.\n");
            uint8_t flags = aml[i];
            kdebug("\tТип передачи: ");
            switch (flags & 3) {
                case 0: kdebug("Только 8 бит\n"); break;
                case 1: kdebug("8 бит и 16 бит\n"); break;
                case 2: kdebug("Только 16 бит\n"); break;
            }
            kdebug("\tБусмейстер? ");
            kdebug((flags & 4) ? "Да\n" : "Нет\n");
            kdebug("\tТип DMA: ");
            switch ((flags & 60) >> 5) {
                case 0: kdebug("Совместимость\n"); break;
                case 1: kdebug("A\n"); break;
                case 2: kdebug("B\n"); break;
                case 3: kdebug("F\n"); break;
            }
        } else if (!isLarge && type == 0x06) {
            kdebug("Тип элемента - StartDependentFn.\n");
            if (elLen == 1) {
                uint8_t priority = aml[i];
                kdebug("\tСовместимость: ");
                switch (priority & 3) {
                    case 0: kdebug("Хорошо\n"); break;
                    case 1: kdebug("Приемлемо\n"); break;
                    case 2: kdebug("Плохо\n"); break;
                }
                kdebug("\tПроизводительность: ");
                switch ((priority & 12) >> 2) {
                    case 0: kdebug("Хорошо\n"); break;
                    case 1: kdebug("Приемлемо\n"); break;
                    case 2: kdebug("Плохо\n"); break;
                }
            } else {
                kdebug("\tСовместимость: ");
                kdebug("Приемлемо\n");
                kdebug("\tПроизводительность: ");
                kdebug("Приемлемо\n");
            }
        } else if (!isLarge && type == 0x07) {
            kdebug("Тип элемента - EndDependentFn.\n");
        } else if (!isLarge && type == 0x08) {
            kdebug("Тип элемента - IO.\n");
            uint8_t info = aml[i];
            kdebug("\tШирина адреса: ");
            kdebug((info & 1) ? "16 бит\n" : "10 бит\n");
            i ++;
            uint16_t minAddr = *(uint16_t*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 2;
            uint16_t maxAddr = *(uint16_t*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 2;
            uint8_t align = aml[i];
            i ++;
            kdebug("\tРовнение: %d Б\n", align);
            uint8_t length = aml[i];
            kdebug("\tДлина пространства: %d Б\n", length);
        } else if (!isLarge && type == 0x09) {
            kdebug("Тип элемента - FixedIO.");
            uint16_t addr = *(uint16_t*)(aml + i);
            kdebug("\tАдрес: %x\n", addr);
            i += 2;
            uint8_t length = aml[i];
            kdebug("\tДлина пространства: %d Б\n", length);
        } else if (!isLarge && type == 0x0A) {
            kdebug("Тип элемента - FixedDMA.\n");
            uint16_t req = *(uint16_t*)(aml + i);
            kdebug("\tЛиния запроса: %x\n", req);
            i += 2;
            uint16_t chan = *(uint16_t*)(aml + i);
            kdebug("\tКанал: %x\n", chan);
            i += 2;
            uint8_t tranWidth = aml[i];
            kdebug("\tШирина передачи: %d бит\n", (1 << tranWidth) * 8);
        } else if (!isLarge && type == 0x0F) {
            kdebug("Тип элемента - End.\n");
            uint8_t checksum = aml[i];
            kdebug("\tКонтрольная сумма: %x\n", checksum);
        } else if (isLarge && type == 0x01) {
            kdebug("Тип элемента - Memory24.\n");
            uint8_t info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            uint16_t minAddr = *(uint16_t*)(aml + i);
            i += 2;
            kdebug("\tМинимальный адрес: %x\n", (uint32_t)minAddr << 8);
            uint16_t maxAddr = *(uint16_t*)(aml + i);
            i += 2;
            kdebug("\tМаксимальный адрес: %x\n", (uint32_t)maxAddr << 8);
            uint16_t align = *(uint16_t*)(aml + i);
            i += 2;
            kdebug("\tРовнение: %d Б\n", align ? align : 65536);
            uint16_t range = *(uint16_t*)(aml + i);
            i ++;
            kdebug("\tДлина пространства: %d Б\n", (uint32_t)range * 256);
        } else if (isLarge && type == 0x05) {
            kdebug("Тип элемента - Memory32.\n");
            uint8_t info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            uint32_t minAddr = *(uint32_t*)(aml + i);
            i += 4;
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            uint32_t maxAddr = *(uint32_t*)(aml + i);
            i += 4;
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            uint32_t align = *(uint32_t*)(aml + i);
            i += 4;
            kdebug("\tРовнение: %d Б\n", align);
            uint32_t range = *(uint32_t*)(aml + i);
            i += 3;
            kdebug("\tДлина пространства: %d Б\n", range);
        } else if (isLarge && type == 0x06) {
            kdebug("Тип элемента - Memory32Fixed.\n");
            uint8_t info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            uint32_t baseAddr = *(uint32_t*)(aml + i);
            i += 4;
            kdebug("\tОсновный адрес: %x\n", baseAddr);
            uint32_t range = *(uint32_t*)(aml + i);
            i += 3;
            kdebug("\tДлина пространства: %d Б\n", range);
        } else if (isLarge && type == 0x0A) {
            kdebug("Тип элемента - QWordAddrSpace.\n");
            uint8_t info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            uint8_t genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            uint8_t typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            uint64_t granularity = *(uint64_t*)(aml + i);
            kdebug("\tГранулярность: %D Б\n", granularity);
            i += 8;
            uint64_t minAddr = *(uint64_t*)(aml + i);
            kdebug("\tМинимальный адрес: %X\n", minAddr);
            i += 8;
            uint64_t maxAddr = *(uint64_t*)(aml + i);
            kdebug("\tМаксимальный адрес: %X\n", maxAddr);
            i += 8;
            uint64_t traOffset = *(uint64_t*)(aml + i);
            kdebug("\tСдвиг перевода: %X\n", traOffset);
            i += 8;
            uint64_t length = *(uint64_t*)(aml + i);
            kdebug("\tДлина пространства: %D Б\n", length);
            i += 7;
            if (elLen > 43) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (uint8_t j = 0; j < elLen - 43; j ++) {
                    if (aml[i] == 0) break;
                    writeCom(aml[i], 1);
                    i ++;
                }
                kdebug("\"\n");
            }
        } else if (isLarge && type == 0x07) {
            kdebug("Тип элемента - DWordAddrSpace.\n");
            uint8_t info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            uint8_t genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            uint8_t typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            uint32_t granularity = *(uint32_t*)(aml + i);
            kdebug("\tГранулярность: %d Б\n", granularity);
            i += 4;
            uint32_t minAddr = *(uint32_t*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 4;
            uint32_t maxAddr = *(uint32_t*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 4;
            uint32_t traOffset = *(uint32_t*)(aml + i);
            kdebug("\tСдвиг перевода: %x\n", traOffset);
            i += 4;
            uint32_t length = *(uint32_t*)(aml + i);
            kdebug("\tДлина пространства: %d Б\n", length);
            i += 3;
            if (elLen > 23) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (uint8_t j = 0; j < elLen - 23; j ++) {
                    if (aml[i] == 0) break;
                    writeCom(aml[i], 1);
                    i ++;
                }
                kdebug("\"\n");
            }
        } else if (isLarge && type == 0x08) {
            kdebug("Тип элемента - WordAddrSpace.\n");
            uint8_t info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            uint8_t genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            uint8_t typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            uint16_t granularity = *(uint16_t*)(aml + i);
            kdebug("\tГранулярность: %d Б\n", granularity);
            i += 2;
            uint16_t minAddr = *(uint16_t*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 2;
            uint16_t maxAddr = *(uint16_t*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 2;
            uint16_t traOffset = *(uint16_t*)(aml + i);
            kdebug("\tСдвиг перевода: %x\n", traOffset);
            i += 2;
            uint16_t length = *(uint16_t*)(aml + i);
            kdebug("\tДлина пространства: %d Б\n", length);
            i ++;
            if (elLen > 13) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (uint8_t j = 0; j < elLen - 13; j ++) {
                    if (aml[i] == 0) break;
                    writeCom(aml[i], 1);
                    i ++;
                }
                kdebug("\"\n");
                i --;
            }
        } else {
            i += elLen;
            if (isLarge)
                i += 2;
        }

        element ++;
    }
    return bufLen;
}

uint32_t parseVarPackage(uint8_t *aml) {
    kdebug("TODO: Обработка VarPackage\n");
    return getPkgLength(++aml);
}

uint32_t parsePackage(uint8_t *aml) {
    uint32_t pkgLen = getPkgLength(aml);
    aml += getPkgBytes(aml);
    kdebug("Длина пакета: %d Б.\n", pkgLen);
    uint8_t numElements = *aml++;
    kdebug("Число элементов: %d.\n", numElements);
    for (uint8_t i = 0; i < numElements; i++) {
        kdebug("Вид элемента %d - ", i+1);
        if (*aml == 0x0A) {
            kdebug("ByteConst.\nЗначение = %x.\n", *(++aml));
            aml ++;
        } else if (*aml == 0x0B) {
            kdebug("WordConst.\nЗначение = %x.\n", *(uint16_t*)(++aml));
            aml += 2;
        } else if (*aml == 0x0C) {
            kdebug("DWordConst.\nЗначение = %x.\n", *(uint32_t*)(++aml));
            aml += 4;
        } else if (*aml == 0x0E) {
            kdebug("QWordConst.\nЗначение = %x.\n", *(uint64_t*)(++aml));
            aml += 8;
        } else if (*aml == 0x0D) {
            kdebug("String.\nЗначение = \"");
            kdebug((const char*)aml);
            kdebug("\".\n");
        } else if (*aml == 0x00) {
            kdebug("ConstObj.\nЗначение = ZERO (0).\n");
            aml ++;
        } else if (*aml == 0x01) {
            kdebug("ConstObj.\nЗначение = ONE (1).\n");
            aml ++;
        } else if (*aml == 0xFF) {
            kdebug("ConstObj.\nЗначение = ONES (0xFF).\n");
            aml ++;
        } else if (*aml == 0x5B && *(aml + 1) == 0x30) {
            kdebug("RevisionOp.\n");
            aml += 2;
        } else if (*aml == 0x12) {
            kdebug("Package.\n");
            aml ++;
            aml += parsePackage(aml);
        } else if (*aml == 0x13) {
            kdebug("VarPackage.\n");
            aml ++;
            aml += parseVarPackage(aml);
        } else if (*aml == 0x11) {
            kdebug("Buffer.\n");
            aml ++;
            aml += parseBuffer(aml);
        } else if (*aml >= 0x20 && *aml <= 0x7E){
            kdebug("ссылка.\nЗначение = \"");
            for (uint8_t i = 0; i < 4; i++) {
                writeCom(*aml++, 1);
            }
            kdebug("\".\n");
        } else {
            kdebug("не определён.\nВНИМАНИЕ: Не удалось определить тип элемента %d.\nРовнение, вероятнее всего, сбито.\n", i+1);
            break;
        }
    }
    return pkgLen;
}

EISAId dwordToEISAId(uint32_t dword) {
    EISAId id;
    
    char *ptr = (char*)&dword;
    swap(ptr, ptr + 3);
    swap(ptr + 1, ptr + 2);

    id.mfg[0] = ((dword & 0x7C000000) >> 26) + 0x40;
    id.mfg[1] = ((dword & 0x3E00000) >> 21) + 0x40;
    id.mfg[2] = ((dword & 0x1F0000) >> 16) + 0x40;
    id.prodNo[0] = (dword & 0xF000) >> 12;
    id.prodNo[1] = (dword & 0xF00) >> 8;
    id.prodNo[2] = (dword & 0xF0) >> 4;
    id.prodNo[3] = dword & 0xF;
    return id;
}

void logEISAid(EISAId id) {
    for (uint8_t i = 0; i < 3; i++)
        writeCom(id.mfg[i], 1);
    for (uint8_t i = 0; i < 4; i++) {
        if (id.prodNo[i] < 10)
            writeCom(id.prodNo[i] + 0x30, 1);
        else
            writeCom(id.prodNo[i] + 0x41 - 10, 1);
    }
}

void logPath(AMLName *path, uint8_t length) {
    if (length == 0) {
        kdebug("<пусто>");
        return;
    }
    for (uint8_t i = 0; i < length; i++) {
        AMLName name = *path++;
        if (name == 0) {
            kdebug("<нуль>.", name);
            continue;
        }
        uint8_t *ptr = (uint8_t*)&name;
        for (uint8_t j = 0; j < 4; j++) {
            writeCom(ptr[j], 1);
        }
        kdebug(".");
    }
}

inline void logParsingPath() {
    logPath((AMLName*)parsingPathBase, getParsingPathLen());
}

void clearParsingPath() {
    for (uint8_t i = 0; i < 0x10; i++)
        parsingPath[i] = 0;
    parsingPath = (AMLName*)parsingPathBase;
}

void parseTermList(uint8_t *aml, uint32_t len) {
    for (uint32_t i = 0; i < len; i ++) {
        if (aml[i] == 0x10) {
            kdebug("Найдено определение Scope.\n");
            i ++;
            i += parseScope(aml + i) - 1;
        }
        else if (aml[i] == 0x08) {
            kdebug("Найдено определение Name.\n");
            i ++;
            uint16_t nameLen = parseName(aml + i);
            kdebug("Абсолютный путь к объекту: ");
            logParsingPath();
            kdebug("\n");
            AMLName name = *(parsingPath - 1);
            i += nameLen >> 8;
            uint8_t names = getParsingPathLen();
            *acpiNamespace++ = names;
            memcpy((uint8_t*)parsingPathBase, (uint8_t*)acpiNamespace, names * 4);
            acpiNamespace += names;
            uint32_t dataSize = 0;
            kdebug("Первый байт объекта: %x.\n", aml[i]);
            if (aml[i] == 0x0C) {
                i ++;
                kdebug("Вид объекта - DWordConst; данные = %x", *(uint32_t*)(aml + i));
                if (name == 'DIH_') {
                    kdebug(" (\"");
                    EISAId id = dwordToEISAId(*(uint32_t*)(aml + i));
                    logEISAid(id);
                    kdebug("\")");
                }
                kdebug(".\n");
                *(uint32_t*)acpiData = *(uint32_t*)(aml + i);
                dataSize = 4;
                i += 3;
            } else if (aml[i] == 0x12) {
                i ++;
                kdebug("Вид объекта - Package.\n");
                dataSize = parsePackage(aml + i);
                memcpy((uint8_t*)(aml + i - 1), acpiData, dataSize);
                i += dataSize - 1;
            } else if (aml[i] == 0) {
                kdebug("Значение объекта - 0.\n");
                dataSize = 1;
                *acpiData = 0;
            } else if (aml[i] == 1) {
                kdebug("Значение объекта - 1.\n");
                dataSize = 1;
                *acpiData = 1;
            } else if (aml[i] == 0xFF) {
                kdebug("Значение объекта - 255.\n");
                dataSize = 1;
                *acpiData = 0xFF;
            } else if (aml[i] == 0x11) {
                i ++;
                kdebug("Вид объекта - Buffer.\n");
                dataSize = parseBuffer(aml + i) + 1;
                memcpy((uint8_t*)(aml + i - 1), acpiData, dataSize);
                i += dataSize - 3;
            }
            *acpiNamespace++ = (uint32_t)acpiData;
            acpiData += dataSize;
            for (uint8_t i = 0; i < (nameLen & 0xFF); i++)
                if (getParsingPathLen()) *(--parsingPath) = 0;
        }
        else if (aml[i] == 0x5B) {
            i ++;
            if (aml[i] == 0x82) {
                kdebug("Найдено определение Device.\n");
                i ++;
                i += parseDevice(aml + i) - 2;
            } else if (aml[i] == 0x80) {
                kdebug("Найдено определение OperationRegion.\n");
                i ++;
                i += parseOpRegion(aml + i) - 1;
            }
        }
    }
}

uint16_t parseName(uint8_t *aml) {
    uint8_t bytes = 0;
    if (*aml == 0x2E) {
        aml++;
        bytes ++;
        kdebug("Двойное имя: \"");
        AMLName name1 = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            writeCom(*aml, 1);
            name1 |= (uint32_t)*aml << (8*i);
            bytes ++;
            aml ++;
        }
        kdebug("\", \"");
        AMLName name2 = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            writeCom(*aml, 1);
            name2 |= (uint32_t)(*aml << (8*i));
            bytes ++;
            aml ++;
        }
        kdebug("\".\n");
        *parsingPath++ = name1;
        *parsingPath++ = name2;
        return (bytes << 8) | 2;
    } else if (*aml == 0x2F) {
        aml++;
        bytes ++;
        uint8_t len = *aml++;
        bytes ++;
        AMLName nameSegs[len];
        kdebug("Множественное имя (%d): \"", len);
        for (uint8_t i = 0; i < len; i++) {
            for (uint8_t j = 0; j < 4; j++) {
                if (*aml == 0) {bytes ++; break;}
                writeCom(*aml, 1);
                nameSegs[i] |= (uint32_t)(*aml << (8*j));
                aml ++;
                bytes ++;
            }
            kdebug("\", \"");
            *parsingPath++ = nameSegs[i];
        }
        kdebug("\x7f\x7f\x7f.\n");
        return (bytes << 8) | len;
    } else {
        kdebug("Имя: \"");
        AMLName name = 0;
        for (uint8_t i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            writeCom(*aml, 1);
            name |= (uint32_t)(*aml << (8*i));
            bytes ++;
            aml ++;
        }
        kdebug("\".\n");
        if ((name & 0xFF) == 0x5C)
            clearParsingPath();
        else if (name)
            *parsingPath++ = name;
        return (bytes << 8) | 1;
    }
}

uint32_t parseDevice(uint8_t* aml) {
    uint32_t deviceLen = getPkgLength(aml);
    kdebug("Длина: %d Б.\n", deviceLen);
    deviceLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    uint16_t nameSegs = parseName(aml);
    kdebug("Абсолютный путь к устройству: ");
    deviceLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    logParsingPath();
    kdebug("\n");
    parseTermList(aml, deviceLen);
    for (uint8_t i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return deviceLen;
}

uint32_t parseOpRegion(uint8_t *aml) {
    uint32_t length = 0;
    uint16_t nameSegs = parseName(aml);
    length += nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Абсолютный путь к региону: ");
    logParsingPath();
    kdebug("\n");
    uint8_t regSpace = *aml++;
    length ++;
    kdebug("Пространство региона - ");
    switch(regSpace) {
        case 0: kdebug("ОЗУ.\n"); break;
        case 1: kdebug("I/O.\n"); break;
        case 2: kdebug("конфигурация PCI.\n"); break;
        case 3: kdebug("встроенная шина.\n"); break;
        case 4: kdebug("SMBus.\n"); break;
        case 5: kdebug("CMOS.\n"); break;
        case 6: kdebug("BAR PCI.\n"); break;
        case 7: kdebug("IPMI.\n"); break;
        case 8: kdebug("GPIO.\n"); break;
        case 9: kdebug("общая последовательная шина.\n"); break;
        case 10: kdebug("PCC.\n"); break;
        default: kdebug("не определён.\n"); break;
    }
    uint32_t regOffset = getIntegerTerm(aml);
    kdebug("Сдвиг региона = %x.\n", regOffset);
    length += getIntegerTermBytes(aml);
    aml += getIntegerTermBytes(aml);
    uint32_t regSize = getIntegerTerm(aml);
    kdebug("Размер региона: %d Б.\n", regSize);
    length += getIntegerTermBytes(aml);
    aml += getIntegerTermBytes(aml);
    kdebug("Длина AML-блока региона: %d Б.\n", length);
    for (uint8_t i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return length;
}

uint32_t parseScope(uint8_t *aml) {
    uint32_t scopeLen = getPkgLength(aml);
    kdebug("Длина: %d Б.\n", scopeLen);
    scopeLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    uint16_t nameSegs = parseName(aml);
    kdebug("Длина имени: %d Б.\n", nameSegs >> 8);
    scopeLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Новый путь обработки: ");
    logParsingPath();
    kdebug("\nДлина пути обработки: %d имён\n", getParsingPathLen());
    parseTermList(aml, scopeLen - 1);
    for (uint8_t i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return scopeLen;
}

uint8_t strToPath(const char* str, AMLName* out) {
    AMLName tmp = 0;
    uint8_t shift = 0;
    uint8_t length = 0;
    while (*str != 0) {
        if (*str == '.' || shift == 32) {
            *out++ = tmp;
            tmp = 0;
            shift = 0;
            length ++;
            str ++;
            continue;
        }
        tmp |= (*str) << shift;
        shift += 8;
        str ++;
    }
    if (tmp) {
        *out++ = tmp;
        length ++;
    }
    return length;
}

uint8_t* getACPIVarAddr(const char* strPath) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t length;
    length = strToPath(strPath, path);
    kdebug("Попытка найти адрес переменной ");
    logPath(path, length);
    kdebug("\nДлина пути: %d имён.\n", length);
    uint32_t *names = (uint32_t*)acpiNamespaceBase;
    for (uint32_t i = 0; i < 0x2000; i++) {
        if (names[i] == 0) {
            kdebug("ВНИМАНИЕ: Не удалось найти переменную.\nВозвращается nullptr.\n");
            return nullptr;
        }
        if (names[i] != length) {
            i += length + 1;
            continue;
        }
        i ++;
        if (!memcmp((uint8_t*)(names + i), (uint8_t*)path, length * 4)) {
            i += length;
            continue;
        }
        i += length;
        kdebug("Найдена переменная в пространстве.\nАдрес = %x.\n", names[i]);
        return (uint8_t*)names[i];
    }
    return nullptr;
}

void parseDefBlock(uint8_t *aml) {
    kdebug("Начата обработка блока AML по адресу %x.\n", (uint32_t)aml);

    parsingPath = (AMLName*)parsingPathBase;
    for (uint8_t i = 0; i < 0x10; i++) {
        parsingPath[i] = 0;
    }

    acpiNamespace = (uint32_t*)acpiNamespaceBase;
    for (uint16_t i = 0; i < 0x3C0; i++) {
        acpiNamespace[i] = 0;
    }

    acpiData = (uint8_t*)acpiDataBase;
    for (uint16_t i = 0; i < 0x2000; i++) {
        acpiData[i] = 0;
    }

    acpiFuncs = (uint8_t*)acpiFuncsBase;
    for (uint16_t i = 0; i < 0x2000; i++) {
        acpiFuncs[i] = 0;
    }

    kdebug("Заголовок таблицы:\n");
    kdebug("\tПодпись: \"");
    for (uint8_t i = 0; i < 4; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 4;
    kdebug("\"\n");
    kdebug("\tДлина таблицы: %d Б\n", *(uint32_t*)aml);
    uint32_t len = *(uint32_t*)aml;
    aml += 4;
    kdebug("\tРевизия: %d\n", *aml++);
    kdebug("\tКонтрольная сумма: %x\n", *aml++);
    kdebug("\tИдентификатор OEM: \"");
    for (uint8_t i = 0; i < 6; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 6;
    kdebug("\"\n");
    kdebug("\tИдентификатор таблицы: \"");
    for (uint8_t i = 0; i < 8; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 8;
    kdebug("\"\n");
    kdebug("\tРевизия OEM: %x\n", *(uint32_t*)aml);
    aml += 4;
    kdebug("\tID компилятора ASL: %x\n", *(uint32_t*)aml);
    aml += 4;
    kdebug("\tРевизия компилятора ASL: %x\n", *(uint32_t*)aml);
    aml += 4;
    kdebug("AML начинается с адреса %x.\n", (uint32_t)aml);

    parseTermList(aml, len - 36);
    
    kdebug("Обработка AML успешно завершена.\n");
}