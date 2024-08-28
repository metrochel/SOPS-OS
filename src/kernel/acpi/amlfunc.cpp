#include "aml.hpp"
#include "../memmgr/paging.hpp"
#include "../io/io.hpp"
#include "../pci/pci.hpp"
#include "../str/str.hpp"

const TermArg errorArg = {maxqword, maxqword};
byte *tmpPtr = (byte*)0x801000;

void intToStrDec(qword value, byte*& addr) {
    if (value == 0) {
        *addr++ = '0';
        return;
    }
    byte digits = 0;
    qword numclone = value;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    addr += digits - 1;
    for (byte i = 0; i < digits; i++) {
        *addr-- = value % 10 + 0x30;
        value /= 10;
    }
    addr += digits + 1;
    *addr++ = 0x00;
}

void intToStrHex(qword value, byte*& addr) {
    if (value == 0) {
        *addr++ = '0';
        *addr++ = 'x';
        *addr++ = '0';
        *addr++ = '0';
        return;
    }

    qword mask = 0xF;
    byte digits = 1;
    while (mask < value && mask < (qword)(0x8000000000000000)) {
        mask <<= 4;
        digits ++;
    }
    if (!(value & mask))
        mask >>= 4;
    *addr++ = '0';
    *addr++ = 'x';
    if (mask == 0xF)
        *addr++ = '0';
    while (mask > 0) {
        qword digit = value & mask;
        while (digit > 0xF)
            digit >>= 4;
        if (digit < 10)
            *addr++ = digit + 0x30;
        else
            *addr++ = digit - 10 + 0x41;
        mask >>= 4;
    }
    *addr++ = 0x00;
}

void intToStr(qword value, byte*& addr) {
    *addr++ = 0x0D;
    if (value == 0) {
        *addr++ = 0x00;
        return;
    }

    byte *ptr = (byte*)&value;
    for (byte i = 0; i < 8; i++) {
        if (!*ptr)
            break;
        *addr++ = *ptr++;
    }
}

void bufToStrDec(byte *buf, byte*& addr) {
    buf ++;
    buf += (*buf) >> 6;
    buf ++;
    dword len = getIntegerTerm(buf);
    kdebug("Длина буфера: %d Б.\n", len);
    if (!len) {
        *addr++ = 0;
        return;
    }
    buf += getIntegerTermBytes(buf);
    intToStrDec(buf[0], addr);
    for (dword i = 1; i < len; i++) {
        addr--;
        *addr++ = ',';
        *addr++ = ' ';
        intToStrDec(buf[i], addr);
    }
}

void bufToStrHex(byte *buf, byte*& addr) {
    buf ++;
    buf += (*buf) >> 6;
    buf ++;
    dword len = getIntegerTerm(buf);
    kdebug("Длина буфера: %d Б.\n", len);
    if (!len) {
        *addr++ = 0;
        return;
    }
    buf += getIntegerTermBytes(buf);
    intToStrHex(buf[0], addr);
    for (dword i = 1; i < len; i++) {
        addr--;
        *addr++ = ',';
        *addr++ = ' ';
        intToStrHex(buf[i], addr);
    }
}

qword strToInt(byte *buf) {
    qword out = 0;
    kdebug("Система счисления строки - ");
    if (*(word*)buf == 0x7830) {
        kdebug("шестнадцатеричная.\n");
        buf += 2;
        while (*buf != 0) {
            byte digit = *buf++;
            if (!digit)
                break;
            if (digit >= 0x41) {
                digit -= 0x41;
                digit += 10;
            } else
                digit -= 0x30;
            out *= 16;
            out += digit;
        }
        kdebug("Полученное значение: %X.\n", out);
    }
    else {
        kdebug("десятичная.\n");
        while (*buf != 0) {
            byte digit = *buf++;
            if (!digit)
                break;
            digit -= 0x30;
            out *= 10;
            out += digit;
        }
        kdebug("Полученное значение: %D.\n", out);
    }
    return out;
}

qword bufToInt(byte *buf) {
    qword out = 0;
    buf += *buf >> 6;
    buf ++;
    dword length = getIntegerTerm(buf);
    kdebug("Длина буфера: %d Б.\n", length);
    buf += getIntegerTermBytes(buf);
    if (length > 8)
        length = 8;
    for (byte i = 0; i < length; i++) {
        out |= (qword)(buf[i]) << (i * 8);
    }
    buf += length;
    kdebug("Полученное значение: %X.\n", out);
    return out;
}

void bufToStr(byte *buf, dword length, byte*& addr) {
    buf ++;
    buf += (*buf) >> 6;
    buf ++;
    dword bufLen = getIntegerTerm(buf);
    kdebug("Длина буфера: %d Б.\n", bufLen);
    buf += getIntegerTermBytes(bufLen);
    if (length == 0 || length == 0xFF || length > bufLen)
        length = bufLen;
    kdebug("Длина-аргумент: %d Б.\n", length);
    *addr++ = 0x0D;
    kdebug("Значение строки: \"");
    for (dword i = 0; i < length; i++) {
        if (!buf[i])
            break;
        *addr++ = buf[i];
        kdebug(buf[i]);
    }
    kdebug("\".\n");
    *addr++ = 0x00;
}

void strToBuf(byte *src, dword length, byte*& addr) {
    *addr++ = 0x11;
    addr += encodePkgLength(length, addr);
    addr += encodeIntegerTerm(length, addr);
    kdebug("Записывается значение \"");
    src++;
    while (*src) {
        kdebug(*src);
        *addr++ = *src++;
    }
    kdebug("\".\n");
    *addr++ = 0;
}

void intToBuf(qword val, dword length, byte*& addr) {
    *addr++ = 0x11;
    addr += encodePkgLength(length, addr);
    addr += encodeIntegerTerm(length, addr);
    if (defBlockRevision == 1) {
        *(dword*)addr = val & maxdword;
        kdebug("Записывается значение %x.\n", *(dword*)addr);
        addr += 4;
    } else {
        *(qword*)addr = val;
        kdebug("Записывается значение %X.\n", *(qword*)addr);
        addr += 8;
    }
}

byte findSetRightBit(qword value) {
    qword mask = 0x8000000000000000;
    byte msb = 64;
    while (msb > 0) {
        if (value & mask)
            return msb;
        mask >>= 1;
        msb --;
    }
    return 0;
}

byte findSetLeftBit(qword value) {
    qword mask = 1;
    byte lsb = 1;
    while (lsb <= 64) {
        if (value & mask)
            return lsb;
        mask <<= 1;
        lsb ++;
    }
    return 0;
}

TermArg performMatch(byte *pkg, byte op1, TermArg obj1, byte op2, TermArg obj2, dword start, TermArg*& locals, TermArg*& args) {
    kdebug("%x\n", *pkg);
    pkg ++;
    pkg += *pkg >> 6;
    pkg ++;
    byte length = *pkg++;
    kdebug("Число элементов: %d.\n", length);
    for (byte i = 0; i < length; i++) {
        TermArg arg = getTermArg(pkg, locals, args);
        if (i < start) {
            continue;
        }
        if (arg.type != 0x01 && arg.type != 0x8D && arg.type != 0x91)
            continue;
        
        bool match1 = false;
        bool match2 = false;
        switch (op1) {
            case 0: match1 = true; break;
            case 1:
                if (obj1.type == 0x01 || obj1.type == 0x02)
                    match1 = arg.value == obj1.value;
                else if (obj1.type == 0x8D)
                    match1 = strcmp((char*)(arg.value + 1), (char*)(obj1.value + 1)) == 0x80 ? 1 : 0;
                else if (obj1.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match1 = memcmp((byte*)arg.value, (byte*)obj1.value, length) == 0x80 ? 1 : 0;
                }
                break;
            case 2:
                if (obj1.type == 0x01 || obj1.type == 0x02)
                    match1 = arg.value <= obj1.value;
                else if (obj1.type == 0x8D)
                    match1 = strcmp((char*)(arg.value + 1), (char*)(obj1.value + 1)) <= 0x80 ? 1 : 0;
                else if (obj1.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match1 = memcmp((byte*)arg.value, (byte*)obj1.value, length) <= 0x80 ? 1 : 0;
                }
                break;
            case 3:
                if (obj1.type == 0x01 || obj1.type == 0x02)
                    match1 = arg.value < obj1.value;
                else if (obj1.type == 0x8D)
                    match1 = strcmp((char*)(arg.value + 1), (char*)(obj1.value + 1)) < 0x80 ? 1 : 0;
                else if (obj1.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match1 = memcmp((byte*)arg.value, (byte*)obj1.value, length) < 0x80 ? 1 : 0;
                }
                break;
            case 4:
                if (obj1.type == 0x01 || obj1.type == 0x02)
                    match1 = arg.value >= obj1.value;
                else if (obj1.type == 0x8D)
                    match1 = strcmp((char*)(arg.value + 1), (char*)(obj1.value + 1)) >= 0x80 ? 1 : 0;
                else if (obj1.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match1 = memcmp((byte*)arg.value, (byte*)obj1.value, length) >= 0x80 ? 1 : 0;
                }
                break;
            case 5:
                if (obj1.type == 0x01 || obj1.type == 0x02)
                    match1 = arg.value > obj1.value;
                else if (obj1.type == 0x8D)
                    match1 = strcmp((char*)(arg.value + 1), (char*)(obj1.value + 1)) > 0x80 ? 1 : 0;
                else if (obj1.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match1 = memcmp((byte*)arg.value, (byte*)obj1.value, length) > 0x80 ? 1 : 0;
                }
                break;
        }

        switch (op2) {
            case 0: match2 = true; break;
            case 1:
                if (obj2.type == 0x01 || obj2.type == 0x02)
                    match2 = arg.value == obj2.value;
                else if (obj2.type == 0x8D)
                    match2 = strcmp((char*)(arg.value + 1), (char*)(obj2.value + 1)) == 0x80 ? 1 : 0;
                else if (obj2.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match2 = memcmp((byte*)arg.value, (byte*)obj2.value, length) == 0x80 ? 1 : 0;
                }
                break;
            case 2:
                if (obj2.type == 0x01 || obj2.type == 0x02)
                    match2 = arg.value <= obj2.value;
                else if (obj2.type == 0x8D)
                    match2 = strcmp((char*)(arg.value + 1), (char*)(obj2.value + 1)) <= 0x80 ? 1 : 0;
                else if (obj2.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match2 = memcmp((byte*)arg.value, (byte*)obj2.value, length) <= 0x80 ? 1 : 0;
                }
                break;
            case 3:
                if (obj2.type == 0x01 || obj2.type == 0x02)
                    match2 = arg.value < obj2.value;
                else if (obj2.type == 0x8D)
                    match2 = strcmp((char*)(arg.value + 1), (char*)(obj2.value + 1)) < 0x80 ? 1 : 0;
                else if (obj2.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match2 = memcmp((byte*)arg.value, (byte*)obj2.value, length) < 0x80 ? 1 : 0;
                }
                break;
            case 4:
                if (obj2.type == 0x01 || obj2.type == 0x02)
                    match2 = arg.value >= obj2.value;
                else if (obj2.type == 0x8D)
                    match2 = strcmp((char*)(arg.value + 1), (char*)(obj2.value + 1)) >= 0x80 ? 1 : 0;
                else if (obj2.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match2 = memcmp((byte*)arg.value, (byte*)obj2.value, length) >= 0x80 ? 1 : 0;
                }
                break;
            case 5:
                if (obj2.type == 0x01 || obj2.type == 0x02)
                    match2 = arg.value > obj2.value;
                else if (obj2.type == 0x8D)
                    match2 = strcmp((char*)(arg.value + 1), (char*)(obj2.value + 1)) > 0x80 ? 1 : 0;
                else if (obj2.type == 0x91) {
                    dword length = getPkgLength((byte*)(arg.value + 1));
                    match2 = memcmp((byte*)arg.value, (byte*)obj2.value, length) > 0x80 ? 1 : 0;
                }
                break;
        }
        kdebug("Значение 1 сравнения: %d\nЗначение 2 сравнения: %d\n", match1, match2);

        if (match1 && match2) {
            kdebug("Найдено совпадение.\n");
            kdebug("Индекс = %d.\n", i);
            return {0x01, i};
        }
    }
    kdebug("Совпадение не найдено.\nВозвращается Ones.\n");
    return {0x01, 0xFF};
}

qword toBCD(qword value) {
    byte i = 0;
    qword bcd = 0;
    while (value > 0) {
        bcd |= (value % 10) << (i*4);
        i++;
        value /= 10;
    }
    return bcd;
}

qword fromBCD(qword value) {
    byte i = 1;
    qword bin = 0;
    while (value > 0) {
        bin += (value & 0xF) * i;
        value >>= 4;
        i *= 10;
    }
    return bin;
}

TermArg getSlice(byte *obj, dword index, dword length, byte*& addr) {
    TermArg ret = {0, (dword)addr};
    if (*obj == 0x0D) {
        ret.type = 0x8D;
        obj++;
        dword len = strlen((char*)obj);
        dword endIndex = index + length - 1;
        if (endIndex >= len) {
            endIndex = len - 1;
        }
        *addr++ = 0x0D;
        obj += index;
        for (dword i = index; i <= endIndex; i++) {
            *addr++ = *obj++;
        }
        if (*(addr - 1) != 0)
            *addr++ = 0;
        kdebug("Полученный кусок: \"");
        byte *ptr = (byte*)(ret.value + 1);
        while (*ptr != 0) {
            kdebug(*ptr++);
        }
        kdebug("\".\n");
    } else {
        ret.type = 0x91;
        obj ++;
        obj += *obj >> 6;
        obj ++;
        dword len = getIntegerTerm(obj);
        obj += getIntegerTermBytes(len);
        dword endIndex = index + length - 1;
        if (endIndex >= len) {
            endIndex = len - 1;
        }
        *addr++ = 0x11;
        dword indexLen = endIndex - index + 1;
        addr += encodePkgLength(indexLen + getIntegerTermBytes(indexLen), addr);
        addr += encodeIntegerTerm(indexLen, addr);
        obj += index;
        for (dword i = index; i <= endIndex; i++) {
            *addr++ = *obj++;
        }
        kdebug("Длина куска: %d Б.\n", indexLen);
    }
    return ret;
}

TermArg readField(byte* fieldAddr) {
    if (*fieldAddr != 0xFD)
        return {0,0};
    
    fieldAddr ++;
    byte flags = *fieldAddr++;
    byte space = *fieldAddr++;
    byte access = flags & 0xF;
    qword offset = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(offset);
    qword size = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(size);

    kdebug("Вид доступа: %d.\n", access);
    kdebug("Сдвиг поля: %X.\n", offset);
    kdebug("Размер поля: %D Б.\n", size);
    kdebug("Пространство поля - ");
    switch (space) {
        case 0:
            kdebug("ОЗУ.\n");
            if (access == 1) {
                byte *read = (byte*)offset;
                return {0x01, *read};
            }
            else if (access == 2) {
                word *read = (word*)offset;
                return {0x01, *read};
            }
            else if (access == 3) {
                dword *read = (dword*)offset;
                return {0x01, *read};
            }
            else if (access == 4) {
                qword *read = (qword*)offset;
                return {0x01, *read};
            }
            break;
        case 1:
            kdebug("I/O.\n");
            offset &= maxword;
            if (access == 1) {
                return {0x01, inb(offset)};
            }
            else if (access == 2) {
                return {0x01, inw(offset)};
            }
            else if (access == 3) {
                return {0x01, inl(offset)};
            }
            break;
        case 2:
            kdebug("конфигурация PCI.\n");
            byte bus = (offset >> 16) & 0xFF;
            byte device = (offset >> 11) & 0x1F;
            byte function = (offset >> 8) & 0x7;
            byte byteOffset = offset & 0xFF;
            if (access == 1) {
                return {0x01, pciConfigReadB(bus, device, function, byteOffset)};
            }
            else if (access == 2) {
                return {0x01, pciConfigReadW(bus, device, function, byteOffset)};
            }
            else if (access == 3) {
                return {0x01, pciConfigReadDW(bus, device, function, byteOffset)};
            }
    }
    return {0,0};
}

TermArg readField(AMLName fieldName) {
    byte *addr = getACPIObjAddr(&fieldName, 1);
    return readField(addr);
}

void writeField(byte* fieldAddr, TermArg value) {
    if (*fieldAddr != 0xFD)
        return;
    
    fieldAddr ++;
    byte flags = *fieldAddr++;
    byte space = *fieldAddr++;
    byte access = flags & 0xF;
    qword offset = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(offset);
    qword size = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(size);

    kdebug("Вид доступа: %d.\n", access);
    kdebug("Сдвиг поля: %X.\n", offset);
    kdebug("Размер поля: %D Б.\n", size);
    kdebug("Пространство поля - ");
    switch (space) {
        case 0:
            kdebug("ОЗУ.\n");
            if (access == 1) {
                byte *write = (byte*)offset;
                *write = value.value & maxbyte;
            }
            else if (access == 2) {
                word *write = (word*)offset;
                *write = value.value & maxword;
            }
            else if (access == 3) {
                dword *write = (dword*)offset;
                *write = value.value & maxdword;
            }
            else if (access == 4) {
                qword *write = (qword*)offset;
                *write = value.value;
            }
            break;
        case 1:
            kdebug("I/O.\n");
            offset &= maxword;
            if (access == 1) {
                outb(offset, value.value & maxbyte);
            }
            else if (access == 2) {
                outw(offset, value.value & maxword);
            }
            else if (access == 3) {
                outl(offset, value.value & maxdword);
            }
            break;
        case 2:
            kdebug("конфигурация PCI.\n");
            byte bus = (offset >> 16) & 0xFF;
            byte device = (offset >> 11) & 0x1F;
            byte function = (offset >> 8) & 0x7;
            byte byteOffset = offset & 0xFF;
            if (access == 1) {
                pciConfigWriteB(bus, device, function, byteOffset, value.value & maxbyte);
            }
            else if (access == 2) {
                pciConfigWriteW(bus, device, function, byteOffset, value.value & maxword);
            }
            else if (access == 3) {
                pciConfigWriteDW(bus, device, function, byteOffset, value.value & maxdword);
            }
    }
}

void writeField(AMLName fieldName, TermArg value) {
    byte *addr = getACPIObjAddr(&fieldName, 1);
    writeField(addr, value);
}

TermArg castTermArg(TermArg value, byte type) {
    if (value.type == type)
        return value;

    kdebug("Тип аргумента: %X.\n", value.type);
    kdebug("Искомый тип: %x.\n", type);
    if (value.type == 0x01) {
        if (type == 0x8D) {
            dword addr = (dword)tmpPtr;
            intToStr(value.value, tmpPtr);
            return {0x8D, addr};
        }
        if (type == 0x91) {
            dword addr = (dword)tmpPtr;
            dword length;
            if (defBlockRevision == 1)
                length = 4;
            else
                length = 8;
            intToBuf(value.value, length, tmpPtr);
            return {0x91, addr};
        }
        if (type == 0x02) {
            return {0x02, (value.value) ? 1ull : 0ull};
        }
    }
    if (value.type == 0x8D) {
        if (type == 0x01) {
            return {0x01, strToInt((byte*)value.value)};
        }
        if (type == 0x91) {
            dword addr = (dword)tmpPtr;
            strToBuf((byte*)value.value, 0, tmpPtr);
            return {0x91, addr};
        }
        if (type == 0x02) {
            return {0x02, *(byte*)(value.value + 1) ? 1ull : 0ull};
        }
    }
    if (value.type == 0x91) {
        if (type == 0x01) {
            return {0x01, bufToInt((byte*)value.value)};
        }
        if (type == 0x8D) {
            dword addr = (dword)tmpPtr;
            bufToStr((byte*)value.value, 0, tmpPtr);
            return {0x8D, addr};
        }
        if (type == 0x02) {
            byte *ptr = (byte*)value.value;
            ptr++;
            ptr += *ptr >> 6;
            ptr ++;
            dword length = getIntegerTerm(ptr);
            return {0x02, (length) ? 1ull : 0ull};
        }
    }
    
    kdebug("ОШИБКА: Провал преобразования\n");
    kdebug("Невозможно преобразовать тип %X в тип %x.\n", value.type, type);
    return errorArg;
}

TermArg getTermArg(byte*& code, TermArg*& locals, TermArg*& args) {
    TermArg val = {0,0};
    if (isLeadNameChar(*code)) {
        kdebug("Тип значения - ссылка.\n");
        word nameSegs = parseName(code);
        code += nameSegs >> 8;
        byte *addr = getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen());
        if (!addr) {
            addr = getACPIObjAddr(parsingPath - 1, 1);
            if (!addr)
                return {0,0};
            else {
                kdebug("Ссылка указывает на поле ");
                kdebug(*(parsingPath - 1));
                kdebug(".\n");
            }
        }
        for (byte j = 0; j < (nameSegs & 0xFF); j++)
            *(--parsingPath) = 0;
        return getTermArg(addr, locals, args);
    }
    if (*code == 0x0A || *code == 0x0B || *code == 0x0C || *code == 0x0E || *code == 0x00 || *code == 0x01 || *code == 0xFF) {
        kdebug("Тип значения - IntegerTerm.\n");
        val = {0x01, getIntegerTerm(code)};
        kdebug("Значение = %D (%X).\n", val.value, val.value);
        code += getIntegerTermBytes(code);
        return val;
    }
    else if (*code == 0x0D) {
        kdebug("Значение = \"");
        val = {0x8D, (qword)code};
        code++;
        while (*code != 0) {
            kdebug(*code++);
        }
        code++;
        kdebug("\" (%X).\n", val.value);
        return val;
    }
    else if (*code == 0x11) {
        kdebug("Тип значения - Buffer.\n");
        val = {0x91, (qword)code};
        code ++;
        dword len = parseBuffer(code);
        code += len;
        return val;
    }
    else if (*code == 0x12) {
        val = {0x92, (qword)code};
        code ++;
        kdebug("Тип значения - Package.\n");
        dword len = parsePackage(code);
        code += len;
        return val;
    }
    else if (*code >= 0x60 && *code <= 0x66) {
        byte localNo = *code & 0x7;
        kdebug("Значение в Local%d.\n", localNo);
        val = locals[localNo];
        code++;
        return val;
    }
    else if (*code >= 0x67 && *code <= 0x6E) {
        byte argNo = *code & 0x7;
        kdebug("Значение в Arg%d.\n", argNo);
        val = args[argNo];
        code++;
        return val;
    }
    else if (*code == 0xFD) {
        return readField(code);
    }
    else if (*code == 0x72 || *code == 0x74 || *code == 0x77 || *code == 0x78 || *code == 0x85 || *code == 0x79 || *code == 0x7A || *code == 0x7B || *code == 0x7C || *code == 0x7D || *code == 0x7E || *code == 0x7F) {
        kdebug("Найден арифметический оператор (%x).\n", *code);
        byte opType = *code++;
        TermArg op1 = getTermArg(code, locals, args);
        TermArg op2 = getTermArg(code, locals, args);
        if (op1.type != 0x01 && op1.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 1\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", op1.type);
            return errorArg;
        }
        if (op2.type != 0x01 && op2.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 2\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", op2.type);
            return errorArg;
        }
        TermArg result = {0x01, 0};
        kdebug("Тип арифметического оператора: ");
        switch (opType) {
            case 0x72:
                kdebug("Add.\n");
                result.value = op1.value + op2.value;
                break;
            case 0x74:
                kdebug("Subtract.\n");
                result.value = op1.value - op2.value;
                break;
            case 0x77:
                kdebug("Multiply.\n");
                result.value = op1.value * op2.value;
                break;
            case 0x78:
                kdebug("Divide.\n");
                result.value = (dword)(op1.value) / (dword)(op2.value);
                break;
            case 0x85:
                kdebug("Mod.\n");
                result.value = (dword)(op1.value) % (dword)(op2.value);
                break;
            case 0x79:
                kdebug("ShiftLeft.\n");
                result.value = op1.value << op2.value;
                break;
            case 0x7A:
                kdebug("ShiftRight.\n");
                result.value = op1.value >> op2.value;
                break;
            case 0x7B:
                kdebug("And.\n");
                result.value = op1.value & op2.value;
                break;
            case 0x7C:
                kdebug("Nand.\n");
                result.value = ~(op1.value & op2.value);
                break;
            case 0x7D:
                kdebug("Or.\n");
                result.value = op1.value | op2.value;
                break;
            case 0x7E:
                kdebug("Nor.\n");
                result.value = ~(op1.value | op2.value);
                break;
            case 0x7F:
                kdebug("Xor.\n");
                result.value = op1.value ^ op2.value;
                break;
        }
        if (opType == 0x78 && *code)
            setTermArg(code, locals, args, result);
        else if (opType == 0x78)
            code++;
        if (*code)
            setTermArg(code, locals, args, result);
        else
            code++;
        return result;
    }
    else if (*code == 0x75 || *code == 0x76 || *code == 0x80) {
        kdebug("Найден арифметический оператор (%x).\n", *code);
        byte opType = *code++;
        byte *container = code;
        TermArg value = getTermArg(code, locals, args);
        if (value.type != 0x01 && value.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 1\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", value.type);
            return errorArg;
        }
        kdebug("Тип операции: ");
        switch (opType) {
            case 0x75:
                kdebug("Increment.\n");
                value.value ++;
                break;
            case 0x76:
                kdebug("Decrement.\n");
                value.value --;
                break;
            case 0x80:
                kdebug("Not.\n");
                value.value = ~value.value;
                break;
        }
        code = container;
        setTermArg(code, locals, args, value);
        if (*code)
            setTermArg(code, locals, args, value);
        else
            code++;
        return value;
    }
    else if (*code == 0x97) {
        kdebug("Найден оператор ToDecimalString.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            return arg;
        }
        dword addr = (dword)tmpPtr;
        TermArg ret = {0x8D, addr};
        *tmpPtr++ = 0x0D;
        if (arg.type == 0x01) {
            kdebug("Тип операнда - IntegerTerm.\n");
            intToStrDec(arg.value, tmpPtr);
            if (*code)
                setTermArg(code, locals, args, ret);
            else
                code++;
            return ret;
        }
        else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            bufToStrDec((byte*)arg.value, tmpPtr);
            if (*code)
                setTermArg(code, locals, args, ret);
            else
                code++;
            return ret;
        }
        else {
            kdebug("ОШИБКА: Неправильный тип операнда\n");
            kdebug("Ожидался тип 0x01, 0x0D или 0x91, получен %X.\n", arg.type);
            return errorArg;
        }
    }
    else if (*code == 0x98) {
        kdebug("Найден оператор ToHexString.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            return arg;
        }
        dword addr = (dword)tmpPtr;
        TermArg ret = {0x8D, addr};
        *tmpPtr++ = 0x0D;
        if (arg.type == 0x01) {
            kdebug("Тип операнда - IntegerTerm.\n");
            intToStrHex(arg.value, tmpPtr);
            if (*code)
                setTermArg(code, locals, args, ret);
            else
                code++;
            return ret;
        }
        else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            bufToStrHex((byte*)arg.value, tmpPtr);
            if (*code)
                setTermArg(code, locals, args, ret);
            else
                code++;
            return ret;
        }
        else {
            kdebug("ОШИБКА: Неправильный тип операнда\n");
            kdebug("Ожидался тип 0x01, 0x0D или 0x91, получен %X.\n", arg.type);
            return errorArg;
        }
    }
    else if (*code == 0x9C) {
        kdebug("Найден оператор ToString.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x91, получен %X.\n", arg.type);
        }
        dword addr = (dword)tmpPtr;
        TermArg length = getTermArg(code, locals, args);
        if (length.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
        }
        bufToStr((byte*)arg.value, length.value, tmpPtr);
        TermArg str = {0x8D, addr};
        setTermArg(code, locals, args, str);
        return str;
    }
    else if (*code == 0x99) {
        kdebug("Найден оператор ToInteger.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type == 0x01) {
            kdebug("Тип операнда - Integer.\n");
            return arg;
        }
        byte* value = (byte*)(arg.value);
        value ++;
        qword out = 0;
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            out = strToInt(value);
        } else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            out = bufToInt(value);
        } else {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", arg.type);
            return errorArg;
        }
        TermArg ret = {0x01, out};
        if (*code)
            setTermArg(code, locals, args, ret);
        return ret;
    }
    else if (*code == 0x96) {
        kdebug("Найден оператор ToBuffer.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            return arg;
        }
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
        }
        else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
        }

        if (arg.type != 0x8D && arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", arg.type);
            return errorArg;
        }

        TermArg ret = {0x91, (qword)tmpPtr};
        dword length;
        if (arg.type == 0x8D)
            length = strlen((char*)arg.value);
        else
            if (defBlockRevision == 1)
                length = 4;
            else
                length = 8;
        
        if (arg.type == 0x01) {
            intToBuf(arg.value, length, tmpPtr);
        } else {
            strToBuf((byte*)arg.value, length, tmpPtr);
        }
        if (*code)
            setTermArg(code, locals, args, ret);
        else
            code++;
        return ret;
    }
    else if (*code >= 0x90 && *code <= 0x95) {
        byte opType = *code++;
        kdebug("Найден логический оператор (%x).\n", opType);
        TermArg op1 = getTermArg(code, locals, args);
        TermArg op2 = {0,0};
        if (op1.type != 0x01 && op1.type != 0x02 && op1.type != 0x8D && op1.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, 0x02, 0x8D или 0x91, получен %X.\n", op1.type);
            return errorArg;
        }
        if (opType != 0x92) {
            op2 = getTermArg(code, locals, args);
            if (op2.type != 0x01 && op2.type != 0x02 && op2.type != 0x8D && op2.type != 0x91) {
                kdebug("ОШИБКА: Неправильный тип операнда 2\n");
                kdebug("Ожидался тип 0x01, 0x02, 0x8D или 0x91, получен %X.\n", op2.type);
                return errorArg;
            }
        }
        if (op2.type != 0)
            op2 = castTermArg(op2, op1.type);
        if (op2.value == maxqword)
            return errorArg;
        TermArg result = {0x02, 0};
        kdebug("Тип операции: ");
        if (opType == 0x90) {
            kdebug("LAnd.\n");
            result.value = (op1.value && op2.value) ? 1 : 0;
            return result;
        }
        if (opType == 0x91) {
            kdebug("LOr.\n");
            result.value = (op1.value || op2.value) ? 1 : 0;
            return result;
        }
        if (opType == 0x92) {
            kdebug("LNot.\n");
            result.value = (!op1.value) ? 1 : 0;
            return result;
        }
        
        if (opType == 0x93) {
            kdebug("LEqual.\n");
            if (op1.type == 0x01 || op1.type == 0x02)
                result.value = op1.value == op2.value;
            else if (op1.type == 0x8D)
                result.value = strcmp((char*)(op1.value + 1), (char*)(op2.value + 1)) == 0x80 ? 1 : 0;
            else if (op1.type == 0x91) {
                dword length = getPkgLength((byte*)(op1.value + 1));
                result.value = memcmp((byte*)op1.value, (byte*)op2.value, length) == 0x80 ? 1 : 0;
            }
        }
        else if (opType == 0x94) {
            kdebug("LGreater.\n");
            if (op1.type == 0x01 || op1.type == 0x02)
                result.value = op1.value > op2.value;
            else if (op1.type == 0x8D)
                result.value = strcmp((char*)(op1.value + 1), (char*)(op2.value + 1)) == 0xFF ? 1 : 0;
            else if (op1.type == 0x91) {
                dword length = getPkgLength((byte*)(op1.value + 1));
                result.value = memcmp((byte*)op1.value, (byte*)op2.value, length) == 0xFF ? 1 : 0;
            }
        }
        else if (opType == 0x95) {
            kdebug("LLess.\n");
            if (op1.type == 0x01 || op1.type == 0x02)
                result.value = op1.value < op2.value;
            else if (op1.type == 0x8D)
                result.value = strcmp((char*)(op1.value + 1), (char*)(op2.value + 1)) == 0x00 ? 1 : 0;
            else if (op1.type == 0x91) {
                dword length = getPkgLength((byte*)(op1.value + 1));
                result.value = memcmp((byte*)op1.value, (byte*)op2.value, length) == 0x00 ? 1 : 0;
            }
        }
        return result;
    }
    else if (*code == 0x81) {
        kdebug("Найден оператор FindSetLeftBit.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            return errorArg;
        }
        TermArg result = {0x01, 0};
        kdebug("Значение операнда: %X (%B).\n", arg.value, arg.value);
        byte msb = findSetLeftBit(arg.value);
        result.value = msb;
        if (*code)
            setTermArg(code, locals, args, result);
        else
            code++;
        return result;
    }
    else if (*code == 0x82) {
        kdebug("Найден оператор FindSetRightBit.\n");
        code++;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            return errorArg;
        }
        TermArg result = {0x01, 0};
        kdebug("Значение операнда: %X (%B).\n", arg.value, arg.value);
        byte msb = findSetRightBit(arg.value);
        result.value = msb;
        if (*code)
            setTermArg(code, locals, args, result);
        else
            code++;
        return result;
    }
    else if (*code == 0x89) {
        kdebug("Найден оператор Match.\n");
        code ++;
        TermArg pkg = getTermArg(code, locals, args);
        if (pkg.type != 0x92) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x92, получен %X.\n", pkg.type);
            return errorArg;
        }
        byte matchOp1 = *code++;
        kdebug("Первый оператор совпадения: %d.\n", matchOp1);
        TermArg matchObj1 = getTermArg(code, locals, args);
        if (matchObj1.type != 0x01 && matchObj1.type != 0x8D && matchObj1.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", pkg.type);
            return errorArg;
        }
        byte matchOp2 = *code++;
        kdebug("Второй оператор совпадения: %d.\n", matchOp1);
        TermArg matchObj2 = getTermArg(code, locals, args);
        if (matchObj2.type != 0x01 && matchObj2.type != 0x8D && matchObj2.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 3\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", pkg.type);
            return errorArg;
        }
        TermArg start = getTermArg(code, locals, args);
        if (start.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 4\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", start.type);
            return errorArg;
        }
        kdebug("Индекс начала поиска: %D.\n", start.value);

        TermArg result = performMatch((byte*)pkg.value, matchOp1, matchObj1, matchOp2, matchObj2, start.value, locals, args);
        kdebug("Получено значение %D.\n", result.value);
        return result;
    }
    else if (*(word*)code == 0x295B) {
        kdebug("Найден оператор ToBCD.\n");
        code += 2;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            return errorArg;
        }
        TermArg result;
        result.type = 1;
        result.value = toBCD(arg.value);
        kdebug("Новое значение: %X.\n", result.value);
        if (*code)
            setTermArg(code, locals, args, result);
        else
            code++;
        return result;
    }
    else if (*(word*)code == 0x285B) {
        kdebug("Найден оператор FromBCD.\n");
        code += 2;
        TermArg arg = getTermArg(code, locals, args);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            return errorArg;
        }
        TermArg result;
        result.type = 1;
        result.value = fromBCD(arg.value);
        if (*code)
            setTermArg(code, locals, args, result);
        else
            code++;
        return result;
    }
    else if (*code == 0x9E) {
        kdebug("Найден оператор Mid.\n");
        code ++;
        TermArg midObj = getTermArg(code, locals, args);
        if (midObj.type != 0x8D && midObj.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x8D или 0x91, получен %X.\n", midObj.type);
            return errorArg;
        }
        TermArg start = getTermArg(code, locals, args);
        if (start.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", start.type);
            return errorArg;
        }
        kdebug("Начальный индекс: %D.\n", start.value);
        TermArg length = getTermArg(code, locals, args);
        if (length.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 3\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", length.type);
            return errorArg;
        }
        kdebug("Длина куска: %D Б.\n", length.value);
        TermArg slice = getSlice((byte*)midObj.value, start.value, length.value, tmpPtr);
        if (*code)
            setTermArg(code, locals, args, slice);
        else
            code++;
        
        return slice;
    }
}

void setTermArg(byte*& code, TermArg*& locals, TermArg*& args, TermArg newVal) {
    kdebug("Новое значение: %X.\n", newVal.value);
    if (!(*code)) {
        code ++;
        return;
    }
    if (isLeadNameChar(*code)) {
        word nameSegs = parseName(code);
        code += nameSegs >> 8;
        byte *addr = getACPIObjAddr((AMLName*)parsingPathBase, getParsingPathLen());
        if (!addr) {
            addr = getACPIObjAddr(parsingPath - 1, 1);
            if (!addr)
                return;
        }
        if (*addr == 0xFD) {
            kdebug("Запись производится в поле ");
            kdebug(*(parsingPath - 1));
            kdebug(".\n");
            writeField(*(parsingPath-1), newVal);
            return;
        }
        dword targetLen = getACPIObjLen((AMLName*)parsingPathBase, getParsingPathLen());
        dword valueLen = 0;
        bool isInteger = newVal.type == 0x01 || newVal.type == 0x7F;
        if (isInteger) {
            valueLen = getIntegerTermBytes(newVal.value);
            if (valueLen > targetLen) {
                remapACPIObj((AMLName*)parsingPathBase, getParsingPathLen(), acpiData);
                acpiData += encodeIntegerTerm(newVal.value, acpiData);
            } else {
                encodeIntegerTerm(newVal.value, addr);
            }
        }
        else {
            valueLen = getPkgLength((byte*)(newVal.value + 1)) + 1;
            if (valueLen > targetLen) {
                remapACPIObj((AMLName*)parsingPathBase, getParsingPathLen(), acpiData);
                memcpy((byte*)newVal.value, acpiData, valueLen);
                acpiData += valueLen;
            } else {
                memcpy((byte*)newVal.value, addr, valueLen);
            }
        }
    }
    else if (*code >= 0x60 && *code <= 0x66) {
        byte localNo = *code & 7;
        code++;
        kdebug("Запись производится в Local%d.\n", localNo);
        if (newVal.type == 0x01 || newVal.type == 0x7F) {
            kdebug("Устанавливается значение IntegerTerm.\n");
            locals[localNo] = newVal;
            kdebug("Значение = %D (%X).\n", locals[localNo].value, locals[localNo].value);
            return;
        }
        if (!locals[localNo].value) {
            kdebug("Устанавливается новое значение для Local%d.\n", localNo);
            locals[localNo] = newVal;
            kdebug("Значение = %D (%X).\n", locals[localNo].value, locals[localNo].value);
            return;
        }
        byte *ptr = (byte*)newVal.value;
        dword len = 0;
        if (*ptr == 0x0D) {
            kdebug("Новое значение - строка.\n");
            ptr++;
            len = strlen((char*)ptr) + 1;
            kdebug("Значение: \"");
            for (dword i = 0; i < len; i++)
                kdebug(ptr[i]);
            kdebug("\".\n");
            locals[localNo].type = 0x8D;
            ptr--;
        } else {
            ptr++;
            len = getPkgLength(ptr) + 1;
            locals[localNo].type = *(--ptr) | 0x80;
        }
        dword targetLen = getACPIObjLen((byte*)(locals[localNo].value));
        if (len > targetLen) {
            memcpy(ptr, tmpPtr, len);
            locals[localNo].value = (dword)tmpPtr;
            tmpPtr += len;
        }
        else {
            byte* targetPtr = (byte*)locals[localNo].value;
            memcpy(ptr, targetPtr, len);
        }
    }
}

qword runMethod(byte* code, dword length, byte argc, va_list argl) {
    TermArg *locals = (TermArg*)0x12500;
    for (byte i = 0; i < 7; i++) {
        locals[i] = {0x7F,0};
    }
    TermArg *args = (TermArg*)0x12600;
    for (byte i = 0; i < argc; i++) {
        args[i] = {0x7F, va_arg(argl, dword)};
    }
    tmpPtr = (byte*)0x801000;
    code++;
    dword addr = (dword)code;
    kdebug("Начало кода: %x.\n", addr);
    for (dword i = 0; i < length; i++) {
        kdebug("%x %x\n", i, *(code+i));
        if (code[i] == 0xA4) {
            kdebug("Найден оператор Return.\n");
            dword tmp = (dword)code + i;
            code += i + 1;
            TermArg retVal = getTermArg(code, locals, args);
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
            kdebug("Возвращается значение %D (%X).\n", retVal.value, retVal.value);
            return retVal.value;
        }
        else if (code[i] == 0x08) {
            kdebug("Найден оператор Name.\n");
            i++;
            i += nameACPIObj(code + i);
            i++;
        }
        else if (code[i] == 0x72 || code[i] == 0x74 || code[i] == 0x77 || code[i] == 0x78 || code[i] == 0x85 || code[i] == 0x79 || code[i] == 0x7A || code[i] == 0x7B || code[i] == 0x7C || code[i] == 0x7D || code[i] == 0x7E || code[i] == 0x7F) {
            byte opType = code[i];
            dword tmp = (dword)code + i;
            code += i;
            TermArg val = getTermArg(code, locals, args);
            if (val.value == maxqword) {
                return maxqword;
            }
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x75 || code[i] == 0x76 || code[i] == 0x80) {
            dword tmp = (dword)code + i;
            code += i;
            TermArg arg = getTermArg(code, locals, args);
            if (arg.value == maxqword)
                return maxqword;
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x70) {
            kdebug("Найден оператор Store.\n");
            dword tmp = (dword)code + i;
            code += i + 1;
            TermArg arg = getTermArg(code, locals, args);
            setTermArg(code, locals, args, arg);
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x97 || code[i] == 0x98 || code[i] == 0x99 || code[i] == 0x9C) {
            dword tmp = (dword)code + i;
            code += i;
            TermArg arg = getTermArg(code, locals, args);
            if (arg.value == maxqword)
                return maxqword;
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else {
            dword tmp = (dword)code + i;
            code += i;
            TermArg arg = getTermArg(code, locals, args);
            if (arg.value == maxqword)
                return maxqword;
            dword diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        i --;
    }
    return maxqword;
}