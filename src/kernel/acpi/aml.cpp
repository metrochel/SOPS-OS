#include "aml.hpp"
#include "../str/str.hpp"
#include "../dbg/dbg.hpp"
#include "../graphics/glyphs.hpp"
#include <stdarg.h>

const dword parsingPathBase = 0x13000;
AMLName* parsingPath = (AMLName*)parsingPathBase;

const dword varPathBase = 0x13040;
AMLName* varPath = (AMLName*)varPathBase;

const dword bufferedPathBase = 0x13080;
AMLName* bufferedPath = (AMLName*)bufferedPathBase;

const dword acpiNamespaceBase = 0x13200;
dword* acpiNamespace = (dword*)acpiNamespaceBase;

const dword acpiFieldsBase = 0x15200;
byte* acpiFields = (byte*)acpiFieldsBase;

const dword acpiDataBase = 0x15600;
byte* acpiData = (byte*)acpiDataBase;

const dword acpiFuncsBase = 0x18600;
byte* acpiFuncs = (byte*)acpiFuncsBase;

const dword acpiReEvalBase = 0x12C00;
dword* acpiReEval = (dword*)acpiReEvalBase;

bool acpiNamespaceInit = false;
bool reEvaluating = false;
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
    if (!len) {
        kdebug("<пусто>.");
        return;
    }
    for (byte i = 0; i < len; i++) {
        logName(path[i]);
        kdebug((byte)'.');
    }
}

void storeParsingPath() {
    byte len = getParsingPathLen();
    AMLName *addr = bufferedPath;
    AMLName *ptr = (AMLName*)parsingPathBase;
    for (byte i = 0; i < len; i++) {
        *bufferedPath++ = ptr[i];
    }
    *bufferedPath++ = (dword)len;
}

void restoreParsingPath() {
    byte len = *(--bufferedPath);
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < len; i++) {
        parsingPath[len - i - 1] = *(--bufferedPath);
        *bufferedPath = 0;
    }
    parsingPath += len;
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
        kdebug((byte)id.mfg[i]);
    for (byte i = 0; i < 4; i++) {
        if (id.prodNo[i] < 10)
            kdebug((byte)(id.prodNo[i] + 0x30));
        else
            kdebug((byte)(id.prodNo[i] + 0x41 - 10));
    }
}

dword parseBuffer(byte *aml) {
    if (*aml++ == 0x11)
        aml += getPkgBytes(aml);
    dword numBytes = getIntegerTerm(aml);
    kdebug("Длина буфера: %d Б.\n", numBytes);
    aml += getIntegerTermBytes(aml);
    byte element = 1;
    for (dword i = 0; i < numBytes; i++) {
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
            kdebugnewl();
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
                    if (aml[j] == 0) break;
                    kdebug(aml[j]);
                    j ++;
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
                    kdebug(aml[i]);
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
                    kdebug(aml[i]);
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
    return numBytes;
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
        } else if (isLeadNameChar(*aml)){
            kdebug("ссылка.\n");
            word nameSegs = parseName(aml);
            aml += nameSegs >> 8;
        } else {
            kdebug("не определён.\nВНИМАНИЕ: Не удалось определить тип элемента %d.\nРовнение, вероятнее всего, сбито.\n", i+1);
            break;
        }
    }
    return pkgLen;
}

word parseName(byte *aml) {
    varPath = (AMLName*)varPathBase;
    memcpy((byte*)parsingPathBase, (byte*)varPath, getParsingPathLen() * 4);
    varPath += getParsingPathLen();
    byte bytes = 0;
    if (*aml == 0x5C) {
        clearVarPath();
        aml ++;
        bytes ++;
    }
    if (*aml == 0x2E) {
        aml++;
        bytes ++;
        kdebug("Двойное имя: \"");
        if (!getVarPathLen())
            kdebug((byte)0x5C);
        AMLName name1 = 0;
        byte prefixes = 0;
        for (byte i = 0; i < 4 + prefixes; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == prefixes)
                kdebug(*aml);
            if (*aml == '^') {
                varPath --;
                prefixes++;
            }
            else {
                name1 |= (dword)*aml << (8*i);
            }
            bytes ++;
            aml ++;
        }
        kdebug("\", \"");
        AMLName name2 = 0;
        prefixes = 0;
        for (byte i = 0; i < 4 + prefixes; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == prefixes)
                kdebug(*aml);
            if (*aml == '^') {
                varPath --;
                prefixes++;
            }
            else {
                name2 |= (dword)*aml << (8*i);
            }
            bytes ++;
            aml ++;
        }
        kdebug("\".\n");
        *varPath++ = name1;
        *varPath++ = name2;
        return (bytes << 8) | 2;
    } else if (*aml == 0x2F) {
        aml++;
        bytes ++;
        byte len = *aml++;
        bytes ++;
        AMLName seg = 0;
        kdebug("Множественное имя (%d): \"", len);
        if (!getVarPathLen())
            kdebug((byte)0x5C);
        byte prefixes = 0;
        for (byte j = 0; j < 4 + prefixes; j++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || j == prefixes)
                kdebug(*aml);
            if (*aml == '^') {
                varPath --;
                prefixes++;
            }
            else {
                seg |= (dword)*aml << (8*j);
            }
            bytes ++;
            aml ++;
        }
        *varPath++ = seg;
        seg = 0;
        for (byte i = 1; i < len; i++) {
            kdebug("\", \"");
            for (byte j = 0; j < 4 + prefixes; j++) {
                if (*aml == 0) {bytes ++; break;}
                if (*aml != '_' || i == prefixes)
                    kdebug(*aml);
                if (*aml == '^') {
                    varPath --;
                    prefixes++;
                } else {
                    seg |= (dword)*aml << (8*i);
                }
                bytes ++;
                aml ++;
            }
            *varPath++ = seg;
            seg = 0;
            prefixes = 0;
        }
        kdebug("\".\n");
        return (bytes << 8) | len;
    } else {
        kdebug("Имя: \"");
        if (!getVarPathLen())
            kdebug((byte)0x5C);
        AMLName name = 0;
        byte prefixes = 0;
        for (byte i = 0; i < 4 + prefixes; i++) {
            if (*aml == 0) {bytes ++; break;}
            if (*aml != '_' || i == prefixes)
                kdebug(*aml);
            if (*aml == '^') {
                varPath --;
                prefixes++;
            }
            else {
                name |= (dword)*aml << (8*i);
            }
            bytes ++;
            aml ++;
        }
        kdebug("\".\n");
        if ((name & 0xFF) == 0x5C)
            clearVarPath();
        else if (name)
            *varPath++ = name;
        return (bytes << 8) | 1;
    }
}

word getName(byte *aml) {
    word nameSegs = parseName(aml);
    AMLName name = *(--varPath);
    varPath++;
    while ((dword)varPath > varPathBase) {
        if (getACPIObjAddr((AMLName*)varPathBase, getVarPathLen()))
            return nameSegs;
        varPath --;
        *(--varPath) = name;
        varPath ++;
    }
    return nameSegs;
}

dword parseDevice(byte* aml) {
    storeParsingPath();
    dword deviceLen = getPkgLength(aml);
    dword _len;
    _len = deviceLen;
    kdebug("Длина: %d Б.\n", deviceLen);
    deviceLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к устройству: ");
    logVarPath();
    kdebugnewl();
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < getVarPathLen(); i++)
        *parsingPath++ = *(AMLName*)(varPathBase + 4*i);
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, getVarPathLen() * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace = (dword)acpiData;
    *acpiData++ = 0x82;
    *(dword*)acpiData = (dword)(acpiData + 4);
    acpiData += 4;
    kdebug("Устройство записано в адрес %x.\n", *acpiNamespace++);
    *acpiNamespace++ = 5;
    deviceLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    parseTermList(aml, deviceLen);
    restoreParsingPath();
    return _len;
}

dword parseThermalZone(byte* aml) {
    storeParsingPath();
    dword tzLen = getPkgLength(aml);
    dword _len;
    _len = tzLen;
    kdebug("Длина: %d Б.\n", tzLen);
    tzLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к термозоне: ");
    logVarPath();
    kdebugnewl();
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < getVarPathLen(); i++)
        *parsingPath++ = *(AMLName*)(varPathBase + 4*i);
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, getVarPathLen() * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace = (dword)acpiData;
    *acpiData++ = 0x85;
    *(dword*)acpiData = (dword)(acpiData + 4);
    acpiData += 4;
    kdebug("Термозона записана в адрес %x.\n", *acpiNamespace++);
    *acpiNamespace++ = 5;
    tzLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    parseTermList(aml, tzLen);
    restoreParsingPath();
    return _len;
}

dword parseOpRegion(byte *aml) {
    dword length = 0;
    word nameSegs = parseName(aml);
    dword start = (dword)aml;
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, getVarPathLen() * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace++ = (dword)acpiData;
    length += nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Абсолютный путь к региону: ");
    logVarPath();
    kdebugnewl();
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
    acpiData += 2;
    length -= nameSegs >> 8;
    memcpy((byte*)start + (nameSegs >> 8), acpiData, length);
    acpiData += length;
    *acpiNamespace++ = length + 2;
    length += nameSegs >> 8;
    return length;
}

dword parseField(byte *aml) {
    dword amlLength = getPkgLength(aml);
    aml += getPkgBytes(aml);
    kdebug("Длина: %d Б.\n", amlLength);
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к региону поля: ");
    logVarPath();
    kdebugnewl();
    byte *opReg = getACPIObjAddr((AMLName*)varPathBase, getVarPathLen());
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
    qword offset = getIntegerTerm(opReg);
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
    dword bitsOffset = 0;
    dword fieldsBlock = amlLength - (nameSegs >> 8) - getPkgBytes(amlLength) - 1;
    for (dword i = 0; i < fieldsBlock; i++) {
        dword bits = 0;
        dword fieldLen = 0;
        if (aml[i] == 0) {
            kdebug("Тип поля %d - резервированное.\n", fieldNo);
            i ++;
            bits = getPkgLength(aml + i);
            kdebug("\tДлина поля: %d бит\n", bits);
            i += getPkgBytes(aml + i);
        } else if (isLeadNameChar(aml[i])) {
            kdebug("Тип поля %d - названное.\n", fieldNo);
            word nameSegs = parseName(aml + i);
            i += nameSegs >> 8;
            kdebug("Абсолютный путь к полю: ");
            logVarPath();
            kdebugnewl();
            bits = getPkgLength(aml + i);
            i += getPkgBytes(aml + i);
            kdebug("\tДлина поля: %d бит\n", bits);
            dword addr = (dword)acpiFields;
            *acpiNamespace++ = getVarPathLen();
            memcpy((byte*)varPathBase, (byte*)acpiNamespace, getVarPathLen() * 4);
            acpiNamespace += getVarPathLen();
            *acpiNamespace++ = (dword)acpiFields;
            *acpiFields++ = 0xFD;
            *acpiFields++ = flags;
            *acpiFields++ = regSpace;
            acpiFields += encodeIntegerTerm(offset, acpiFields);
            acpiFields += encodeIntegerTerm(bitsOffset, acpiFields);
            acpiFields += encodeIntegerTerm(bits, acpiFields);
            *acpiNamespace++ = (dword)acpiFields - addr;
        }
        fieldNo++;
        bitsOffset += bits;
        i--;
    }
    return amlLength;
}

dword parseScope(byte *aml) {
    storeParsingPath();
    dword scopeLen = getPkgLength(aml);
    dword _len = scopeLen;
    kdebug("Длина: %d Б.\n", scopeLen);
    scopeLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    kdebug("Длина имени: %d Б.\n", nameSegs >> 8);
    scopeLen -= nameSegs >> 8;
    aml += nameSegs >> 8;
    kdebug("Новый путь обработки: ");
    logVarPath();
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < getVarPathLen(); i++)
        *parsingPath++ = *(AMLName*)(varPathBase + 4*i);
    kdebug("\nДлина пути обработки: %d имён\n", getParsingPathLen());
    parseTermList(aml, scopeLen - 1);
    restoreParsingPath();
    return _len;
}

dword parseMethod(byte *aml) {
    dword methodLen = getPkgLength(aml);
    dword _len = methodLen;
    dword addr = (dword)acpiFuncs;
    *acpiFuncs++ = 0x14;
    byte pkgBytes = getPkgBytes(aml);
    methodLen -= getPkgBytes(aml);
    aml += getPkgBytes(aml);
    word nameSegs = parseName(aml);
    aml += nameSegs >> 8;
    methodLen -= nameSegs >> 8;
    acpiFuncs += encodeIntegerTerm(methodLen, acpiFuncs);
    dword _methodLen = methodLen + 1 + getIntegerTermBytes(methodLen);
    memcpy(aml, acpiFuncs, methodLen);
    kdebug("Абсолютный путь к методу: ");
    logVarPath();
    byte flags = *aml++;
    kdebug("\n\tЧисло аргументов: %d\n", flags & 7);
    kdebug("\tСериализация: ");
    kdebug((flags & 8) ? "Да\n" : "Нет\n");
    kdebug("\tУровень синхронизации: %x\n", (flags & 0xF0) >> 4);
    byte names = getVarPathLen();
    *acpiNamespace++ = names;
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, names * 4);
    acpiNamespace += names;
    *acpiNamespace++ = addr;
    acpiFuncs += methodLen;
    kdebug("Метод сохранён в %x.\n", addr);
    *acpiNamespace++ = _methodLen;
    return _len;
}

dword parseMutex(byte *aml) {
    word nameSegs = parseName(aml);
    aml += nameSegs >> 8;
    dword addr = (dword)acpiData;
    *acpiData++ = 0x01;
    byte syncChannel = *aml++;
    kdebug("Канал синхронизации: %d.\n", syncChannel);
    *acpiData++ = syncChannel;
    acpiData += defBlockRevision > 1 ? 8 : 4;
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, (getVarPathLen()) * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace++ = addr;
    *acpiNamespace++ = 2 + (defBlockRevision > 1 ? 8 : 4);
    return 1 + (nameSegs >> 8);
}

dword parseEvent(byte *aml) {
    word nameSegs = parseName(aml);
    aml += nameSegs >> 8;
    dword addr = (dword)acpiData;
    *acpiData++ = 0x02;
    *acpiData++ = 0x00;
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, (getVarPathLen()) * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace++ = addr;
    *acpiNamespace++ = 2;
    return nameSegs >> 8;
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

dword upgradePackage(byte *pkg, byte *&out, FuncFrame *frame) {
    *out++ = 0x9A;
    pkg ++;
    pkg += getPkgBytes(*pkg);
    byte elements = *pkg++;
    *out++ = elements;
    dword *pkgEls = (dword*)out;
    out += elements * 8;
    dword pkgLen = 2 + elements * 8;
    kdebugdisable();
    for (byte i = 0; i < elements; i++) {
        byte *addr = pkg;
        pkgEls[2*i] = (dword)out;
        TermArg a = getTermArg(pkg, frame);
        if (a.type == maxqword) {
            kdebugenable();
            kdebug("ОШИБКА: Не удалось рассчитать элемент %d\n", i+1);
            return 0;
        }
        if (a.type & 0x80) {
            dword len = getACPIObjLen((byte*)a.value);
            if (a.type == 0x91)
                upgradeBuffer(addr, out, frame);
            else if (a.type != 0x92) {
                memcpy((byte*)a.value, out, len);
                out += len;
            }
            pkgEls[2*i+1] = len;
            pkgLen += len;
        } else if (isLeadNameChar(*addr)) {
            word nameSegs = parseName(addr);
            memcpy(addr, out, nameSegs >> 8);
            out += nameSegs >> 8;
            pkgEls[2*i+1] = nameSegs >> 8;
            pkgLen += nameSegs >> 8;
        } else {
            dword len = getIntegerTermBytes(a.value);
            out += encodeIntegerTerm(a.value, out);
            pkgEls[2*i+1] = len;
            pkgLen += len;
        }
    }
    kdebugenable();
    return pkgLen;
}

void remapPackage(byte *pkg, byte *newAddr) {
    dword addr = (dword)newAddr;
    pkg++;
    byte elements = *pkg++;
    dword *table = (dword*)pkg;
    dword base = *table;
    base -= elements * 8 + 2;
    newAddr += elements * 8 + 2;
    for (byte i = 0; i < elements; i++) {
        table[2*i] -= base;
        table[2*i] += addr;
        byte *maybePackage = (byte*)table[2*i];
        if (*maybePackage == 0x9A)
            remapPackage(maybePackage, newAddr);
        newAddr += table[2*i + 1];
    }
}

dword upgradeBuffer(byte *buf, byte *&out, FuncFrame *frame) {
    *out++ = 0x22;
    if (*buf++ == 0x11)
        buf += getPkgBytes(*buf);
    dword bufLen = getIntegerTerm(buf);
    buf += getIntegerTermBytes(buf);
    out += encodeIntegerTerm(bufLen, out);
    memcpy(buf, out, bufLen);
    return bufLen + getIntegerTermBytes(bufLen) + 1;
}

dword nameACPIObj(byte* aml) {
    dword addr = (dword)aml;
    word nameSegs = parseName(aml);
    kdebug("Абсолютный путь к объекту: ");
    logVarPath();
    kdebugnewl();
    if (getACPIObjAddr((AMLName*)varPathBase, getVarPathLen())) {
        kdebug("ВНИМАНИЕ: Объект с таким названием уже существует\n");
        return getACPIObjLen((AMLName*)varPathBase, getVarPathLen()) + (nameSegs >> 8);
    }
    aml += nameSegs >> 8;
    kdebug("Первый байт объекта: %x.\n", *aml);
    FuncFrame _frame = {};
    _frame.tmpSpace = (byte*)0x2000000;
    TermArg arg = getTermArg(aml, &_frame);
    if (arg.type == maxqword) {
        if (!reEvaluating) {
            kdebug("Объект потребует дополнительного вычисления.\n");
            *acpiReEval++ = (dword)'NAME';
            *acpiReEval++ = addr;
            return (dword)aml - addr;
        } else {
            kdebug("ОШИБКА: Не удаётся вычислить объект\n");
            return (dword)aml - addr;
        }
    }
    *acpiNamespace++ = getVarPathLen();
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, getVarPathLen() * 4);
    acpiNamespace += getVarPathLen();
    *acpiNamespace++ = (dword)acpiData;
    dword dataSize = 0;
    if (arg.type == 0x01 || arg.type == 0x02) {
        dataSize = encodeIntegerTerm(arg.value, acpiData);
        acpiData += dataSize;
        *acpiNamespace++ = dataSize;
    }
    else if (arg.type == 0x8D) {
        dataSize = 2;
        *acpiData++ = 0x0D;
        byte *ptr = (byte*)arg.value;
        ptr ++;
        while (*ptr) {
            *acpiData++ = *ptr++;
            dataSize ++;
        }
        *acpiData++ = 0;
        *acpiNamespace++ = dataSize;
    }
    else if (arg.type == 0x91) {
        byte *ptr = (byte*)arg.value;
        dataSize = upgradeBuffer(ptr, acpiData, &_frame);
        *acpiNamespace++ = dataSize;
    }
    else if (arg.type == 0x92) {
        byte *ptr = (byte*)arg.value;
        dataSize = getACPIObjLen(ptr);
        memcpy(ptr, acpiData, dataSize);
        remapPackage(acpiData, acpiData);
        acpiData += dataSize;
        *acpiNamespace++ = dataSize;
    }
    else if (arg.type == 0xF1) {
        dataSize = 9;
        *acpiData++ = 0xF1;
        *(qword*)acpiData = arg.value;
        acpiData += 8;
        *acpiNamespace++ = dataSize;
    }
    else if (arg.type == maxqword) {
        kdebug("Объект потребует дополнительного вычисления.\n");
    }
    return (dword)aml - addr;
}

byte* getACPIObjAddr(const char* strPath) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length = strToPath(strPath, path);
    return getACPIObjAddr(path, length);
}

byte* getACPIObjAddr(AMLName* path, byte length) {
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x1000; i += 0) {
        if (names[i] == 0) {
            return nullptr;
        }
        if (names[i] != length) {
            i += names[i] + 3;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 2;
            continue;
        }
        
        i += length;
        kdebug("Найден объект в пространстве.\n");
        kdebug("Адрес = %x.\n", names[i]);
        return (byte*)names[i];
    }
    return nullptr;
}

bool sameACPIDomain(dword a1, dword a2) {
    return (a1 >= acpiFieldsBase && a2 >= acpiFieldsBase && a1 < (acpiFieldsBase + 0x400) && a2 < (acpiFieldsBase + 0x400)) ||
        (a1 >= acpiDataBase && a2 >= acpiDataBase && a1 < (acpiDataBase + 0x2000) && a2 < (acpiDataBase + 0x2000)) ||
        (a1 >= acpiFuncsBase && a2 >= acpiFuncsBase && a1 < (acpiFuncsBase + 0x2000) && a2 < (acpiFuncsBase + 0x2000));
}

dword getACPIObjLen(AMLName* path, byte length) {
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x1000; i += 0) {
        if (names[i] == 0) {
            return 0;
        }
        if (names[i] != length) {
            i += names[i] + 3;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 2;
            continue;
        }
        i += length + 1;
        dword objLen = names[i];
        kdebug("Найден объект в пространстве.\n");
        kdebug("Длина объекта - %d Б.\n", objLen);
        return objLen;
    }
    return 0;
}

dword getACPIObjLen(byte *ptr) {
    byte id = *ptr;
    kdebug("Тип объекта - ");
    if (id >= 0x0A && id <= 0x0C || id == 0x0E || id == 0x00 || id == 0x01 || id == 0xFF) {
        kdebug("IntegerTerm.\n");
        byte len = getIntegerTermBytes(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    else if (isLeadNameChar(*ptr)) {
        kdebug("ссылка.\n");
        word nameSegs = parseName(ptr);
        kdebug("Длина = %d Б.\n", nameSegs >> 8);
        return nameSegs >> 8;
    }
    ptr ++;
    if (id == 0x0D) {
        kdebug("строка.\n");
        byte len = strlen((char*)ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x11) {
        kdebug("Buffer.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x22) {
        kdebug("Buffer+.\n");
        dword len = getIntegerTerm(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x12) {
        kdebug("Package.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x9A) {
        kdebug("Package+.\n");
        dword len = 2;
        byte elements = *ptr++;
        len += elements * 8;
        dword *elementTable = (dword*)ptr;
        for (byte i = 0; i < elements; i++) {
            len += elementTable[2 * i + 1];
        }
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    if (id == 0x13) {
        kdebug("VarPackage.\n");
        dword len = getPkgLength(ptr);
        kdebug("Длина = %d Б.\n", len);
        return len;
    }
    kdebug("<%x>.\n", id);
    kdebug("ВНИМАНИЕ: Невозможно определить длину. Возвращается 0\n");
    return 0;
}

dword getACPIObjLen(const char* strPath) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length = strToPath(strPath, path);
    return getACPIObjLen(path, length);
}

void remapACPIObj(const char* strPath, byte *newPtr) {
    AMLName path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte length = strToPath(strPath, path);
    remapACPIObj(path, length, newPtr);
}

void remapACPIObj(AMLName* path, byte length, byte *newPtr) {
    dword *names = (dword*)acpiNamespaceBase;
    for (dword i = 0; i < 0x1000; i += 0) {
        if (names[i] == 0) {
            kdebug("ВНИМАНИЕ: Не удалось найти объект\n");
            return;
        }
        if (names[i] != length) {
            i += names[i] + 3;
            continue;
        }
        i ++;
        if (memcmp((byte*)(names + i), (byte*)path, length * 4) != 0x80) {
            i += names[i-1] + 2;
            continue;
        }
        i += length;
        kdebug("Найден объект в пространстве.\nАдрес = %x.\n", names[i]);
        names[i] = (dword)newPtr;
        kdebug("Адрес изменён на %x.\n", names[i]);
        i++;
        dword newlen = getACPIObjLen(newPtr);
        if (names[i] < newlen)
            names[i] = newlen;
    }
}

void parseTermList(byte *aml, dword len) {
    for (dword i = 0; i < len; i++) {
        kdebug("Основа = %x Сдвиг = %d Адрес = %x Байт = %x\n", aml, i, aml+i, aml[i]);
        if (aml[i] == 0x10) {
            kdebug("Найдено определение Scope.\n");
            i ++;
            i += parseScope(aml + i) - 1;
        }
        else if (aml[i] == 0x08) {
            kdebug("Найдено определение Name.\n");
            i ++;
            i += nameACPIObj(aml + i) - 1;
        }
        else if (aml[i] == 0x14) {
            kdebug("Найдено определение Method.\n");
            i ++;
            i += parseMethod(aml + i) - 1;
        }
        else if (aml[i] == 0x5B) {
            i ++;
            if (aml[i] == 0x01) {
                kdebug("Найдено определение Mutex.\n");
                i ++;
                i += parseMutex(aml + i) - 1;
            }
            else if (aml[i] == 0x02) {
                kdebug("Найдено определение Event.\n");
                i ++;
                i += parseEvent(aml + i) - 1;
            }
            else if (aml[i] == 0x80) {
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
                dword dl = parseDevice(aml + i) - 1;
                i += dl;
            }
            else if (aml[i] == 0x85) {
                kdebug("Найдено определение ThermalZone.\n");
                i ++;
                dword dl = parseThermalZone(aml + i) - 1;
                i += dl;
            }
        }
    }
}

void reEvaluate() {
    if (acpiReEval == (dword*)acpiReEvalBase) {
        kdebug("Перерасчёт объектов не требуется.\n");
        return;
    }
    kdebug("Начат перерасчёт объектов.\n");
    reEvaluating = true;
    dword *ptr = (dword*)acpiReEvalBase;
    while (ptr < acpiReEval) {
        if (*ptr == (dword)'NAME') {
            kdebug("Требуется перерасчёт имени.\n");
            ptr ++;
            byte *addr = (byte*)*ptr;
            kdebug("Адрес = %x.\n", addr);
            nameACPIObj(addr);
        }
        ptr ++;
    }
}

void parseDefBlock(byte *aml) {
    kdebug("Начата обработка блока AML по адресу %x.\n", (dword)aml);
    reEvaluating = false;
    if (!acpiNamespaceInit) {
        kdebug("Расчистка места.\n");
        for (byte i = 0; i < 0x10; i++) {
            parsingPath[i] = 0;
        }

        for (byte i = 0; i < 0x10; i++) {
            varPath[i] = 0;
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

        acpiNamespaceInit = true;
    }

    kdebug("Заголовок таблицы:\n");
    kdebug("\tПодпись: \"");
    for (byte i = 0; i < 4; i++) {
        if (aml[i] != 0)
            kdebug(aml[i]);
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
            kdebug(aml[i]);
    }
    aml += 6;
    kdebug("\"\n");
    kdebug("\tИдентификатор таблицы: \"");
    for (byte i = 0; i < 8; i++) {
        if (aml[i] != 0)
            kdebug(aml[i]);
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
    reEvaluate();

    kdebug("Длина пространства имён: %d Б.\n", (dword)acpiNamespace - acpiNamespaceBase);
    kdebug("Длина пространства полей: %d Б.\n", (dword)acpiFields - acpiFieldsBase);
    kdebug("Длина пространства данных: %d Б.\n", (dword)acpiData - acpiDataBase);
    kdebug("Длина пространства функций: %d Б.\n", (dword)acpiFuncs - acpiFuncsBase);
    kdebug("Обработка AML успешно завершена.\n");
}

qword callMethod(const char* strPath, ...) {
    va_list l;
    va_start(l, strPath);
    kdebug("\nВызван метод \"");
    kdebug(strPath);
    kdebug("\".\n");
    storeParsingPath();
    parsingPath = (AMLName*)parsingPathBase;
    byte len = strToPath(strPath, parsingPath);
    parsingPath += len;
    byte *code = getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen());
    if (!code) {
        kdebug("ОШИБКА: Метод не существует\n");
        return maxqword;
    }
    kdebug("Адрес метода: %x.\n", code);
    code++;
    dword length = getIntegerTerm(code) - 1;
    kdebug("Длина метода: %d Б.\n", length);
    code += getIntegerTermBytes(code);
    byte argc = *code & 7;
    kdebug("Число аргументов: %d.\n", argc);
    byte syncLevel = (*code >> 4) & 0xF;
    kdebug("Уровень синхронизации: %d.\n", syncLevel);
    qword args[] = {0,0,0,0,0,0,0};
    for (byte i = 0; i < argc; i++) {
        args[i] = va_arg(l, dword);
    }
    code ++;
    FuncFrame frame;
    for (byte i = 0; i < 7; i++) {
        frame.locals[i] = {0x7F,0};
    }
    for (byte i = 0; i < argc; i++) {
        frame.args[i] = {0x7F, args[i]};
    }
    frame.syncLevel = syncLevel;
    dword addr = (dword)code;
    frame.addr = (byte*)addr;
    frame.tmpSpace = (byte*)0x801000;
    TermArg retVal = runMethod(code, length, syncLevel, &frame);
    restoreParsingPath();
    if (retVal.type == 0xAAAAAAAA)
        return 0;
    return retVal.value;
}

void processEvent(byte eventNo) {
    byte *addr = getACPIObjAddr((AMLName*)varPathBase, getVarPathLen());
    kdebug("Событие %d выпущено ", eventNo);
    switch (*addr) {
        case 0x82: kdebug("устройством "); break;
        case 0x85: kdebug("термозоной "); break;
        case 0x83: kdebug("процессором "); break;
    }
    logVarPath();
    kdebugnewl();
    kdebug("// TODO\n");
}