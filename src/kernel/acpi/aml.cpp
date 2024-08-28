#include "aml.hpp"
#include "../str/str.hpp"

const dword parsingPathBase = 0x13000;
AMLName* parsingPath = (AMLName*)parsingPathBase;

const dword scopePathBase = 0x13010;
AMLName* scopePath = (AMLName*)scopePathBase;

const dword bufferedPathBase = 0x13020;
AMLName* bufferedPath = (AMLName*)bufferedPathBase;

const dword acpiNamespaceBase = 0x13100;
dword* acpiNamespace = (dword*)acpiNamespaceBase;

const dword acpiFieldsBase = 0x14000;
byte* acpiFields = (byte*)acpiFieldsBase;

const dword acpiDataBase = 0x14400;
byte* acpiData = (byte*)acpiDataBase;

const dword acpiFuncsBase = 0x16400;
byte* acpiFuncs = (byte*)acpiFuncsBase;

bool acpiNamespaceInit = false;

byte defBlockRevision = 0;

inline byte getPkgBytes(byte *pkg) {
    return (*pkg >> 6) + 1;
}

dword getPkgLength(byte *pkg) {
    byte numBytes = getPkgBytes(pkg);
    if (numBytes == 1) {
        return *pkg & 0x3F;
    }
    dword len = *pkg++ & 0x0F;
    byte shift = 4;
    for (byte i = 1; i < numBytes; i++) {
        len |= (*pkg++) << shift;
        shift += 8;
    }
    return len;
}

byte encodePkgLength(dword length, byte *out) {
    if (length <= 63) {
        *out = length + 1;
        return 1;
    }
    else {
        byte numBytes;
        if (length < 4096)
            numBytes = 1;
        else if (length < 1048576)
            numBytes = 2;
        else
            numBytes = 3;
        length += numBytes + 1;
        *out++ = (numBytes << 6) | (length & 0xF);
        for (byte i = 0; i < numBytes; i++) {
            *out++ = (length >> (4+i*8)) & 0xFF;
        }
        return numBytes + 1;
    }
}

qword getIntegerTerm(byte* aml) {
    qword res;
    switch (*aml) {
        case 0x0A:
            res = *(++aml);
            break;
        case 0x0B:
            res = *(word*)(++aml);
            break;
        case 0x0C:
            res = *(dword*)(++aml);
            break;
        case 0x0E:
            res = *(qword*)(++aml);
            break;
        case 0x01:
            res = 1;
            break;
        case 0x00:
            res = 0;
            break;
        case 0xFF:
            res = 0xFF;
            break;
        default:
            res = 0;
            break;
    }
    return res;
}

byte getIntegerTermBytes(byte* aml) {
    switch (*aml) {
        case 0x0A: return 2;
        case 0x0B: return 3;
        case 0x0C: return 5;
        case 0x0E: return 9;
        case 0x00: return 1;
        case 0x01: return 1;
        case 0xFF: return 1;
    }
    return 0;
}

byte getIntegerTermBytes(qword value) {
    if (value == 0 || value == 1 || value == maxbyte)
        return 1;
    if (value < maxbyte)
        return 2;
    if (value <= maxword)
        return 3;
    if (value <= maxdword)
        return 5;
    if (value <= maxqword)
        return 9;
    return 0;
}

byte encodeIntegerTerm(qword num, byte *ptr) {
    if (num == 0x00 || num == 0x01 || num == 0xFF) {
        *ptr = num & 0xFF;
        return 1;
    }
    else if (num < 0xFF) {
        *ptr++ = 0x0A;
        *ptr = num & 0xFF;
        return 2;
    }
    else if (num <= 0xFFFF) {
        *ptr++ = 0x0B;
        *ptr++ = num & 0xFF;
        *ptr++ = (num >> 8) & 0xFF;
        return 3;
    }
    else if (num <= 0xFFFFFFFF) {
        *ptr++ = 0x0C;
        *ptr++ = num & 0xFF;
        *ptr++ = (num >> 8) & 0xFF;
        *ptr++ = (num >> 16) & 0xFF;
        *ptr++ = (num >> 24) & 0xFF;
        return 5;
    }
    else {
        *ptr++ = 0x0E;
        *ptr++ = num & 0xFF;
        *ptr++ = (num >> 8) & 0xFF;
        *ptr++ = (num >> 16) & 0xFF;
        *ptr++ = (num >> 24) & 0xFF;
        *ptr++ = (num >> 32) & 0xFF;
        *ptr++ = (num >> 40) & 0xFF;
        *ptr++ = (num >> 48) & 0xFF;
        *ptr++ = (num >> 56) & 0xFF;
        return 9;
    }
}

void logPath(AMLName* path, byte len) {
    for (byte i = 0; i < len; i++) {
        logName(path[i]);
        kdebug((byte)'.');
    }
}

EISAId dwordToEISAId(dword num) {
    EISAId id;
    char *ptr = (char*)&num;
    swap(ptr, ptr + 3);
    swap(ptr + 1, ptr + 2);

    id.mfg[0] = ((num & 0x7C000000) >> 26) + 0x40;
    id.mfg[1] = ((num & 0x3E00000) >> 21) + 0x40;
    id.mfg[2] = ((num & 0x1F0000) >> 16) + 0x40;
    id.prodNo[0] = (num & 0xF000) >> 12;
    id.prodNo[1] = (num & 0xF00) >> 8;
    id.prodNo[2] = (num & 0xF0) >> 4;
    id.prodNo[3] = num & 0xF;
    return id;
}

void logEISAid(EISAId id) {
    for (byte i = 0; i < 3; i++)
        writeCom(id.mfg[i], 1);
    for (byte i = 0; i < 4; i++) {
        if (id.prodNo[i] < 10)
            writeCom(id.prodNo[i] + 0x30, 1);
        else
            writeCom(id.prodNo[i] + 0x41 - 10, 1);
    }
}

dword parseBuffer(byte *aml) {
    dword bufLen = getPkgLength(aml);
    kdebug("Длина буфера: %d Б.\n", bufLen);
    aml += getPkgBytes(aml);
    qword numBytes = getIntegerTerm(aml);
    aml += getIntegerTermBytes(aml);
    byte element = 1;
    for (byte i = 0; i < numBytes; i++) {
        word elLen;
        byte type;
        bool isLarge = aml[i] & 0x80;
        if (isLarge) {
            type = aml[i] & 0x7F;
            elLen = *(word*)(aml + i + 1);
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
            byte irq = 0;
            word irqField = *(word*)(aml + i);
            i += 2;
            for (word j = 0; j < 0x10; j++) {
                if (irqField & (1 << j)) {
                    irq = j;
                    break;
                }
            }
            kdebug("\tНомера IRQ: %d", irq);
            for (word j = irq+1; j < 0x10; j++) {
                if (irqField & (1 << j)) {
                    kdebug(", %d", j);
                }
            }
            kdebug("\n");
            if (elLen == 3) {
                byte irqFlags = aml[i];
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
            byte mask = aml[i];
            byte chan;
            for (byte i = 0; i < 8; i++) {
                if (mask & (1 << i)) {
                    chan = i;
                    break;
                }
            }
            i ++;
            kdebug("\tКанал: %d.\n");
            byte flags = aml[i];
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
                byte priority = aml[i];
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
            byte info = aml[i];
            kdebug("\tШирина адреса: ");
            kdebug((info & 1) ? "16 бит\n" : "10 бит\n");
            i ++;
            word minAddr = *(word*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 2;
            word maxAddr = *(word*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 2;
            byte align = aml[i];
            i ++;
            kdebug("\tРовнение: %d Б\n", align);
            byte length = aml[i];
            kdebug("\tДлина пространства: %d Б\n", length);
        } else if (!isLarge && type == 0x09) {
            kdebug("Тип элемента - FixedIO.");
            word addr = *(word*)(aml + i);
            kdebug("\tАдрес: %x\n", addr);
            i += 2;
            byte length = aml[i];
            kdebug("\tДлина пространства: %d Б\n", length);
        } else if (!isLarge && type == 0x0A) {
            kdebug("Тип элемента - FixedDMA.\n");
            word req = *(word*)(aml + i);
            kdebug("\tЛиния запроса: %x\n", req);
            i += 2;
            word chan = *(word*)(aml + i);
            kdebug("\tКанал: %x\n", chan);
            i += 2;
            byte tranWidth = aml[i];
            kdebug("\tШирина передачи: %d бит\n", (1 << tranWidth) * 8);
        } else if (!isLarge && type == 0x0F) {
            kdebug("Тип элемента - End.\n");
            byte checksum = aml[i];
            kdebug("\tКонтрольная сумма: %x\n", checksum);
        } else if (isLarge && type == 0x01) {
            kdebug("Тип элемента - Memory24.\n");
            byte info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            word minAddr = *(word*)(aml + i);
            i += 2;
            kdebug("\tМинимальный адрес: %x\n", (dword)minAddr << 8);
            word maxAddr = *(word*)(aml + i);
            i += 2;
            kdebug("\tМаксимальный адрес: %x\n", (dword)maxAddr << 8);
            word align = *(word*)(aml + i);
            i += 2;
            kdebug("\tРовнение: %d Б\n", align ? align : 65536);
            word range = *(word*)(aml + i);
            i ++;
            kdebug("\tДлина пространства: %d Б\n", (dword)range * 256);
        } else if (isLarge && type == 0x05) {
            kdebug("Тип элемента - Memory32.\n");
            byte info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            dword minAddr = *(dword*)(aml + i);
            i += 4;
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            dword maxAddr = *(dword*)(aml + i);
            i += 4;
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            dword align = *(dword*)(aml + i);
            i += 4;
            kdebug("\tРовнение: %d Б\n", align);
            dword range = *(dword*)(aml + i);
            i += 3;
            kdebug("\tДлина пространства: %d Б\n", range);
        } else if (isLarge && type == 0x06) {
            kdebug("Тип элемента - Memory32Fixed.\n");
            byte info = aml[i];
            kdebug("\tВозможность записи: ");
            kdebug(info ? "Да\n" : "Нет\n");
            i ++;
            dword baseAddr = *(dword*)(aml + i);
            i += 4;
            kdebug("\tОсновный адрес: %x\n", baseAddr);
            dword range = *(dword*)(aml + i);
            i += 3;
            kdebug("\tДлина пространства: %d Б\n", range);
        } else if (isLarge && type == 0x0A) {
            kdebug("Тип элемента - QWordAddrSpace.\n");
            byte info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            byte genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            byte typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            qword granularity = *(qword*)(aml + i);
            kdebug("\tГранулярность: %D Б\n", granularity);
            i += 8;
            qword minAddr = *(qword*)(aml + i);
            kdebug("\tМинимальный адрес: %X\n", minAddr);
            i += 8;
            qword maxAddr = *(qword*)(aml + i);
            kdebug("\tМаксимальный адрес: %X\n", maxAddr);
            i += 8;
            qword traOffset = *(qword*)(aml + i);
            kdebug("\tСдвиг перевода: %X\n", traOffset);
            i += 8;
            qword length = *(qword*)(aml + i);
            kdebug("\tДлина пространства: %D Б\n", length);
            i += 7;
            if (elLen > 43) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (byte j = 0; j < elLen - 43; j ++) {
                    if (aml[i] == 0) break;
                    writeCom(aml[i], 1);
                    i ++;
                }
                kdebug("\"\n");
            }
        } else if (isLarge && type == 0x07) {
            kdebug("Тип элемента - DWordAddrSpace.\n");
            byte info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            byte genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            byte typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            dword granularity = *(dword*)(aml + i);
            kdebug("\tГранулярность: %d Б\n", granularity);
            i += 4;
            dword minAddr = *(dword*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 4;
            dword maxAddr = *(dword*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 4;
            dword traOffset = *(dword*)(aml + i);
            kdebug("\tСдвиг перевода: %x\n", traOffset);
            i += 4;
            dword length = *(dword*)(aml + i);
            kdebug("\tДлина пространства: %d Б\n", length);
            i += 3;
            if (elLen > 23) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (byte j = 0; j < elLen - 23; j ++) {
                    if (aml[i] == 0) break;
                    writeCom(aml[i], 1);
                    i ++;
                }
                kdebug("\"\n");
            }
        } else if (isLarge && type == 0x08) {
            kdebug("Тип элемента - WordAddrSpace.\n");
            byte info = aml[i];
            kdebug("\tТип ресурса: ");
            switch (info) {
                case 0: kdebug("ОЗУ\n"); break;
                case 1: kdebug("IO\n"); break;
                case 2: kdebug("Номер шины\n"); break;
                default: kdebug("<ошибка>\n"); break;
            }
            i ++;
            byte genFlags = aml[i];
            kdebug("\tТип расшифровки: ");
            kdebug((genFlags & 2) ? "Вычитание\n" : "Сложение (?)\n");
            kdebug("\tФиксирован верхний адрес: ");
            kdebug((genFlags & 8) ? "Да\n" : "Нет\n");
            kdebug("\tФиксирован нижний адрес: ");
            kdebug((genFlags & 4) ? "Да\n" : "Нет\n");
            i ++;
            byte typeFlags = aml[i];
            i ++;
            kdebug("\tФлаги ресурса: %b\n", typeFlags);
            word granularity = *(word*)(aml + i);
            kdebug("\tГранулярность: %d Б\n", granularity);
            i += 2;
            word minAddr = *(word*)(aml + i);
            kdebug("\tМинимальный адрес: %x\n", minAddr);
            i += 2;
            word maxAddr = *(word*)(aml + i);
            kdebug("\tМаксимальный адрес: %x\n", maxAddr);
            i += 2;
            word traOffset = *(word*)(aml + i);
            kdebug("\tСдвиг перевода: %x\n", traOffset);
            i += 2;
            word length = *(word*)(aml + i);
            kdebug("\tДлина пространства: %d Б\n", length);
            i ++;
            if (elLen > 13) {
                i ++;
                kdebug("\tИндекс источника: %d\n", aml[i]);
                i ++;
                kdebug("\tИсточник: \"");
                for (byte j = 0; j < elLen - 13; j ++) {
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

dword parseVarPackage(byte *aml) {
    kdebug("TODO: Обработка VarPackage\n");
    return getPkgLength(++aml);
}

dword parsePackage(byte *aml) {
    dword pkgLen = getPkgLength(aml);
    aml += getPkgBytes(aml);
    kdebug("Длина пакета: %d Б.\n", pkgLen);
    byte numElements = *aml++;
    kdebug("Число элементов: %d.\n", numElements);
    for (byte i = 0; i < numElements; i++) {
        kdebug("Вид элемента %d - ", i+1);
        if (*aml == 0x0A) {
            kdebug("ByteConst.\nЗначение = %x.\n", *(++aml));
            aml ++;
        } else if (*aml == 0x0B) {
            kdebug("WordConst.\nЗначение = %x.\n", *(word*)(++aml));
            aml += 2;
        } else if (*aml == 0x0C) {
            kdebug("DWordConst.\nЗначение = %x.\n", *(dword*)(++aml));
            aml += 4;
        } else if (*aml == 0x0E) {
            kdebug("QWordConst.\nЗначение = %x.\n", *(qword*)(++aml));
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
            for (byte i = 0; i < 4; i++) {
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

word parseName(byte *aml) {
    byte bytes = 0;
    if (*aml == 0x2E) {
        aml++;
        bytes ++;
        kdebug("Двойное имя: \"");
        AMLName name1 = 0;
        for (byte i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == 0)
                writeCom(*aml, 1);
            name1 |= (dword)*aml << (8*i);
            bytes ++;
            aml ++;
        }
        kdebug("\", \"");
        AMLName name2 = 0;
        for (byte i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == 0)
                writeCom(*aml, 1);
            name2 |= (dword)(*aml << (8*i));
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
        byte len = *aml++;
        bytes ++;
        AMLName seg = 0;
        kdebug("Множественное имя (%d): \"", len);
        for (byte j = 0; j < 4; j++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || j == 0)
                writeCom(*aml, 1);
            seg |= (dword)(*aml << (8*j));
            aml ++;
            bytes ++;
        }
        *parsingPath++ = seg;
        seg = 0;
        for (byte i = 1; i < len; i++) {
            kdebug("\", \"");
            for (byte j = 0; j < 4; j++) {
                if (*aml == 0) {bytes ++; break;}
                if (*aml != '_' || j == 0)
                    writeCom(*aml, 1);
                seg |= (dword)((*aml) << (8*j));
                aml ++;
                bytes ++;
            }
            *parsingPath++ = seg;
            seg = 0;
        }
        kdebug("\".\n");
        return (bytes << 8) | len;
    } else {
        kdebug("Имя: \"");
        AMLName name = 0;
        for (byte i = 0; i < 4; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == 0)
                writeCom(*aml, 1);
            name |= (dword)(*aml << (8*i));
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

dword parseDevice(byte* aml) {
    dword deviceLen = getPkgLength(aml);
    kdebug("Длина: %d Б.\n", deviceLen);
    deviceLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к устройству: ");
    deviceLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    logParsingPath();
    kdebug("\n");
    parseTermList(aml, deviceLen);
    for (byte i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return deviceLen;
}

dword parseOpRegion(byte *aml) {
    dword length = 0;
    word nameSegs = parseName(aml);
    dword start = (dword)aml;
    *acpiNamespace++ = getParsingPathLen();
    memcpy((byte*)parsingPathBase, (byte*)acpiNamespace, getParsingPathLen() * 4);
    acpiNamespace += getParsingPathLen();
    *acpiNamespace++ = (dword)acpiData;
    length += nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Абсолютный путь к региону: ");
    logParsingPath();
    kdebug("\n");
    byte regSpace = *aml++;
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
        default: kdebug("<%x>.\n", regSpace); break;
    }
    dword regOffset = getIntegerTerm(aml);
    kdebug("Сдвиг региона = %x.\n", regOffset);
    length += getIntegerTermBytes(aml);
    aml += getIntegerTermBytes(aml);
    dword regSize = getIntegerTerm(aml);
    kdebug("Размер региона: %d Б.\n", regSize);
    length += getIntegerTermBytes(aml);
    aml += getIntegerTermBytes(aml);
    kdebug("Длина AML-блока региона: %d Б.\n", length);
    *(word*)acpiData = 0x805B;
    memcpy((byte*)start + (nameSegs >> 8), acpiData + 2, length - (nameSegs >> 8));
    acpiData += length + 2 - (nameSegs >> 8);
    for (byte i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return length;
}

dword parseField(byte *aml) {
    dword amlLength = getPkgLength(aml);
    aml += getPkgBytes(aml);
    kdebug("Длина: %d Б.\n", amlLength);
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к региону поля: ");
    logParsingPath();
    kdebug("\n");
    byte *opReg = getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen());
    opReg += 2;
    byte regSpace = *opReg++;
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
        default: kdebug("<%x>.\n", regSpace); break;
    }
    qword offset = getIntegerTerm(opReg) * 8;
    aml += nameSegs >> 8;
    byte flags = *aml++;
    kdebug("Параметры поля:\n\tВид доступа: ");
    switch (flags & 0xF) {
        case 0: kdebug("Любой\n"); break;
        case 1: kdebug("Байт\n"); break;
        case 2: kdebug("Слово\n"); break;
        case 3: kdebug("Двойное слово\n"); break;
        case 4: kdebug("Четверное слово\n"); break;
        case 5: kdebug("Буфер\n"); break;
        default: kdebug("<%x>\n", flags & 0xF); break;
    }
    kdebug("\tБлокировка: ");
    kdebug(flags & 16 ? "Да\n" : "Нет\n");
    kdebug("\tПравило обновления: ");
    switch ((flags & 96) >> 5) {
        case 0: kdebug("Сохранять\n"); break;
        case 1: kdebug("Записывать единицами\n"); break;
        case 2: kdebug("Записывать нулями\n"); break;
        default: kdebug("<%x>\n", (flags & 96) >> 5); break;
    }
    word fieldNo = 1;
    for (dword i = 0; i < amlLength - (nameSegs >> 8) - 1; i++) {
        dword bits = 0;
        dword fieldLen = 0;
        if (aml[i] == 0) {
            kdebug("Тип поля %d - резервированное.\n", fieldNo);
            i ++;
            bits = getPkgLength(aml + i);
            kdebug("\tДлина поля: %d бит\n", bits);
            i += getPkgBytes(aml + i) - 1;
        } else if (isLeadNameChar(aml[i])) {
            kdebug("Тип поля %d - названное.\n", fieldNo);
            dword nameSeg = *(dword*)(aml + i);
            kdebug("\tИмя поля: \"");
            kdebug(nameSeg);
            kdebug("\"\n");
            i += 4;
            bits = aml[i];
            kdebug("\tРазмер поля: %d бит\n", bits);
            if (bits % 8 == 0)
                bits /= 8;
            else
                bits |= 0x80;
            *acpiFields = 0xFD;
            fieldLen ++;
            *(acpiFields+1) = flags;
            fieldLen ++;
            *(acpiFields+2) = regSpace;
            fieldLen ++;
            if (offset % 8 == 0)
                fieldLen += encodeIntegerTerm(offset / 8, acpiFields + fieldLen);
            else
                fieldLen += encodeIntegerTerm(offset, acpiFields + fieldLen);
            if (offset % 8 == 0)
                fieldLen += encodeIntegerTerm(bits / 8, acpiFields + fieldLen);
            else
                fieldLen += encodeIntegerTerm(bits | 0x80, acpiFields + fieldLen);
            kdebug("Поле записано в адрес %x.\n", acpiFields);
            *acpiNamespace++ = 1;
            *acpiNamespace++ = nameSeg;
            *acpiNamespace++ = (dword)acpiFields;
            acpiFields += fieldLen;
        }
        fieldNo++;
        offset += bits / 8;
    }
    for (byte i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return amlLength;
}

dword parseScope(byte *aml) {
    dword scopeLen = getPkgLength(aml);
    kdebug("Длина: %d Б.\n", scopeLen);
    scopeLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    kdebug("Длина имени: %d Б.\n", nameSegs >> 8);
    scopeLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Новый путь обработки: ");
    logParsingPath();
    kdebug("\nДлина пути обработки: %d имён\n", getParsingPathLen());
    parseTermList(aml, scopeLen - 1);
    for (byte i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return scopeLen;
}

dword parseMethod(byte *aml) {
    dword methodLen = getPkgLength(aml);
    dword addr = (dword)acpiFuncs;
    *acpiFuncs++ = 0x14;
    methodLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    aml += nameSegs >> 8;
    methodLen -= nameSegs >> 8;
    acpiFuncs += encodeIntegerTerm(methodLen, acpiFuncs);
    memcpy(aml, acpiFuncs, methodLen);
    kdebug("Абсолютный путь к методу: ");
    logParsingPath();
    byte flags = *aml++;
    kdebug("\n\tЧисло аргументов: %d\n", flags & 7);
    kdebug("\tСериализация: ");
    kdebug((flags & 8) ? "Да\n" : "Нет\n");
    kdebug("\tУровень синхронизации: %x\n", (flags & 0xF0) >> 4);
    byte names = getParsingPathLen();
    *acpiNamespace++ = names;
    memcpy((byte*)parsingPathBase, (byte*)acpiNamespace, names * 4);
    acpiNamespace += names;
    *(dword*)acpiNamespace = addr;
    acpiNamespace++;
    acpiFuncs += methodLen;
    kdebug("Метод сохранён в %x.\n", addr);
    for (byte i = 0; i < (nameSegs & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return methodLen;
}

byte strToPath(const char* str, AMLName* out) {
    AMLName tmp = 0;
    byte shift = 0;
    byte length = 0;
    while (*str != 0) {
        if (*str == '.' || shift == 32) {
            while (shift < 32) {
                tmp |= (byte)'_' << shift;
                shift += 8;
            }
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
        while (shift < 32) {
            tmp |= (byte)'_' << shift;
            shift += 8;
        }
        *out++ = tmp;
        length ++;
    }
    return length;
}

dword nameACPIObj(byte* aml) {
    dword addr = (dword)aml;
    word nameLen = parseName(aml);
    kdebug("Абсолютный путь к объекту: ");
    logParsingPath();
    kdebug("\n");
    if (getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen())) {
        kdebug("ВНИМАНИЕ: Объект с таким названием уже существует\n");
        for (byte i = 0; i < (nameLen & 0xFF); i++)
            if (getParsingPathLen()) *(--parsingPath) = 0;
        return getACPIObjLen((AMLName*)parsingPathBase, getParsingPathLen()) + (nameLen >> 8);
    }
    AMLName name = *(parsingPath - 1);
    aml += nameLen >> 8;
    byte names = getParsingPathLen();
    *acpiNamespace++ = names;
    memcpy((byte*)parsingPathBase, (byte*)acpiNamespace, names * 4);
    acpiNamespace += names;
    dword dataSize = 0;
    kdebug("Первый байт объекта: %x.\n", *aml);
    if (*aml == 0x0A) {
        aml ++;
        kdebug("Вид объекта - ByteConst; данные = %x.\n", *aml);
        *acpiData++ = 0x0A;
        *acpiData-- = *aml;
        dataSize = 2;
    }
    else if (*aml == 0x0B) {
        aml ++;
        word val = *(word*)aml;
        kdebug("Вид объекта - WordConst; данные = %x.\n", val);
        *acpiData++ = 0x0B;
        *(word*)acpiData = val;
        acpiData--;
        dataSize = 3;
        aml ++;
    }
    else if (*aml == 0x0C) {
        aml ++;
        dword val = *(dword*)aml;
        kdebug("Вид объекта - DWordConst; данные = %x", val);
        if (name == 'DIH_') {
            kdebug(" (\"");
            EISAId id = dwordToEISAId(val);
            logEISAid(id);
            kdebug("\")");
        }
        kdebug(".\n");
        *acpiData++ = 0x0C;
        *(dword*)acpiData = val;
        acpiData --;
        dataSize = 5;
        aml += 3;
    } else if (*aml == 0x0E) {
        aml ++;
        qword val = *(qword*)aml;
        kdebug("Вид объекта - QWordConst; данные = %X.\n", val);
        *acpiData++ = 0x0E;
        *(qword*)acpiData = val;
        acpiData--;
        aml += 7;
        dataSize = 9;
    } else if (*aml == 0x0D) {
        aml ++;
        kdebug("Вид объекта - String; данные = \"");
        dword length = 1;
        *acpiData++ = 0x0D;
        while (*aml != 0) {
            kdebug(*aml);
            *acpiData++ = *aml++;
            length ++;
        }
        *acpiData++ = 0;
        length ++;
        acpiData -= length;
        aml++;
        kdebug("\".\n");
        dataSize = length;
    } else if (*aml == 0x12) {
        aml ++;
        kdebug("Вид объекта - Package.\n");
        dataSize = parsePackage(aml);
        memcpy((byte*)(aml - 1), acpiData, dataSize);
        aml += dataSize - 1;
    } else if (*aml == 0) {
        kdebug("Значение объекта - 0.\n");
        dataSize = 1;
        *acpiData = 0;
    } else if (*aml == 1) {
        kdebug("Значение объекта - 1.\n");
        dataSize = 1;
        *acpiData = 1;
    } else if (*aml == 0xFF) {
        kdebug("Значение объекта - 255.\n");
        dataSize = 1;
        *acpiData = 0xFF;
    } else if (*aml == 0x11) {
        aml ++;
        kdebug("Вид объекта - Buffer.\n");
        dataSize = parseBuffer(aml) + 1;
        memcpy((byte*)(aml - 1), acpiData, dataSize);
        aml += dataSize - 2;
    }
    *acpiNamespace++ = (dword)acpiData;
    acpiData += dataSize;
    for (byte i = 0; i < (nameLen & 0xFF); i++)
        if (getParsingPathLen()) *(--parsingPath) = 0;
    return (dword)aml - addr;
}

byte* getACPIObjAddr(const char* strPath) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length;
    length = strToPath(strPath, path);
    return getACPIObjAddr(path, length);
}

byte* getACPIObjAddr(AMLName* path, byte length) {
    kdebug("Попытка найти адрес объекта ");
    logPath(path, length);
    kdebug("\nДлина пути: %d имён.\n", length);
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x2000; i += 0) {
        if (names[i] == 0) {
            kdebug("ВНИМАНИЕ: Не удалось найти объект.\nВозвращается nullptr.\n");
            return nullptr;
        }
        if (names[i] != length) {
            i += names[i] + 2;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 1;
            continue;
        }
        
        i += length;
        kdebug("Найден объект в пространстве.\nАдрес = %x.\n", names[i]);
        return (byte*)names[i];
    }
    return nullptr;
}

dword getACPIObjLen(AMLName* path, byte length) {
    kdebug("Попытка найти длину объекта ");
    logPath(path, length);
    kdebug("\nДлина пути: %d имён.\n", length);
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x2000; i += 0) {
        if (names[i] == 0) {
            kdebug("ВНИМАНИЕ: Не удалось найти объект.\nВозвращается 0.\n");
            return 0;
        }
        if (names[i] != length) {
            i += names[i] + 2;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 1;
            continue;
        }
        i += length;
        dword startAddr = names[i];
        kdebug("Найден объект в пространстве.\nАдрес = %x.\n", startAddr);
        dword endAddr = 0;
        do {
            i++;
            i += names[i] + 1;
            endAddr = names[i];
        } while (endAddr < startAddr);
        kdebug("Адрес следующего объекта = %x.\n", endAddr);
        kdebug("Длина объекта - %d Б.", endAddr - startAddr);
        return endAddr - startAddr;
    }
    return 0;
}

dword getACPIObjLen(byte *ptr) {
    kdebug("Попытка определить длину объекта по адресу %x.", ptr);
    byte id = *ptr;
    if (id >= 0x0A && id <= 0x0C || id == 0x0E) {
        byte len = getIntegerTermBytes(ptr);
        kdebug("Тип объекта - IntegerTerm.\n");
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    ptr ++;
    if (id == 0x0D) {
        kdebug("Тип объекта - строка.\n");
        byte len = strlen((char*)ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x11) {
        kdebug("Тип объекта - Buffer.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x12) {
        kdebug("Тип объекта - Package.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x13) {
        kdebug("Тип объекта - VarPackage.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
}

void remapACPIObj(const char* strPath, byte *newPtr) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length;
    length = strToPath(strPath, path);
    remapACPIObj(path, length, newPtr);
}

void remapACPIObj(AMLName* path, byte length, byte *newPtr) {
    kdebug("Попытка изменить адрес объекта ");
    logPath(path, length);
    kdebug("\nДлина пути: %d имён.\n", length);
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x2000; i += 0) {
        if (names[i] == 0) {
            kdebug("ВНИМАНИЕ: Не удалось найти объект.\n");
            return;
        }
        if (names[i] != length) {
            i += names[i] + 2;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 1;
            continue;
        }
        i += length;
        kdebug("Найден объект в пространстве.\nАдрес = %x.\n", names[i]);
        names[i] = (dword)newPtr;
        kdebug("Адрес изменён на %x.\n", names[i]);
    }
}

dword getACPIObjLen(const char* strPath) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length;
    length = strToPath(strPath, path);
    return getACPIObjLen(path, length);
}

void parseTermList(byte *aml, dword len) {
    for (dword i = 0; i < len; i++) {
        if (aml[i] == 0x10) {
            kdebug("Найдено определение Scope.\n");
            i ++;
            i += parseScope(aml + i) - 1;
        }
        else if (aml[i] == 0x08) {
            kdebug("Найдено определение Name.\n");
            i ++;
            i += nameACPIObj(aml + i);
        }
        else if (aml[i] == 0x14) {
            kdebug("Найдено определение Method.\n");
            i ++;
            i += parseMethod(aml + i) - 1;
        }
        else if (aml[i] == 0x5B) {
            i ++;
            if (aml[i] == 0x80) {
                kdebug("Найдено определение OperationRegion.\n");
                i ++;
                dword regSize = parseOpRegion(aml + i);
                i += regSize - 1;
            }
            else if (aml[i] == 0x81) {
                kdebug("Найдено определение Field.\n");
                i ++;
                i += parseField(aml + i) - 1;
            }
            else if (aml[i] == 0x82) {
                kdebug("Найдено определение Device.\n");
                i ++;
                i += parseDevice(aml + i) - 2;
            }
        }
    }
}

void parseDefBlock(byte *aml) {
    kdebug("Начата обработка блока AML по адресу %x.\n", (dword)aml);

    if (!acpiNamespaceInit) {
        kdebug("Расчистка места.\n");
        for (byte i = 0; i < 0x10; i++) {
            parsingPath[i] = 0;
        }

        for (word i = 0; i < 0x3C0; i++) {
            acpiNamespace[i] = 0;
        }

        for (word i = 0; i < 0x400; i++) {
            acpiFields[i] = 0;
        }

        for (word i = 0; i < 0x2000; i++) {
            acpiData[i] = 0;
        }

        for (word i = 0; i < 0x2000; i++) {
            acpiFuncs[i] = 0;
        }
        createPages(0x700000, 0x700000, 4);
        createPages(0x710000, 0x710000, 4);

        acpiNamespaceInit = true;
    }

    kdebug("Заголовок таблицы:\n");
    kdebug("\tПодпись: \"");
    for (byte i = 0; i < 4; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 4;
    kdebug("\"\n");
    kdebug("\tДлина таблицы: %d Б\n", *(dword*)aml);
    dword len = *(dword*)aml;
    aml += 4;
    defBlockRevision = *aml++;
    kdebug("\tРевизия: %d\n", defBlockRevision);
    kdebug("\tКонтрольная сумма: %x\n", *aml++);
    kdebug("\tИдентификатор OEM: \"");
    for (byte i = 0; i < 6; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 6;
    kdebug("\"\n");
    kdebug("\tИдентификатор таблицы: \"");
    for (byte i = 0; i < 8; i++) {
        if (aml[i] != 0)
            writeCom(aml[i], 1);
    }
    aml += 8;
    kdebug("\"\n");
    kdebug("\tРевизия OEM: %x\n", *(dword*)aml);
    aml += 4;
    kdebug("\tID компилятора ASL: %x\n", *(dword*)aml);
    aml += 4;
    kdebug("\tРевизия компилятора ASL: %x\n", *(dword*)aml);
    aml += 4;
    kdebug("AML начинается с адреса %x.\n", (dword)aml);

    parseTermList(aml, len - 36);
    
    kdebug("Длина пространства имён: %d Б.\n", (dword)acpiNamespace - acpiNamespaceBase);
    kdebug("Длина пространства полей: %d Б.\n", (dword)acpiFields - acpiFieldsBase);
    kdebug("Длина пространства данных: %d Б.\n", (dword)acpiData - acpiDataBase);
    kdebug("Длина пространства функций: %d Б.\n", (dword)acpiFuncs - acpiFuncsBase);
    kdebug("Обработка AML успешно завершена.\n");
}

qword callMethod(const char* strPath, ...) {
    va_list args;
    va_start(args, strPath);
    kdebug("\nВызван метод \"");
    kdebug(strPath);
    kdebug("\".\n");
    storeParsingPath();
    parsingPath = (AMLName*)parsingPathBase;
    byte len = strToPath(strPath, parsingPath);
    parsingPath += len;
    byte *code = getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen());
    kdebug("Адрес метода: %x.\n", code);
    code++;
    dword length = getIntegerTerm(code);
    kdebug("Длина метода: %d Б.\n", length);
    code += getIntegerTermBytes(code);
    byte argc = *code & 7;
    kdebug("Число аргументов: %d.\n", argc);
    qword retVal = runMethod(code, length, argc, args);
    restoreParsingPath();
    return retVal;
}