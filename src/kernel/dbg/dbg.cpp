#include "dbg.hpp"
#include "../graphics/glyphs.hpp"
#include "../file/file.hpp"
#include "../io/com.hpp"
#include "../memmgr/memmgr.hpp"
#include "../str/str.hpp"
#include "../util/util.hpp"

const dword kernelSymbolsSize = 0x10000;
const dword kernelMapSize = 0x2000;

char *kernelSymbols = nullptr;
char *kernelSymbolsBase = nullptr;

ptrint *kernelMap = nullptr;
ptrint *kernelMapBase = nullptr;

void traceStack() {
    StackFrame *frame;
    kprint("Стек вызовов:\n");
    __asm__ ("movl %%ebp, %d0" : "=r"(frame) :);
    frame = frame->ebp;
    bool first = true;
    while (frame) {
        ptrint addr = (ptrint)frame->eip;
        char *funcName = findFunctionName(addr);

        if (!first)
            kprint(",\n");
        first = false;
        if (funcName) {
            kprint("  ");
            kprint(funcName);
            kprint(" (%x)", addr);
        } else {
            kprint("  %x", addr);
        }
        frame = frame->ebp;
    }
    kprint("\n");
}

char *findFunctionName(ptrint addr) {
    ptrint lowAddr = *(ptrint*)kernelSymbolsBase;
    char *funcName = nullptr;
    dword mapSize = (dword)(kernelMap - kernelMapSize) / (sizeof(byte*));
    for (dword i = 0; i < mapSize - 2; i++) {
        ptrint lowAddr = kernelMapBase[2*i];
        ptrint highAddr = kernelMapBase[2*i+2];
        if ((lowAddr <= addr) && (addr < highAddr)) {
            funcName = (char*)kernelMapBase[2*i+1];
            break;
        }
    }
    return funcName;
}

char *parseSymbol(char *symbol) {
    char *out = (char*)kernelSymbols;
    if (!strstartswith(symbol, "_Z")) {
        strcpy(symbol, out);
        kernelSymbols += strlen(out) + 1;
        return out;
    }

    char *ptr = out;
    symbol += 2;
    if (*symbol == 'N') {
        symbol ++;
        while (*symbol != 'E') {
            word nameLen = strdectoint(symbol);
            symbol += digitsLength(nameLen);
            memcpy((byte*)symbol, (byte*)ptr, nameLen);
            symbol += nameLen;
            ptr += nameLen;
            *ptr++ = '.';
            if (strstartswith(symbol, "nt")) {
                ptr += strcpy("!", ptr) + 1;
                symbol += 2;
            }
            if (strstartswith(symbol, "ne")) {
                ptr += strcpy("!=", ptr) + 1;
                symbol += 2;
            }
            if (strstartswith(symbol, "eq")) {
                ptr += strcpy("==", ptr) + 1;
                symbol += 2;
            }
            if (*symbol == 'C') {
                ptr += strcpy("new", ptr) + 1;
                symbol++;
                symbol += digitsLength(strdectoint(symbol));
            }
            if (*symbol == 'D') {
                ptr += strcpy("<деструктор>", ptr) + 1;
                symbol++;
                symbol += digitsLength(strdectoint(symbol));
            }
        }
        ptr --;
        symbol++;
    } else {
        word nameLen = strdectoint(symbol);
        symbol += digitsLength(nameLen);
        memcpy((byte*)symbol, (byte*)ptr, nameLen);
        symbol += nameLen;
        ptr += nameLen;
    }

    *ptr++ = '(';

    byte ptrDepth = 0;
    byte refDepth = 0;
    while (*symbol) {
        if (*symbol == 'v' && !(ptrDepth || refDepth)) {
            ptr += 2;
            break;
        }

        if (*symbol == 'P') {
            ptrDepth ++;
            symbol++;
            continue;
        }

        if (*symbol == 'R') {
            refDepth = 1;
            symbol ++;
            continue;
        }

        if (*symbol == 'O') {
            refDepth = 2;
            symbol ++;
            continue;
        }

        if (*symbol == 'K') {
            ptr += strcpy("const ", ptr);
            symbol ++;
            continue;
        }

        if (strstartswith(symbol, "S_")) {
            ptr += strcpy("char*", ptr);
            symbol += 2;
            symbol--;
        }
        else if (strstartswith(symbol, "S0_")) {
            ptr += strcpy("const char*", ptr);
            symbol += 3;
            symbol--;
        }
        else if (*symbol >= '0' && *symbol <= '9') {
            word typenameLen = strdectoint(symbol);
            symbol += digitsLength(typenameLen);
            memcpy((byte*)symbol, (byte*)ptr, typenameLen);
            ptr += typenameLen;
            symbol += typenameLen;
            symbol--;
        }
        else if (*symbol == 'b')
            ptr += strcpy("bool", ptr);
        else if (*symbol == 'c')
            ptr += strcpy("char", ptr);
        else if (*symbol == 'h')
            ptr += strcpy("byte", ptr);
        else if (*symbol == 's')
            ptr += strcpy("short", ptr);
        else if (*symbol == 't')
            ptr += strcpy("word", ptr);
        else if (*symbol == 'i')
            ptr += strcpy("int", ptr);
        else if (*symbol == 'j')
            ptr += strcpy("dword", ptr);
        else if (*symbol == 'l')
            ptr += strcpy("long", ptr);
        else if (*symbol == 'm')
            ptr += strcpy("qword", ptr);
        else if (*symbol == 'x')
            ptr += strcpy("long long", ptr);
        else if (*symbol == 'y')
            ptr += strcpy("qword", ptr);
        else if (*symbol == 'f')
            ptr += strcpy("float", ptr);
        else if (*symbol == 'd')
            ptr += strcpy("double", ptr);
        else if (*symbol == 'e')
            ptr += strcpy("long double", ptr);
        else if (*symbol == 'z')
            ptr += strcpy("va_list", ptr);
        else if (*symbol == 'v')
            ptr += strcpy("void", ptr);
        else
            *ptr++ = *symbol;

        symbol++;

        for (byte i = 0; i < ptrDepth; i++) {
            *ptr++ = '*';
        }
        for (byte i = 0; i < refDepth; i++) {
            *ptr++ = '&';
        }
        ptrDepth = 0;
        refDepth = 0;
        ptr += strcpy(", ", ptr);
        continue;
    }

    ptr -= 2;
    *ptr++ = ')';
    *ptr++ = 0;
    kernelSymbols = ptr;
    return out;
}

void initKernelMap(byte drive) {
    kdebug("Начата инициализация карты ядра.\n");
    FileHandle *map = openFile("/kernel.map", drive, FILE_MODE_READ);
    if (!map)
        return;

    kdebug("Файл готов. Начинается анализ.\n");
    char *fileBuf = (char*)kmalloc(map->file->size);
    char *_fileBuf = fileBuf;
    map->file->read((byte*)fileBuf);

    kernelSymbols = (char*)kmalloc(kernelSymbolsSize);
    kernelSymbolsBase = kernelSymbols;
    kernelMap = (ptrint*)kmalloc(kernelMapSize);
    kernelMapBase = kernelMap;

    strskiplines(fileBuf, 12);
    kdebugnewl();
    while (strstartswith(fileBuf, "LOAD")) {
        strskiplines(fileBuf, 1);
    }
    strskiplines(fileBuf, 11);

    dword i = 1;
    while (*fileBuf) {
        if (*fileBuf != ' ') {
            strskiplines(fileBuf, 1);
            continue;
        }

        fileBuf++;
        if (*fileBuf != ' ') {
            strskiplines(fileBuf, 1);
            continue;
        }

        fileBuf += 15;
        fileBuf += 24;
        if (*fileBuf != ' ') {
            strskiplines(fileBuf, 1);
            continue;
        }

        fileBuf += 2;

        ptrint addr = strhextoint(fileBuf - 26);
        char *ptr = fileBuf;
        while (*ptr && *ptr != 0x0A)
            ptr++;
        *ptr = 0;
        char *name = parseSymbol(fileBuf);

        *kernelMap++ = (ptrint)addr;
        *kernelMap++ = (ptrint)name;

        kdebug("Описание объекта %d:\n", i);
        kdebug("\tАдрес: %x\n", addr);
        kdebug("\tНазвание: \"");
        kdebug(name);
        kdebug("\"\n");
        i++;
        *ptr = 0x0A;
        strskiplines(fileBuf, 1);
    }

    kfree(_fileBuf);
}