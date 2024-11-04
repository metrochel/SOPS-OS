#include "aml.hpp"
#include "../io/io.hpp"
#include "../pci/pci.hpp"
#include "../str/str.hpp"
#include "../timing/time.hpp"
#include "../dbg/dbg.hpp"

const TermArg errorArg = {maxqword, maxqword};
const TermArg continueArg = {0xAAAAAAAA, 0x55555555};
const TermArg breakArg = {0xBBBBBBBB, 0x55555555};
const TermArg loopContinueArg = {0xCCCCCCCC, 0x55555555};

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
    if (*buf++ == 0x11)
        buf += *buf >> 6;
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
    if (*buf++ == 0x11)
        buf += *buf >> 6;
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
    *addr++ = 0x22;
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
    *addr++ = 0x22;
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

TermArg performMatch(byte *pkg, byte op1, TermArg obj1, byte op2, TermArg obj2, dword start, FuncFrame *frame) {
    pkg ++;
    pkg += *pkg >> 6;
    pkg ++;
    byte length = *pkg++;
    kdebug("Число элементов: %d.\n", length);
    for (byte i = 0; i < length; i++) {
        TermArg arg = getTermArg(pkg, frame);
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
    dword i = 0;
    qword bcd = 0;
    while (value > 0) {
        bcd |= (value % 10) << (i*4);
        i++;
        value /= 10;
    }
    return bcd;
}

qword fromBCD(qword value) {
    dword i = 1;
    qword bin = 0;
    while (value > 0) {
        bin += (value & 0xF) * i;
        value >>= 4;
        i *= 10;
    }
    return bin;
}

TermArg getIndex(TermArg obj, dword index, FuncFrame *frame) {
    TermArg ret = {0xF1, 0};
    byte *ptr = (byte*)obj.value;
    if (obj.type == 0x91) {
        if (*ptr++ == 0x11)
            ptr += getPkgBytes(*ptr);
        dword len = getIntegerTerm(ptr);
        if (index >= len) {
            kdebug("ОШИБКА: Индекс вне буфера\n");
            kdebug("Индекс %d превысил длину буфера %d.\n", index, len);
            return errorArg;
        }
        ptr = (byte*)obj.value;
        ret.value = (qword)frame->tmpSpace;
        createBufferField(ptr, index * 8, 8, frame->tmpSpace);
        return ret;
    }
    if (obj.type == 0x8D) {
        ptr++;
        dword len = strlen((char*)ptr) + 1;
        if (index >= len) {
            kdebug("ОШИБКА: Индекс вне строки\n");
            kdebug("Индекс %d превысил длину строки %d.\n", index, len);
            return errorArg;
        }
        ptr = (byte*)obj.value;
        ret.value = (qword)frame->tmpSpace;
        createBufferField(ptr, index * 8, 8, frame->tmpSpace);
        return ret;
    }
    if (obj.type == 0x92) {
        bool isUpgraded = *ptr++ == 0x9A;
        if (!isUpgraded)
            ptr += getPkgBytes(*ptr);
        byte elements = *ptr++;
        if (index >= elements) {
            kdebug("ОШИБКА: Индекс вне пакета\n");
            kdebug("Индекс %d превысил длину пакета %d.\n", index, elements);
            return errorArg;
        }
        if (!isUpgraded) {
            for (dword i = 0; i < index; i++)
                getTermArg(ptr, frame);
            ret.value = (qword)ptr;
        }
        else {
            dword *els = (dword*)ptr;
            ret.value = els[2*index];
        }
        return ret;
    }
    return errorArg;
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

inline bool isMutexFree(byte* mutex) {
    return (*mutex >> 4) == 0;
}

byte acquireMutex(byte *mutex, word timeout, FuncFrame *frame) {
    mutex ++;
    byte ownLevel = *mutex >> 4;
    byte *ownAddr = *(byte**)(mutex + 1);
    kdebug("Адрес владельца: %x.\n", ownAddr);
    if ((*mutex & 0xF) > frame->syncLevel)
        return 0x0C;
    if (isMutexFree(mutex) || ownAddr == frame->addr) {
        if (ownLevel == 0xF)
            return 0xFF;
        if (ownLevel == 0)
            *(byte**)(mutex + 1) = frame->addr;
        *mutex += 16;
        return 0;
    }
    bool isTimed = timeout != 0xFFFF;
    qword ticks = ((qword)timeout * 1000000) / 40;
    for (qword i = 0; i < ticks || !isTimed; i++) {
        io_wait();
        if (isMutexFree(mutex)) {
            *mutex += 16;
            return 0;
        }
    }
    return 1;
}

byte releaseMutex(byte *mutex, FuncFrame *frame) {
    mutex ++;
    byte ownLevel = *mutex >> 4; 
    if (*(byte**)(mutex + 1) != frame->addr)
        return 1;
    if (ownLevel == 0)
        return 2;
    *mutex -= 16;
    if (ownLevel == 0)
        *(byte**)(mutex + 1) = 0;
    return 0;
}

byte waitEvent(byte *event, dword timeout) {
    event ++;
    if (*event) {
        *event = *event - 1;
        return 0;
    }
    bool timed = timeout < 0xFFFF;
    qword ticks = (timeout * 1000000) / 40;
    for (qword i = 0; i < ticks || !timed; i++) {
        io_wait();
        if (*event) {
            *event = *event - 1;
            return 0;
        }
    }
    return 1;
}

TermArg readField(byte* fieldAddr) {
    if (*fieldAddr != 0xFD)
        return {0,0};
    
    fieldAddr ++;
    byte flags = *fieldAddr++;
    byte space = *fieldAddr++;
    byte access = flags & 0xF;
    qword base = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);
    qword bitsOffset = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);
    qword size = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);

    kdebug("Вид доступа: %d.\n", access);
    kdebug("Основа поля: %X.\n", base);
    kdebug("Сдвиг по полю: %D бит.\n", bitsOffset);
    kdebug("Размер поля: %D бит.\n", size);
    if (access == 0) {
        if (bitsOffset + size < 8)
            access = 1;
        else if (bitsOffset + size < 16)
            access = 2;
        else if (bitsOffset + size < 32)
            access = 3;
        else if (bitsOffset + size < 64)
            access = 4;
        kdebug("Рассчитанный доступ: %d.\n", access);
    }
    qword byteOffset = base + bitsOffset / 8;
    kdebug("Адрес чтения: %X.\n", byteOffset);
    byte batchSize = 0;
    qword val = 0;
    kdebug("Пространство поля - ");
    switch (space) {
        case 0:
            kdebug("ОЗУ.\n");
            if (access == 1) {
                byte *read = (byte*)byteOffset;
                batchSize = 8;
                bitsOffset %= 8;
                val = *read;
            }
            else if (access == 2) {
                byte *read = (byte*)byteOffset;
                batchSize = 16;
                bitsOffset %= 16;
                val = (*read++ << 8);
                val |= *read++;
            }
            else if (access == 3) {
                byte *read = (byte*)byteOffset;
                batchSize = 32;
                bitsOffset %= 32;
                val = (*read++ << 24);
                val |= (*read++ << 16);
                val |= (*read++ << 8);
                val |= *read++;
            }
            else if (access == 4) {
                byte *read = (byte*)byteOffset;
                batchSize = 64;
                bitsOffset %= 64;
                val = ((qword)*read++ << 56);
                val |= ((qword)*read++ << 48);
                val |= ((qword)*read++ << 40);
                val |= ((qword)*read++ << 32);
                val |= (*read++ << 24);
                val |= (*read++ << 16);
                val |= (*read++ << 8);
                val |= *read++;
            }
            break;
        case 1:
            kdebug("I/O.\n");
            if (access == 1) {
                batchSize = 8;
                bitsOffset %= 8;
                val = inb(byteOffset);
            }
            else if (access == 2) {
                batchSize = 16;
                bitsOffset %= 16;
                word _val = inw(byteOffset);
                val = (_val & 0xFF00) >> 8;
                val |= (_val & 0xFF) << 8;
            }
            else if (access == 3) {
                batchSize = 32;
                bitsOffset %= 32;
                dword _val = inl(byteOffset);
                val = (_val & 0xFF000000) >> 24;
                val |= (_val & 0xFF0000) >> 8;
                val |= (_val & 0xFF00) << 8;
                val |= (_val & 0xFF) << 24;
            }
            break;
        case 2:
            kdebug("конфигурация PCI.\n");
            byte bus = (byteOffset >> 16) & 0xFF;
            byte device = (byteOffset >> 11) & 0x1F;
            byte function = (byteOffset >> 8) & 0x7;
            byte offset = byteOffset & 0xFF;
            if (access == 1) {
                batchSize = 8;
                bitsOffset %= 8;
                val = pciConfigReadB(bus, device, function, offset);
            }
            else if (access == 2) {
                batchSize = 16;
                bitsOffset %= 16;
                word _val = pciConfigReadW(bus, device, function, byteOffset);
                val = (_val & 0xFF00) >> 8;
                val |= (_val & 0xFF) << 8;
            }
            else if (access == 3) {
                batchSize = 32;
                bitsOffset %= 32;
                dword _val = pciConfigReadDW(bus, device, function, offset);
                val = (_val & 0xFF000000) >> 24;
                val |= (_val & 0xFF0000) >> 8;
                val |= (_val & 0xFF00) << 8;
                val |= (_val & 0xFF) << 24;
            }
    }
    if ((size == 8 || size == 16 || size == 32 || size == 64) && bitsOffset % 8 == 0) {
        kdebug("Полученное значение: %X.\n", val);
        return {0x01, val};
    }
    kdebug("Цепочка преобразований:\n");
    kdebug("%X -> ", val);
    val >>= batchSize - (bitsOffset + size);
    kdebug("%X (%d, %D, %D) -> ", val, batchSize, bitsOffset, size);
    qword mask = 0;
    for (byte i = 0; i < size; i++) {
        mask = (mask << 1) | 1;
    }
    val &= mask;
    kdebug("%X (%B).\n", val, mask);
    if (size > 8 && size < 16) {
        word _val = val;
        val = (_val & 0xFF00) >> 8;
        val |= (_val & 0xFF) << 8;
    }
    else if (size > 8 && size < 32) {
        dword _val = val;
        val = (_val & 0xFF000000) >> 24;
        val |= (_val & 0xFF0000) >> 8;
        val |= (_val & 0xFF00) << 8;
        val |= (_val & 0xFF) << 24;
    }
    else if (size > 8 && size < 64) {
        qword _val = val;
        byte *_ptr = (byte*)&val;
        for (byte i = 0; i < (size + 7) / 8; i++) {
            *_ptr++ = (_val >> (size - 8*i - 8)) & 0xFF;
        }
    }
    return {0x01, val};
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
    qword base = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);
    qword bitsOffset = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);
    qword size = getIntegerTerm(fieldAddr);
    fieldAddr += getIntegerTermBytes(fieldAddr);

    kdebug("Вид доступа: %d.\n", access);
    kdebug("Основа поля: %X.\n", base);
    kdebug("Сдвиг по полю: %D бит.\n", bitsOffset);
    kdebug("Размер поля: %D бит.\n", size);
    if (access == 0) {
        if (bitsOffset + size < 8)
            access = 1;
        else if (bitsOffset + size < 16)
            access = 2;
        else if (bitsOffset + size < 32)
            access = 3;
        else if (bitsOffset + size < 64)
            access = 4;
        kdebug("Рассчитанный доступ: %d.\n", access);
    }
    qword byteOffset = base + bitsOffset / 8;
    kdebug("Адрес записи: %X.\n", byteOffset);
    byte batchSize = 0;
    qword val = 0;
    kdebug("Пространство поля - ");
    switch (space) {
        case 0:
            kdebug("ОЗУ.\n");
            if (access == 1) {
                byte *read = (byte*)byteOffset;
                batchSize = 8;
                bitsOffset %= 8;
                val = *read;
            }
            else if (access == 2) {
                byte *read = (byte*)byteOffset;
                batchSize = 16;
                bitsOffset %= 16;
                val = (*read++ << 8);
                val |= *read++;
            }
            else if (access == 3) {
                byte *read = (byte*)byteOffset;
                batchSize = 32;
                bitsOffset %= 32;
                val = (*read++ << 24);
                val |= (*read++ << 16);
                val |= (*read++ << 8);
                val |= *read++;
            }
            else if (access == 4) {
                byte *read = (byte*)byteOffset;
                batchSize = 64;
                bitsOffset %= 64;
                val = ((qword)*read++ << 56);
                val |= ((qword)*read++ << 48);
                val |= ((qword)*read++ << 40);
                val |= ((qword)*read++ << 32);
                val |= (*read++ << 24);
                val |= (*read++ << 16);
                val |= (*read++ << 8);
                val |= *read++;
            }
            break;
        case 1:
            kdebug("I/O.\n");
            if (access == 1) {
                batchSize = 8;
                bitsOffset %= 8;
                val = inb(byteOffset);
            }
            else if (access == 2) {
                batchSize = 16;
                bitsOffset %= 16;
                word _val = inw(byteOffset);
                val = (_val & 0xFF00) >> 8;
                val |= (_val & 0xFF) << 8;
            }
            else if (access == 3) {
                batchSize = 32;
                bitsOffset %= 32;
                dword _val = inl(byteOffset);
                val = (_val & 0xFF000000) >> 24;
                val |= (_val & 0xFF0000) >> 8;
                val |= (_val & 0xFF00) << 8;
                val |= (_val & 0xFF) << 24;
            }
            break;
        case 2:
            kdebug("конфигурация PCI.\n");
            byte bus = (byteOffset >> 16) & 0xFF;
            byte device = (byteOffset >> 11) & 0x1F;
            byte function = (byteOffset >> 8) & 0x7;
            byte offset = byteOffset & 0xFF;
            if (access == 1) {
                batchSize = 8;
                bitsOffset %= 8;
                val = pciConfigReadB(bus, device, function, offset);
            }
            else if (access == 2) {
                batchSize = 16;
                bitsOffset %= 16;
                word _val = pciConfigReadW(bus, device, function, byteOffset);
                val = (_val & 0xFF00) >> 8;
                val |= (_val & 0xFF) << 8;
            }
            else if (access == 3) {
                batchSize = 32;
                bitsOffset %= 32;
                dword _val = pciConfigReadDW(bus, device, function, offset);
                val = (_val & 0xFF000000) >> 24;
                val |= (_val & 0xFF0000) >> 8;
                val |= (_val & 0xFF00) << 8;
                val |= (_val & 0xFF) << 24;
            }
            break;
    }
    if ((size != 8 && size != 16 && size != 32 && size != 64) || size % 8 != 0) {
        qword mask = 0;
        for (byte i = 0; i < size; i++) {
            mask = (mask << 1) | 1;
        }
        byte shift = batchSize - (bitsOffset + size);
        mask <<= shift;
        kdebug("Вычисленная маска: %B.\n", mask);
        byte update = (flags & 0x60) >> 5;
        kdebug("Правило обновления: ");
        if (update == 1) {
            kdebug("Записывать единицами.\n");
            val = maxqword;
            val &= ~mask;
        }
        else if (update == 2) {
            kdebug("Записывать нулями.\n");
            val = 0;
        }
        else {
            kdebug("Сохранять.\n");
            val &= ~mask;
        }
        val |= (value.value) << shift;
    }
    else {
        val = value.value;
    }

    switch (space) {
        case 0:
            if (access == 1) {
                *(byte*)byteOffset = val & 0xFF;
            }
            else if (access == 2) {
                *(byte*)byteOffset = (val >> 8) & 0xFF;
                *(byte*)(byteOffset + 1) = val & 0xFF;
            }
            else if (access == 3) {
                *(byte*)byteOffset = (val >> 24) & 0xFF;
                *(byte*)(byteOffset + 1) = (val >> 16) & 0xFF;
                *(byte*)(byteOffset + 2) = (val >> 8) & 0xFF;
                *(byte*)(byteOffset + 3) = val & 0xFF;
            }
            else if (access == 4) {
                *(byte*)byteOffset = (val >> 56) & 0xFF;
                *(byte*)(byteOffset + 1) = (val >> 48) & 0xFF;
                *(byte*)(byteOffset + 2) = (val >> 40) & 0xFF;
                *(byte*)(byteOffset + 3) = (val >> 32) & 0xFF;
                *(byte*)(byteOffset + 4) = (val >> 24) & 0xFF;
                *(byte*)(byteOffset + 5) = (val >> 16) & 0xFF;
                *(byte*)(byteOffset + 6) = (val >> 8) & 0xFF;
                *(byte*)(byteOffset + 7) = val & 0xFF;
            }
            break;
        case 1:
            if (access == 1) {
                outb(byteOffset, val & 0xFF);
            }
            if (access == 2) {
                word _val = (val >> 8) & 0xFF;
                _val |= (val & 0xFF) << 8;
                outw(byteOffset, _val);
            }
            if (access == 3) {
                dword _val = (val >> 24) & 0xFF;
                _val |= (val & 0xFF0000) >> 8;
                _val |= (val & 0xFF00) << 8;
                _val |= (val & 0xFF) << 24;
                outl(byteOffset, _val);
            }
            break;
        case 2:
            byte bus = (byteOffset >> 16) & 0xFF;
            byte device = (byteOffset >> 11) & 0x1F;
            byte function = (byteOffset >> 8) & 0x7;
            byte offset = byteOffset & 0xFF;
            if (access == 1) {
                pciConfigWriteB(bus, device, function, offset, val & 0xFF);
            }
            if (access == 2) {
                word _val = (val >> 8) & 0xFF;
                _val |= (val & 0xFF) << 8;
                pciConfigWriteW(bus, device, function, offset, _val);
            }
            if (access == 3) {
                dword _val = (val >> 24) & 0xFF;
                _val |= (val & 0xFF0000) >> 8;
                _val |= (val & 0xFF00) << 8;
                _val |= (val & 0xFF) << 24;
                pciConfigWriteDW(bus, device, function, offset, _val);
            }
            break;
    }
}

void createBufferField(byte *buf, dword offset, dword size, byte *&dest) {
    kdebug("Адрес буфера: %x.\n", buf);
    if (*buf++ == 0x11) {
        buf += getPkgBytes(*buf);
    }
    buf += getIntegerTermBytes(buf);
    *acpiNamespace++ = varPathLen;
    memcpy((byte*)varPathBase, (byte*)acpiNamespace, varPathLen * 4);
    acpiNamespace += varPathLen;
    *acpiNamespace++ = (dword)dest;
    dword addr = (dword)dest;
    *dest++ = 0xFD;
    switch (size) {
        case 8: *dest++ = 1; break;
        case 16: *dest++ = 2; break;
        case 32: *dest++ = 3; break;
        case 64: *dest++ = 4; break;
        default: *dest++ = 0; break;
    }
    *dest++ = 0;
    dest += encodeIntegerTerm((qword)buf, dest);
    dest += encodeIntegerTerm(offset, dest);
    dest += encodeIntegerTerm(size, dest);
    *acpiNamespace++ = (dword)dest - addr;
}

void writeField(AMLName fieldName, TermArg value) {
    byte *addr = getACPIObjAddr(&fieldName, 1);
    writeField(addr, value);
}

TermArg castTermArg(TermArg value, byte type, FuncFrame *frame) {
    if (value.type == type || type == 0x7F)
        return value;

    kdebug("Тип аргумента: %X.\n", value.type);
    kdebug("Искомый тип: %x.\n", type);
    if (value.type == 0x01) {
        if (type == 0x8D) {
            dword addr = (dword)frame->tmpSpace;
            *frame->tmpSpace++ = 0x0D;
            intToStrHex(value.value, frame->tmpSpace);
            return {0x8D, addr};
        }
        if (type == 0x91) {
            dword addr = (dword)frame->tmpSpace;
            dword length;
            if (defBlockRevision == 1)
                length = 4;
            else
                length = 8;
            intToBuf(value.value, length, frame->tmpSpace);
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
            dword addr = (dword)frame->tmpSpace;
            dword len = 0;
            byte *ptr = (byte*)value.value;
            while (*ptr) {
                len ++;
                ptr++;
            }
            strToBuf((byte*)value.value, len, frame->tmpSpace);
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
            dword addr = (dword)frame->tmpSpace;
            bufToStr((byte*)value.value, 0, frame->tmpSpace);
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

    if (type == 0x8D) {
        if (value.type == 0x7F) {
            dword addr = (dword)frame->tmpSpace;
            frame->tmpSpace += strcpy((char*)"\x0D[Uninitialized Object]", (char*)frame->tmpSpace);
            *frame->tmpSpace++ = 0;
            return {0x8D, addr};
        }
        if (value.type == 0x82) {
            dword addr = (dword)frame->tmpSpace;
            frame->tmpSpace += strcpy((char*)"\x0D[Device]", (char*)frame->tmpSpace);
            *frame->tmpSpace++ = 0;
            return {0x8D, addr};
        }
        if (value.type == 0x83) {
            dword addr = (dword)frame->tmpSpace;
            frame->tmpSpace += strcpy((char*)"\x0D[Processor]", (char*)frame->tmpSpace);
            *frame->tmpSpace++ = 0;
            return {0x8D, addr};
        }
        if (value.type == 0x85) {
            dword addr = (dword)frame->tmpSpace;
            frame->tmpSpace += strcpy((char*)"\x0D[Thermal Zone]", (char*)frame->tmpSpace);
            *frame->tmpSpace++ = 0;
            return {0x8D, addr};
        }
        if (value.type == 0x92) {
            dword addr = (dword)frame->tmpSpace;
            frame->tmpSpace += strcpy((char*)"\x0D[Package]", (char*)frame->tmpSpace);
            *frame->tmpSpace++ = 0;
            return {0x8D, addr};
        }
    }
    
    kdebug("ОШИБКА: Провал преобразования\n");
    kdebug("Невозможно преобразовать тип %X в тип %x.\n", value.type, type);
    return errorArg;
}

byte identifyTermArg(byte b) {
    switch (b) {
        case 0x0A: return 0x01;
        case 0x0B: return 0x01;
        case 0x0C: return 0x01;
        case 0x0E: return 0x01;
        case 0x00: return 0x01;
        case 0x01: return 0x01;
        case 0xFF: return 0x01;
        case 0x0D: return 0x8D;
        case 0x11: return 0x91;
        case 0x12: return 0x92;
        case 0xFD: return 0xFD;
        default: return 0xFF;
    }
}

byte recDepth = 0;
TermArg getTermArg(byte*& code, FuncFrame* frame) {
    recDepth++;
    if (recDepth > 0x20) {
        kerror("ОШИБКА: Переполнение рекурсии\n");
        recret errorArg;
    }
    TermArg val = {0,0};
    if (isLeadNameChar(*code)) {
        kdebug("Тип значения - ссылка.\n");
        word nameSegs = getName(code);
        code += nameSegs >> 8;
        kdebug("Абсолютный адрес объекта: ");
        logVarPath();
        kdebugnewl();
        byte *addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        if (!addr) {
            addr = getACPIObjAddr(varPath - 1, 1);
            if (!addr) {
                recret {0,0};
            }
            else {
                kdebug("Ссылка указывает на поле ");
                kdebug(*(varPath - 1));
                kdebug(".\n");
            }
        }
        kdebug("Первый байт объекта: %x.\n", *addr);
        if (*addr == 0x14) {
            kdebug("Ссылка указывает на метод.\n");
            addr ++;
            dword length = getIntegerTerm(addr);
            addr += getIntegerTermBytes(addr);
            byte flags = *addr++;
            byte argc = flags & 7;
            kdebug("Число аргументов: %d.\n", argc);
            byte syncLevel = (flags >> 4) & 0xF;
            kdebug("Уровень синхронизации: %d.\n", syncLevel);
            qword args[] = {0,0,0,0,0,0,0};
            for (byte i = 0; i < argc; i++) {
                args[i] = getTermArg(code, frame).value;
            }
            TermArg res = {0x7F, 0};
            res = runMethod(addr, length, syncLevel, frame);
            recret res;
        }
        recret getTermArg(addr, frame);
    }
    if (*code == 0x0A || *code == 0x0B || *code == 0x0C || *code == 0x0E || *code == 0x00 || *code == 0x01 || *code == 0xFF) {
        kdebug("Тип значения - IntegerTerm.\n");
        val = {0x01, getIntegerTerm(code)};
        kdebug("Значение = %D (%X).\n", val.value, val.value);
        code += getIntegerTermBytes(code);
        recret val;
    }
    else if (*code == 0x0D) {
        kdebug("Тип значения - String.\n");
        val = {0x8D, (qword)code};
        kdebug("Значение = \"");
        code++;
        while (*code != 0) {
            kdebug(*code++);
        }
        code++;
        kdebug("\" (%X).\n", val.value);
        recret val;
    }
    else if (*code == 0x11) {
        kdebug("Тип значения - Buffer.\n");
        val = {0x91, (qword)code};
        parseBuffer(code);
        code ++;
        dword len = getPkgLength(code);
        kdebug("Длина буфера: %d Б.\n", len);
        code += len;
        recret val;
    }
    else if (*code == 0x22) {
        kdebug("Тип значения - Buffer+.\n");
        val = {0x91, (qword)code};
        parseBuffer(code);
        code ++;
        dword len = getIntegerTerm(code);
        kdebug("Длина буфера: %d Б.\n", len);
        code += getIntegerTermBytes(code);
        code += len;
        recret val;
    }
    else if (*code == 0x12) {
        kdebug("Тип значения - Package.\n");
        val = {0x92, (qword)frame->tmpSpace};
        code ++;
        dword len = parsePackage(code);
        code --;
        dword upgradeLen = upgradePackage(code, frame->tmpSpace, frame);
        code += len + 1;
        if (!upgradeLen) {
            kdebug("ОШИБКА: Обновление пакета провалено\n");
            recret errorArg;
        }
        recret val;
    }
    else if (*code == 0x9A) {
        kdebug("Тип значения - Package+.\n");
        val = {0x92, (qword)code};
        code ++;
        byte elements = *code++;
        kdebug("Число элементов: %d.\n", elements);
        code += elements * 8;
        dword *table = (dword*)code;
        for (byte i = 0; i < elements; i++) {
            code += table[2*i + 1];
        }
        recret val;
    }
    else if (*code >= 0x60 && *code <= 0x66) {
        byte localNo = *code & 0x7;
        kdebug("Значение в Local%d.\n", localNo);
        val = frame->locals[localNo];
        code++;
        recret val;
    }
    else if (*code >= 0x67 && *code <= 0x6E) {
        byte argNo = (*code + 1) & 0x7;
        kdebug("Значение в Arg%d.\n", argNo);
        val = frame->args[argNo];
        code++;
        recret val;
    }
    else if (*code == 0xFD) {
        kdebug("Значение в поле.\n");
        recret readField(code);
    }
    else if (*code == 0x72 || *code == 0x74 || *code == 0x77 || *code == 0x78 || *code == 0x85 || *code == 0x79 || *code == 0x7A || *code == 0x7B || *code == 0x7C || *code == 0x7D || *code == 0x7E || *code == 0x7F) {
        kdebug("Найден арифметический оператор (%x).\n", *code);
        byte opType = *code++;
        TermArg op1 = getTermArg(code, frame);
        TermArg op2 = getTermArg(code, frame);
        if (op1.type != 0x01 && op1.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 1\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", op1.type);
            recret errorArg;
        }
        if (op2.type != 0x01 && op2.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 2\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", op2.type);
            recret errorArg;
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
        if (opType == 0x78)
            setTermArg(code, frame, result);
        setTermArg(code, frame, result);
        recret result;
    }
    else if (*code == 0x75 || *code == 0x76 || *code == 0x80) {
        kdebug("Найден арифметический оператор (%x).\n", *code);
        byte opType = *code++;
        byte *_code = code;
        TermArg value = getTermArg(code, frame);
        if (value.type != 0x01 && value.type != 0x7F) {
            kdebug("ОШИБКА: Несоответствующий тип операнда 1\n");
            kdebug("Ожидался тип 0x01 или 0x7F, получен %X.\n", value.type);
            recret errorArg;
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
        code = _code;
        setTermArg(code, frame, value);
        setTermArg(code, frame, value);
        recret value;
    }
    else if (*code == 0x97) {
        kdebug("Найден оператор ToDecimalString.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            recret arg;
        }
        dword addr = (dword)frame->tmpSpace;
        TermArg ret = {0x8D, addr};
        *frame->tmpSpace++ = 0x0D;
        if (arg.type == 0x01) {
            kdebug("Тип операнда - IntegerTerm.\n");
            intToStrDec(arg.value, frame->tmpSpace);
            setTermArg(code, frame, ret);
            recret ret;
        }
        else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            bufToStrDec((byte*)arg.value, frame->tmpSpace);
            setTermArg(code, frame, ret);
            recret ret;
        }
        else {
            kdebug("ОШИБКА: Неправильный тип операнда\n");
            kdebug("Ожидался тип 0x01, 0x0D или 0x91, получен %X.\n", arg.type);
            recret errorArg;
        }
    }
    else if (*code == 0x98) {
        kdebug("Найден оператор ToHexString.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            recret arg;
        }
        dword addr = (dword)frame->tmpSpace;
        TermArg ret = {0x8D, addr};
        *frame->tmpSpace++ = 0x0D;
        if (arg.type == 0x01) {
            kdebug("Тип операнда - IntegerTerm.\n");
            intToStrHex(arg.value, frame->tmpSpace);
            setTermArg(code, frame, ret);
            recret ret;
        }
        else if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            bufToStrHex((byte*)arg.value, frame->tmpSpace);
            setTermArg(code, frame, ret);
            recret ret;
        }
        else {
            kdebug("ОШИБКА: Неправильный тип операнда\n");
            kdebug("Ожидался тип 0x01, 0x0D или 0x91, получен %X.\n", arg.type);
            recret errorArg;
        }
    }
    else if (*code == 0x9C) {
        kdebug("Найден оператор ToString.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x91, получен %X.\n", arg.type);
        }
        dword addr = (dword)frame->tmpSpace;
        TermArg length = getTermArg(code, frame);
        if (length.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
        }
        bufToStr((byte*)arg.value, length.value, frame->tmpSpace);
        TermArg str = {0x8D, addr};
        setTermArg(code, frame, str);
        recret str;
    }
    else if (*code == 0x99) {
        kdebug("Найден оператор ToInteger.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type == 0x01) {
            kdebug("Тип операнда - Integer.\n");
            recret arg;
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
            recret errorArg;
        }
        TermArg ret = {0x01, out};
        setTermArg(code, frame, ret);
        recret ret;
    }
    else if (*code == 0x96) {
        kdebug("Найден оператор ToBuffer.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type == 0x91) {
            kdebug("Тип операнда - Buffer.\n");
            recret arg;
        }
        val = {0x91, (qword)frame->tmpSpace};
        if (arg.type == 0x8D) {
            kdebug("Тип операнда - String.\n");
            dword length = strlen((char*)arg.value);
            strToBuf((byte*)arg.value, length, frame->tmpSpace);
        }
        else if (arg.type == 0x01) {
            kdebug("Тип операнда - Integer.\n");
            dword length;
            if (defBlockRevision == 1)
                length = 4;
            else
                length = 8;
            intToBuf(arg.value, length, frame->tmpSpace);
        }
        else {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", arg.type);
            recret errorArg;
        }
        setTermArg(code, frame, val);
        recret val;
    }
    else if (*code >= 0x90 && *code <= 0x95) {
        byte opType = *code++;
        kdebug("Найден логический оператор (%x).\n", opType);
        TermArg op1 = getTermArg(code, frame);
        TermArg op2 = {0,0};
        if (op1.type != 0x01 && op1.type != 0x02 && op1.type != 0x8D && op1.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, 0x02, 0x8D или 0x91, получен %X.\n", op1.type);
            recret errorArg;
        }
        if (opType != 0x92) {
            op2 = getTermArg(code, frame);
            if (op2.type != 0x01 && op2.type != 0x02 && op2.type != 0x8D && op2.type != 0x91) {
                kdebug("ОШИБКА: Неправильный тип операнда 2\n");
                kdebug("Ожидался тип 0x01, 0x02, 0x8D или 0x91, получен %X.\n", op2.type);
                recret errorArg;
            }
        }
        if (op2.type != 0)
            op2 = castTermArg(op2, op1.type, frame);
        if (op2.value == maxqword) {
            recret errorArg;
        }
        TermArg result = {0x02, 0};
        kdebug("Тип операции: ");
        if (opType == 0x90) {
            kdebug("LAnd.\n");
            result.value = (op1.value && op2.value) ? 1 : 0;
            recret result;
        }
        if (opType == 0x91) {
            kdebug("LOr.\n");
            result.value = (op1.value || op2.value) ? 1 : 0;
            recret result;
        }
        if (opType == 0x92) {
            kdebug("LNot.\n");
            result.value = (!op1.value) ? 1 : 0;
            recret result;
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
        recret result;
    }
    else if (*code == 0x81) {
        kdebug("Найден оператор FindSetLeftBit.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            recret errorArg;
        }
        TermArg result = {0x01, 0};
        kdebug("Значение операнда: %X (%B).\n", arg.value, arg.value);
        byte msb = findSetLeftBit(arg.value);
        result.value = msb;
        setTermArg(code, frame, result);
        recret result;
    }
    else if (*code == 0x82) {
        kdebug("Найден оператор FindSetRightBit.\n");
        code++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            recret errorArg;
        }
        TermArg result = {0x01, 0};
        kdebug("Значение операнда: %X (%B).\n", arg.value, arg.value);
        byte msb = findSetRightBit(arg.value);
        result.value = msb;
        setTermArg(code, frame, result);
        recret result;
    }
    else if (*code == 0x89) {
        kdebug("Найден оператор Match.\n");
        code ++;
        TermArg pkg = getTermArg(code, frame);
        if (pkg.type != 0x92) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x92, получен %X.\n", pkg.type);
            recret errorArg;
        }
        byte matchOp1 = *code++;
        kdebug("Первый оператор совпадения: %d.\n", matchOp1);
        TermArg matchObj1 = getTermArg(code, frame);
        if (matchObj1.type != 0x01 && matchObj1.type != 0x8D && matchObj1.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", pkg.type);
            recret errorArg;
        }
        byte matchOp2 = *code++;
        kdebug("Второй оператор совпадения: %d.\n", matchOp1);
        TermArg matchObj2 = getTermArg(code, frame);
        if (matchObj2.type != 0x01 && matchObj2.type != 0x8D && matchObj2.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 3\n");
            kdebug("Ожидался тип 0x01, 0x8D или 0x91, получен %X.\n", pkg.type);
            recret errorArg;
        }
        TermArg start = getTermArg(code, frame);
        if (start.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 4\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", start.type);
            recret errorArg;
        }
        kdebug("Индекс начала поиска: %D.\n", start.value);

        TermArg result = performMatch((byte*)pkg.value, matchOp1, matchObj1, matchOp2, matchObj2, start.value, frame);
        kdebug("Получено значение %D.\n", result.value);
        recret result;
    }
    else if (*code == 0x71) {
        kdebug("Найден оператор RefOf.\n");
        code ++;
        TermArg res = {0xF1, 0};
        TermArg target = {0,0};
        kdebug("Цель - ");
        if (*code >= 0x60 && *code <= 0x66) {
            byte localNo = *code & 7;
            kdebug("Local%d.\n", localNo);
            target.value = (qword)&frame->locals[localNo];
            target.type = 0xF1;
            code ++;
        }
        else if (*code >= 0x67 && *code <= 0x6E) {
            byte argNo = *code & 7;
            kdebug("Arg%d.\n", argNo);
            target.value = (qword)&frame->args[argNo];
            target.type = 0x81;
            code ++;
        }
        else if (isLeadNameChar(*code)) {
            kdebug("ссылка.\n");
            word nameSegs = getName(code);
            code += nameSegs >> 8;
            kdebug("Абсолютный путь к цели: ");
            logVarPath();
            kdebugnewl();
            byte *addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
            if (!addr) {
                kdebug("ОШИБКА: Такой объект не существует\n");
                recret errorArg;
            }
            res.value = (qword)addr;
        }
        else if (*code == 0x71) {
            kdebug("ObjectReference.\n");
            dword addr = (dword)frame->tmpSpace;
            code ++;
            kdebug("Адрес ссылки: %x.\n", *(dword*)code);
            *frame->tmpSpace++ = 0x71;
            *(dword*)frame->tmpSpace = *(dword*)code;
            frame->tmpSpace += 4;
            res.value = addr;
        }
        else {
            kdebug("<%x>.\n", *code);
            kdebug("ОШИБКА: Неизвестный байт\n");
            recret errorArg;
        }

        if (target.type) {
            kdebug("Адрес-цель ссылки: %x.\n", target.value);
            recret target;
        }

        kdebug("Адрес-цель ссылки: %x.\n", res.value);
        recret res;
    }
    else if (*(word*)code == 0x125B) {
        kdebug("Найден оператор CondRefOf.\n");
        code += 2;
        kdebug("Создаётся оператор RefOf.\n");

        byte *ptr = frame->tmpSpace;
        *ptr++ = 0x71;
        if (*code == 0x2E) {
            memcpy(code, ptr, 9);
            code += 9;
        }
        else if (*code == 0x2F) {
            memcpy(code, ptr, 2 + *(code + 1));
            code += 2 + (*code - 1);
        }
        else {
            memcpy(code, ptr, 4);
            code += 4;
        }
        ptr --;
        val = getTermArg(ptr, frame);
        kdebug("Удалось получить ссылку? ");
        if (val.type == errorArg.type) {
            kdebug("Нет\n");
            kdebug("Возвращается значение False.\n");
            setTermArg(code, frame, val);
            recret {0x02, 0};
        } else {
            kdebug("Да\n");
            kdebug("Возвращается значение %X.\n", val.value);
            setTermArg(code, frame, val);
            recret val;
        }
    }
    else if (*code == 0x83) {
        kdebug("Найден оператор DerefOf.\n");
        code ++;
        TermArg res = {0,0};
        if (*code == 0x0D) {
            code ++;
            varPath = (AMLName*)varPathBase;
            memcpy((byte*)parsingPathBase, (byte*)varPath, parsingPathLen * 4);
            varPath += parsingPathLen; 
            byte *addr = code;
            word len = 0;
            AMLName seg = 0;
            byte i = 0;
            while (*addr) {
                if (*addr == '\\') {
                    clearVarPath();
                    addr ++;
                    len ++;
                    continue;
                }
                if (*addr == '^') {
                    if (varPath == (AMLName*)varPathBase) {
                        kdebug("ОШИБКА: Слишком много символов ^ в имени\n");
                        recret errorArg;
                    }
                    varPath--;
                    addr ++;
                    len ++;
                    continue;
                }
                byte *ptr = (byte*)&seg;
                if (*addr == '.') {
                    while (i < 4) {
                        ptr[i] = '_';
                        i ++;
                    }
                    *varPath++ = seg;
                    i = 0;
                    seg = 0;
                    continue;
                }
                ptr[i] = *addr++;
                len++;
                i ++;
            }
            kdebug("Вычисленный путь: ");
            logVarPath();
            kdebugnewl();
        }
        else if (isLeadNameChar(*code)) {
            word nameSegs = getName(code);
            kdebug("Вычисленный путь: ");
            logVarPath();
            kdebugnewl();
        }
        else {
            res = getTermArg(code, frame);
        }

        if (res.type != 0xF1) {
            kdebug("ОШИБКА: Неверный тип операнда 1\n");
            kdebug("Ожидался тип 0xF1, получен %X.\n", res.type);
            recret errorArg;
        }

        byte *addr;
        if (res.type) {
            addr = (byte*)res.value;
        }
        else
            addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        kdebug("Адрес объекта: %x.\n", addr);

        byte type = identifyTermArg(*addr);
        if (~type & 0x80)
            res.value = getIntegerTerm(addr);
        else if (type == 0xFD) {
            kdebug("Ссылка указывает на поле.\n");
            res = readField(addr);
        }
        else
            res.value = (qword)addr;
        kdebug("Целевой аргумент:\n");
        kdebug("\tТип: %X\n", res.type);
        kdebug("\tЗначение: %X\n", res.value);
        recret res;
    }
    else if (*code == 0x9D) {
        kdebug("Найден оператор CopyObject.\n");
        code ++;

        val = getTermArg(code, frame);
        setTermArg(code, frame, val);
        recret val;
    }
    else if (*(word*)code == 0x295B) {
        kdebug("Найден оператор ToBCD.\n");
        code += 2;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            recret errorArg;
        }
        TermArg result;
        result.type = 1;
        result.value = toBCD(arg.value);
        kdebug("Новое значение: %X.\n", result.value);
        setTermArg(code, frame, result);
        recret result;
    }
    else if (*(word*)code == 0x285B) {
        kdebug("Найден оператор FromBCD.\n");
        code += 2;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
            recret errorArg;
        }
        TermArg result;
        result.type = 1;
        result.value = fromBCD(arg.value);
        setTermArg(code, frame, result);
        recret result;
    }
    else if (*code == 0x88) {
        kdebug("Найден оператор Index.\n");
        code ++;
        TermArg arg = getTermArg(code, frame);
        if (arg.type != 0x91 && arg.type != 0x92 && arg.type != 0x8D) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x91, 0x92 или 0x8D, получен %X.\n", arg.type);
            return errorArg;
        }

        TermArg index = getTermArg(code, frame);
        if (index.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", index.type);
            return errorArg;
        }

        val = getIndex(arg, index.value, frame);
        kdebug("Ссылка указывает на адрес %X.\n", val.value);
        setTermArg(code, frame, val);
        return val;
    }
    else if (*code == 0x9E) {
        kdebug("Найден оператор Mid.\n");
        code ++;
        TermArg midObj = getTermArg(code, frame);
        if (midObj.type != 0x8D && midObj.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x8D или 0x91, получен %X.\n", midObj.type);
            recret errorArg;
        }
        TermArg start = getTermArg(code, frame);
        if (start.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 2\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", start.type);
            recret errorArg;
        }
        kdebug("Начальный индекс: %D.\n", start.value);
        TermArg length = getTermArg(code, frame);
        if (length.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 3\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", length.type);
            recret errorArg;
        }
        kdebug("Длина куска: %D Б.\n", length.value);
        TermArg slice = getSlice((byte*)midObj.value, start.value, length.value, frame->tmpSpace);
        setTermArg(code, frame, slice);
        
        recret slice;
    }
    else if (*(word*)code == 0x235B) {
        kdebug("Найден оператор Acquire.\n");
        code += 2;
        word nameSegs = getName(code);
        kdebug("Абсолютный путь к мьютексу: ");
        logVarPath();
        kdebugnewl();
        byte* addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        code += nameSegs >> 8;
        word timeout = *(word*)code;
        kdebug("Тайм-аут: %d мс.\n", timeout);
        code += 2;
        byte res = acquireMutex(addr, timeout, frame);
        kdebug("Удалось заполучить мьютекс? ");
        kdebug(res ? "Нет\n" : "Да\n");
        if (res > 1) {
            kdebug("ОШИБКА: Захват мьютекса завершился с ошибкой\n");
            if (res == 0xFF)
                kdebug("Произошло переполнение захватов.\n");
            if (res == 0x0C)
                kdebug("Уровень синхронизации мьютекса выше, чем у метода.\n");
            recret errorArg;
        }
        recret {0x02, res};
    }
    else if (*(word*)code == 0x275B) {
        kdebug("Найден оператор Release.\n");
        code += 2;
        word nameSegs = getName(code);
        kdebug("Абсолютный путь к мьютексу: ");
        logVarPath();
        kdebugnewl();
        byte* addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        code += nameSegs >> 8;
        byte res = releaseMutex(addr, frame);
        kdebug("Удалось освободить мьютекс? ");
        kdebug(res ? "Нет\n" : "Да\n");
        if (res > 2) {
            kdebug("ОШИБКА: Освобождение мьютекса завершилось с ошибкой\n");
            if (res == 1)
                kdebug("Мьютекс принадлежит другому процессу.\n");
            if (res == 0x0C)
                kdebug("Мьютекс уже свободен.\n");
            recret errorArg;
        }
        recret {0x02, res};
    }
    else if (*(word*)code == 0x255B) {
        kdebug("Найден оператор Wait.\n");
        code += 2;
        word nameSegs = getName(code);
        kdebug("Абсолютный путь к событию: ");
        logVarPath();
        kdebugnewl();
        code += nameSegs >> 8;
        byte* addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        if (*addr != 0x02) {
            kdebug("ОШИБКА: Объект не является событием\n");
            recret errorArg;
        }
        kdebug("Адрес события: %x.\n", addr);
        TermArg timeout = getTermArg(code, frame);
        if (timeout.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип операнда 1\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", timeout.type);
            recret errorArg;
        }
        byte wait = waitEvent(addr, timeout.value);
        kdebug("Удалось дождаться события? ");
        kdebug(wait ? "Нет\n" : "Да\n");
        recret {0x02, wait};
    }
    else if (*code == 0x87) {
        kdebug("Найден оператор SizeOf.\n");
        code ++;
        word nameSegs = getName(code);
        code += nameSegs >> 8;
        kdebug("Абсолютный путь к цели: ");
        logVarPath();
        kdebugnewl();
        TermArg res = {0x01, 0};
        dword size = getACPIObjLen((AMLName*)varPathBase, varPathLen);
        res.value = size;
        recret res;
    }
    else if (*code == 0x73) {
        kdebug("Найден оператор Concatenate.\n");
        code++;
        TermArg arg1 = getTermArg(code, frame);
        TermArg arg2 = getTermArg(code, frame);
        TermArg res = {0, 0};
        if (arg1.type == 0x01 || arg1.type == 0x91)
            res.type = 0x91;
        else
            res.type = 0x8D;

        if (arg1.type == 0x01)
            arg1 = castTermArg(arg1, 0x91, frame);
        else if (arg1.type != 0x91)
            arg1 = castTermArg(arg1, 0x8D, frame);
        if (arg1.type == errorArg.type) {
            recret errorArg;
        }
        
        if (arg2.type != arg1.type)
            arg2 = castTermArg(arg2, arg1.type, frame);
        if (arg2.type == errorArg.type) {
            recret errorArg;
        }

        kdebug("Тип результата: ");
        
        if (res.type == 0x91) {
            kdebug("Buffer.\n");
            dword length = 0;
            dword len1 = getACPIObjLen((byte*)arg1.value);
            dword len2 = getACPIObjLen((byte*)arg2.value);
            length = len1 + len2;
            kdebug("Длина итогового буфера: %d Б.\n", length);

            res.value = (qword)frame->tmpSpace;
            byte *ptr = frame->tmpSpace;
            *ptr++ = 0x22;
            ptr += encodeIntegerTerm(length, ptr);
            byte *data1 = (byte*)arg1.value;
            if (*data1++ == 0x11)
                data1 += getPkgBytes(*data1);
            data1 += getIntegerTermBytes(data1);
            byte *data2 = (byte*)arg2.value;
            if (*data2++ == 0x11)
                data2 += getPkgBytes(*data2);
            data2 += getIntegerTermBytes(data2);
            memcpy(data1, ptr, len1);
            ptr += len1;
            memcpy(data2, ptr, len2);
            ptr += len2;

            frame->tmpSpace = ptr;
        }
        else {
            kdebug("String.\n");
            char *ptr1 = (char*)arg1.value;
            char *ptr2 = (char*)arg2.value;
            ptr1++;
            ptr2++;

            res.value = (qword)frame->tmpSpace;
            char *ptr = (char*)frame->tmpSpace;
            *ptr++ = 0x0D;
            ptr += strcpy(ptr1, ptr);
            if (!*(ptr-1))
                ptr --;
            ptr += strcpy(ptr2, ptr);
            *ptr++ = 0;
            kdebug("Полученное значение: \"");
            kdebug((const char*)(frame->tmpSpace + 1));
            kdebug("\".\n");

            frame->tmpSpace = (byte*)ptr;
        }
        setTermArg(code, frame, res);

        recret res;
    }
    else if (*code == 0x84) {
        kdebug("Найден оператор ConcatenateResTemplate.\n");
        code ++;

        TermArg arg1 = getTermArg(code, frame);
        TermArg arg2 = getTermArg(code, frame);
        if (arg1.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 1");
            kdebug("Ожидался тип 0x91, получен %X.\n", arg1.type);
            recret errorArg;
        }

        if (arg2.type != 0x91) {
            kdebug("ОШИБКА: Неправильный тип операнда 2");
            kdebug("Ожидался тип 0x92, получен %X.\n", arg2.type);
            recret errorArg;
        }

        val.type = 0x91;
        byte *ptr1 = (byte*)arg1.value;
        dword len1 = getACPIObjLen(ptr1);
        kdebug("Длина первого буфера: %d Б.\n", len1);
        if (*ptr1++ == 0x11)
            ptr1 += (*ptr1 >> 6) + 1;
        ptr1 += getIntegerTermBytes(ptr1);
        kdebug("Адрес данных первого буфера: %x Б.\n", ptr1);

        byte *ptr2 = (byte*)arg2.value;
        dword len2 = getACPIObjLen(ptr2);
        kdebug("Длина второго буфера: %d Б.\n", len2);
        if (*ptr2++ == 0x11)
            ptr2 += (*ptr2 >> 6) + 1;
        ptr2 += getIntegerTermBytes(ptr2);
        kdebug("Адрес данных второго буфера: %x Б.\n", ptr2);

        dword length = len1 + len2 - 2;
        kdebug("Длина итогового буфера: %d Б.\n", length);
        byte *ptr = frame->tmpSpace;
        *ptr++ = 0x22;
        ptr += encodeIntegerTerm(length, ptr);
        byte checksum = 0;
        memcpy(ptr1, ptr, len1 - 2);
        for (dword i = 0; i < len1 - 2; i++)
            checksum += *ptr++;
        memcpy(ptr2, ptr, len2 - 2);
        for (dword i = 0; i < len2 - 2; i++)
            checksum += *ptr++;
        *ptr++ = 0x79;
        *ptr++ = checksum;
        kdebug("Вычисленная контрольная сумма: %x.\n", checksum);

        val.value = (qword)frame->tmpSpace;
        frame->tmpSpace = ptr;
        setTermArg(code, frame, val);
        recret val;
    }
    kdebug("ОШИБКА: Не удалось вычислить объект\n");
    kdebug("Тип объекта неправилен или не поддерживается. (%x)", *code);
    return errorArg;
}

void setTermArg(byte*& code, FuncFrame *frame, TermArg newVal) {
    if (!(*code)) {
        code ++;
        return;
    }
    if (isLeadNameChar(*code)) {
        kdebug("Новое значение: %X.\n", newVal.value);
        word nameSegs = getName(code);
        code += nameSegs >> 8;
        byte *addr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
        if (!addr) {
            addr = getACPIObjAddr(varPath - 1, 1);
            if (!addr)
                return;
        }
        if (*addr == 0xFD) {
            kdebug("Запись производится в поле ");
            logVarPath();
            kdebugnewl();
            writeField(addr, newVal);
            return;
        }
        dword targetLen = getACPIObjLen((AMLName*)varPathBase, varPathLen);
        dword valueLen = 0;
        bool isInteger = newVal.type == 0x01 || newVal.type == 0x7F;
        if (isInteger) {
            valueLen = getIntegerTermBytes(newVal.value);
            kdebug("Длина нового значения: %d Б.\n", valueLen);
            if (valueLen > targetLen) {
                acpiData += encodeIntegerTerm(newVal.value, acpiData);
                remapACPIObj((AMLName*)varPathBase, varPathLen, acpiData);
            } else {
                encodeIntegerTerm(newVal.value, addr);
            }
        }
        else {
            if (*(byte*)(newVal.value) == 0x22) {
                valueLen = getIntegerTerm((byte*)(newVal.value + 1));
                valueLen += getIntegerTermBytes(valueLen);
                valueLen ++;
            }
            else if (*(byte*)newVal.value == 0x0D)
                valueLen = strlen((char*)(newVal.value + 1)) + 2;
            else
                valueLen = getPkgLength((byte*)(newVal.value + 1)) + 1;
            if (valueLen > targetLen) {
                memcpy((byte*)newVal.value, acpiData, valueLen);
                logVarPath();
                kdebugnewl();
                remapACPIObj((AMLName*)varPathBase, varPathLen, acpiData);
                acpiData += valueLen;
            } else {
                memcpy((byte*)newVal.value, addr, valueLen);
            }
        }
    }
    else if (*code == 0x88) {
        kdebug("Новое значение: %X.\n", newVal.value);
        code ++;
        bool needsCopying = isLeadNameChar(*code);
        TermArg src = getTermArg(code, frame);
        if (src.type != 0x91 && src.type != 0x92 && src.type != 0x8D) {
            kdebug("ОШИБКА: Неправильный тип операнда\n");
            kdebug("Ожидался тип 0x91, 0x92 или 0x8D, получен %X.\n", src.type);
            return;
        }
        TermArg index = getTermArg(code, frame);
        if (index.type != 0x01) {
            kdebug("ОШИБКА: Неправильный тип индекса\n");
            kdebug("Ожидался тип 0x01, получен %X.\n", index.type);
        }
        kdebug("Производится запись в объект ");
        logVarPath();
        kdebug(" [%D].\n", index.value);
        TermArg ref = getIndex(src, index.value, frame);
        if (ref.type == maxqword)
            return;
        byte *ptr = (byte*)ref.value;
        if (sameACPIDomain((dword)acpiData, (dword)ptr))
            needsCopying = true;
        if (src.type == 0x91 || src.type == 0x8D) {
            writeField(ptr, newVal);
            return;
        }
        ptr = (byte*)src.value;
        ptr ++;
        byte elements = *ptr++;
        ptr += index.value * 8;
        dword *entry = (dword*)ptr;
        byte *writeAddr = (byte*)entry[0];
        dword targetLen = entry[1];
        byte *valPtr = (byte*)src.value;
        dword newLen;
        if (newVal.type & 0x80)
            newLen = getACPIObjLen(valPtr);
        else
            newLen = getIntegerTermBytes(newVal.value);

        if (~newVal.type & 0x80) {
            kdebug("Записывается число %X.\n", newVal.value);
            kdebug("Адрес записи - %x.\n", entry[0]);
            kdebug("Длина текущего числа: %d Б.\n", targetLen);
            kdebug("Длина нового числа: %d Б.\n", newLen);
            if (newLen > targetLen) {
                if (needsCopying) {
                    kdebug("Требуется копирование.\n");
                    encodeIntegerTerm(newVal.value, acpiData);
                    entry[0] = (dword)acpiData;
                    entry[1] = newLen;
                    acpiData += newLen;
                } else {
                    kdebug("Копирование не требуется.\n");
                    entry[0] = (dword)frame->tmpSpace;
                    frame->tmpSpace += encodeIntegerTerm(newVal.value, frame->tmpSpace);
                    entry[1] = newLen;
                }
            } else {
                encodeIntegerTerm(newVal.value, writeAddr);
            }
            return;
        }
        
        kdebug("Адрес записи: %x.\n", valPtr);
        kdebug("Длина записи: %d Б.\n", newLen);
        kdebug("Текущая длина элемента: %d Б.\n", targetLen);

        if (newLen > targetLen) {
            if (needsCopying) {
                kdebug("Требуется копирование.\n");
                memcpy(valPtr, acpiData, newLen);
                entry[0] = (dword)acpiData;
                entry[1] = newLen;
                acpiData += newLen;
            } else {
                kdebug("Копирование не требуется.\n");
                entry[0] = (dword)valPtr;
                entry[1] = newLen;
            }
        } else {
            memcpy(valPtr, writeAddr, newLen);
        }
        code ++;
    }
    else if (*code >= 0x60 && *code <= 0x66) {
        kdebug("Новое значение: %X.\n", newVal.value);
        byte localNo = *code & 7;
        code++;
        kdebug("Запись производится в Local%d.\n", localNo);
        if (newVal.type == 0x01 || newVal.type == 0x7F) {
            kdebug("Устанавливается значение IntegerTerm.\n");
            frame->locals[localNo] = newVal;
            kdebug("Значение = %D (%X).\n", frame->locals[localNo].value, frame->locals[localNo].value);
            return;
        }
        if (!frame->locals[localNo].value) {
            kdebug("Устанавливается новое значение для Local%d.\n", localNo);
            frame->locals[localNo] = newVal;
            kdebug("Значение = %D (%X).\n", frame->locals[localNo].value, frame->locals[localNo].value);
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
            frame->locals[localNo].type = 0x8D;
            ptr--;
        } else {
            ptr++;
            len = getPkgLength(ptr) + 1;
            frame->locals[localNo].type = *(--ptr) | 0x80;
        }
        dword targetLen = getACPIObjLen((byte*)(frame->locals[localNo].value));
        if (len > targetLen) {
            memcpy(ptr, frame->tmpSpace, len);
            frame->locals[localNo].value = (dword)frame->tmpSpace;
            frame->tmpSpace += len;
        }
        else {
            byte* targetPtr = (byte*)frame->locals[localNo].value;
            memcpy(ptr, targetPtr, len);
        }
    }
    else if (*(word*)code == 0x315B) {
        code += 2;
        char* ptr = (char*)castTermArg(newVal, 0x8D, frame).value;
        ptr ++;
        kdebug("С.О.М.: \"");
        kdebug((const char*)ptr);
        kdebug("\"\n");
    }
    else if (*code == 0x88) {
        TermArg ref = getTermArg(code, frame);
        byte *addr = (byte*)ref.value;
        if (*addr == 0xFD) {
            writeField(addr, newVal);
        }
    }
}

TermArg runMethod(byte* code, dword length, byte syncLevel, FuncFrame *frame) {
    dword addr = (dword)code;
    kdebug("Начало кода: %x.\n", addr);
    for (dword i = 0; i < length; i++) {
        kdebug("Сдвиг = %x Байт = %x\n", i, code[i]);
        dword tmp = (dword)code + i;
        dword diff;
        if (code[i] == 0xA4) {
            kdebug("Найден оператор Return.\n");
            code += i + 1;
            TermArg retVal = getTermArg(code, frame);
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
            kdebug("Возвращается значение %D (%X).\n\n", retVal.value, retVal.value);
            *frame->tmpSpace = 0xAA;
            return retVal;
        }
        else if (code[i] == 0xA0) {
            kdebug("Найден оператор If.\n");
            i ++;
            dword ifLen = getPkgLength(code + i);
            kdebug("Длина If-блока: %d Б.\n", ifLen);
            i += getPkgBytes(ifLen);
            tmp += 1 + getPkgBytes(ifLen);
            code = (byte*)tmp;
            dword _addr = (dword)code;
            TermArg predicate = getTermArg(code, frame);
            dword predicateLen = (dword)code - _addr;
            ifLen -= getPkgBytes(ifLen) + predicateLen;
            TermArg res;
            kdebug("Условие выполнено? ");
            if (predicate.value) {
                kdebug("Да\n");
                res = runMethod(code, ifLen, syncLevel, frame);
                if (res.type == loopContinueArg.type)
                    return loopContinueArg;
                if (res.type != continueArg.type) {
                    kdebug("Возвращается значение {%X, %X}.\n", res);
                    return res;
                }
                code += ifLen;
                if (*code == 0xA1) {
                    code ++;
                    dword elseLen = getPkgLength(code);
                    kdebug("Пропускается блок Else длиной %d Б.\n", elseLen);
                    code += elseLen;
                }
            } else {
                kdebug("Нет\n");
                code += ifLen;
                if (*code == 0xA1) {
                    kdebug("Исполняется блок Else.\n");
                    dword elseLen = getPkgLength(code);
                    elseLen -= getPkgBytes(elseLen);
                    kdebug("Длина блока - %d Б.\n", elseLen);
                    code += getPkgBytes(*code);
                    res = runMethod(code, elseLen, syncLevel, frame);
                    if (res.type == loopContinueArg.type)
                        return loopContinueArg;
                    if (res.type != continueArg.type) {
                        kdebug("Возвращается значение {%X, %X}.\n", res);
                        return res;
                    }
                }
            }
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0xA2) {
            kdebug("Найден оператор While.\n");
            i ++;
            dword whileLen = getPkgLength(code + i);
            kdebug("Длина блока: %d Б.\n", whileLen);
            i += getPkgBytes(whileLen);
            byte *predicatePtr = code + i;
            byte *_predicatePtr = predicatePtr;
            TermArg predicate = getTermArg(predicatePtr, frame);
            dword predicateLen = predicatePtr - _predicatePtr;
            predicatePtr = _predicatePtr;
            i += predicateLen;
            byte *whilePtr = code + i;
            whileLen -= getPkgBytes(whileLen) + predicateLen;
            TermArg res;
            kdebug("%x %d %x\n", code + i, i, code[i]);
            while (predicate.value) {
                kdebug("Условие цикла выполнено.\n");
                res = runMethod(whilePtr, whileLen, syncLevel, frame);
                if (res.type != continueArg.type && res.type != loopContinueArg.type) {
                    if (res.type == breakArg.type)
                        break;
                    kdebug("Возвращается значение {%X, %X}.\n", res);
                    return res;
                }
                predicatePtr = _predicatePtr;
                kdebug("Перерасчёт условия.\n");
                predicate = getTermArg(predicatePtr, frame);
            }
            if (!predicate.value)
                kdebug("Условие цикла не было выполнено.\n");
            i += whileLen;
        }
        else if (code[i] == 0x9F) {
            kdebug("Найден оператор Continue.\n");
            return loopContinueArg;
        }
        else if (code[i] == 0xA5) {
            kdebug("Найден оператор Break.\n");
            return breakArg;
        }
        else if (code[i] == 0xA3) {
            kdebug("Найден оператор Noop.\n");
            kdebug("// Отдыхаем\n");
            i ++;
        }
        else if (code[i] == 0xCC) {
            kdebug("Найден оператор BreakPoint.\n");
            magicBreakpoint();
            i ++;
        }
        else if (code[i] == 0x86) {
            kdebug("Найден оператор Notify.\n");
            i ++;
            word nameSegs = getName(code + i);
            i += nameSegs >> 8;
            tmp = (dword)code + i;
            code += i;
            TermArg eventNo = getTermArg(code, frame);
            if (eventNo.type != 0x01) {
                kdebug("ОШИБКА: Неправильный тип операнда 1\n");
                kdebug("Ожидался тип 0x01, получен %X.\n", eventNo.type);
                return errorArg;
            }
            processEvent(eventNo.value);
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x08) {
            kdebug("Найден оператор Name.\n");
            i++;
            i += nameACPIObj(code + i);
            i++;
        }
        else if (code[i] == 0x72 || code[i] == 0x74 || code[i] == 0x77 || code[i] == 0x78 || code[i] == 0x85 || code[i] == 0x79 || code[i] == 0x7A || code[i] == 0x7B || code[i] == 0x7C || code[i] == 0x7D || code[i] == 0x7E || code[i] == 0x7F) {
            byte opType = code[i];
            code += i;
            TermArg val = getTermArg(code, frame);
            if (val.value == maxqword) {
                return val;
            }
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x75 || code[i] == 0x76 || code[i] == 0x80) {
            code += i;
            TermArg arg = getTermArg(code, frame);
            if (arg.type == errorArg.type)
                return arg;
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x70) {
            kdebug("Найден оператор Store.\n");
            code += i + 1;
            TermArg arg = getTermArg(code, frame);
            if (arg.type == maxqword)
                return arg;
            if (*code >= 0x60 && *code <= 0x66) {
                byte localNo = *code & 7;
                arg = castTermArg(arg, frame->locals[localNo].type, frame);
                if (arg.type == errorArg.type)
                    return arg;
            }
            setTermArg(code, frame, arg);
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (code[i] == 0x97 || code[i] == 0x98 || code[i] == 0x99 || code[i] == 0x9C) {
            code += i;
            TermArg arg = getTermArg(code, frame);
            if (arg.type == errorArg.type)
                return arg;
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (*(word*)(code + i) == 0x225B) {
            kdebug("Найден оператор Sleep.\n");
            i += 2;
            tmp += 2;
            code += i;
            TermArg arg = getTermArg(code, frame);
            if (arg.type != 0x01) {
                kdebug("ОШИБКА: Неправильный тип операнда 1\n");
                kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
                return arg;
            }
            kdebug("Ожидание %d мс.\n", arg.value);
            sleepM(arg.value);
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (*(word*)(code + i) == 0x215B) {
            kdebug("Найден оператор Stall.\n");
            i += 2;
            tmp += 2;
            code += i;
            TermArg arg = getTermArg(code, frame);
            if (arg.type != 0x01) {
                kdebug("ОШИБКА: Неправильный тип операнда 1\n");
                kdebug("Ожидался тип 0x01, получен %X.\n", arg.type);
                return arg;
            }
            kdebug("Ожидание %d мкс.\n", arg.value);
            sleepU(arg.value);
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (*(word*)(code + i) == 0x245B) {
            kdebug("Найден оператор Signal.\n");
            i += 2;
            tmp += 2;
            word nameSegs = getName(code + i);
            i += nameSegs >> 8;
            tmp += nameSegs >> 8;
            code += i;
            kdebug("Абсолютный путь к событию: ");
            logVarPath();
            kdebugnewl();
            byte *ptr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
            if (*ptr != 0x02) {
                kdebug("ОШИБКА: Объект не является событием");
                return {maxqword, 84};
            }
            ptr ++;
            if (*ptr < 0xFF)
                *ptr = *ptr + 1;
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (*(word*)(code + i) == 0x265B) {
            kdebug("Найден оператор Reset.\n");
            i += 2;
            tmp += 2;
            word nameSegs = getName(code + i);
            i += nameSegs >> 8;
            tmp += nameSegs >> 8;
            code += i;
            kdebug("Абсолютный путь к событию: ");
            logVarPath();
            kdebugnewl();
            byte *ptr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
            if (*ptr != 0x02) {
                kdebug("ОШИБКА: Объект не является событием");
                return {maxqword, 84};
            }
            ptr ++;
            *ptr = 0;
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        else if (*(word*)(code + i) == 0x325B) {
            kdebug("ОШИБКА: Фатальный сбой\n");
            i += 2;
            byte fataltype = code[i];
            i ++;
            dword fatalcode = *(dword*)(code + i);
            i += 4;
            code += i;
            TermArg fatalarg = getTermArg(code, frame);
            kdebug("\tТип: %d\n", fataltype);
            kdebug("\tКод: %x\n", fatalcode);
            kdebug("\tАргумент: %X\n", fatalarg.value);
            return {maxqword, fatalarg.value};
        }
        else if (code[i] == 0x8D || code[i] == 0x8C || code[i] == 0x8B || code[i] == 0x8A || code[i] == 0x8F || *(word*)(code+i) == 0x135B) {
            dword offset = 0;
            dword size = 0;
            byte *ptr = nullptr;
            kdebug("Найден оператор ");
            if (code[i] == 0x8D) {
                kdebug("CreateBitField.\n");
                size = 1;
                i ++;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i);
                i += getIntegerTermBytes(code + i);
            }
            else if (code[i] == 0x8C) {
                kdebug("CreateByteField.\n");
                size = 8;
                i ++;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i) * 8;
                i += getIntegerTermBytes(code + i);
            }
            else if (code[i] == 0x8B) {
                kdebug("CreateWordField.\n");
                size = 16;
                i ++;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i) * 8;
                i += getIntegerTermBytes(code + i);
            }
            else if (code[i] == 0x8A) {
                kdebug("CreateDWordField.\n");
                size = 32;
                i ++;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i) * 8;
                i += getIntegerTermBytes(code + i);
            }
            else if (code[i] == 0x8F) {
                kdebug("CreateQWordField.\n");
                size = 64;
                i ++;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i) * 8;
                i += getIntegerTermBytes(code + i);
            }
            else if (*(word*)(code+i) == 0x135B) {
                kdebug("CreateField.\n");
                i += 2;
                word nameSegs = getName(code + i);
                i += nameSegs >> 8;
                offset = getIntegerTerm(code + i);
                i += getIntegerTermBytes(code + i);
                size = getIntegerTerm(code + i);
                i += getIntegerTermBytes(code + i);
            }
            kdebug("Абсолютный путь к целевому буферу: ");
            logVarPath();
            kdebugnewl();
            ptr = getACPIObjAddr((AMLName*)varPathBase, varPathLen);
            if (!ptr) {
                kdebug("ОШИБКА: Буфер не существует\n");
                return {maxqword, 0};
            }
            kdebug("Размер поля: %d бит.\n", size);
            kdebug("Сдвиг поля: %d бит.\n", offset);
            word nameSegs = parseName(code + i);
            i += nameSegs >> 8;
            kdebug("Абсолютный путь к полю: ");
            logVarPath();
            kdebugnewl();
            if (getACPIObjAddr((AMLName*)varPathBase, varPathLen)) {
                kdebug("ВНИМАНИЕ: Поле уже существует\n");
                i --;
                continue;
            }
            createBufferField(ptr, offset, size, acpiFields);
        }
        else {
            code += i;
            TermArg arg = getTermArg(code, frame);
            if (arg.type == errorArg.type)
                return arg;
            diff = (dword)code - tmp;
            code = (byte*)addr;
            i += diff;
        }
        i --;
    }
    return continueArg;
}