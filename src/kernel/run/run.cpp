#include "run.hpp"
#include "process.hpp"
#include "../memmgr/memmgr.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"
#include "../util/util.hpp"
#include "../str/str.hpp"
#include "../dbg/dbg.hpp"
#include "../cpu/gdt.hpp"

dword runExecutable(FAT32_File executable, char *args, char *path) {
    kdebug("Начат запуск программы ");
    kdebug(executable.name);
    kdebug(".\n");
    byte drive = executable.drive;
    dword clusSize = (executable.size + clustersize(drive) - 1) / clustersize(drive);
    byte *file = kmalloc(clusSize * clustersize(drive));
    executable.read(file);
    
    ELF_Header32 hdr = *(ELF_Header32*)file;
    kdebug("Обработка заголовка.\n");
    kdebug("\tМагическое число: %x\n", hdr.elfMagic);
    kdebug("\tРазрядность файла: %d-бит\n", hdr.fileClass == 1 ? 32 : 64);
    kdebug("\tТип кодирования данных: ");
    kdebug(hdr.encoding == 1 ? "little-endian\n" : "big-endian\n");
    kdebug("\tВерсия ELF-файла: %d\n", hdr.elfVersion);
    kdebug("\tИдентификатор ABI: %d\n", hdr.ABIid);
    kdebug("\tТип файла: %d\n", hdr.type);
    kdebug("\tТип компьютера: %d\n", hdr.machine);
    kdebug("\tВерсия объектного файла: %d\n", hdr.version);
    kdebug("\tТочка входа в программу: %x\n", hdr.entryPoint);
    kdebug("\tСдвиг программной таблицы: %d Б\n", hdr.programHeaderOff);
    kdebug("\tСдвиг таблицы секций: %d Б\n", hdr.sectionHeaderOff);
    kdebug("\tФлаги: %b\n", hdr.flags);
    kdebug("\tРазмер ELF-заголовка: %d Б\n", hdr.elfHeaderSize);
    kdebug("\tРазмер метки программной таблицы: %d Б\n", hdr.phEntrySize);
    kdebug("\tКоличество меток программной таблицы: %d\n", hdr.phEntryCount);
    kdebug("\tРазмер метки таблицы секций: %d Б\n", hdr.shEntrySize);
    kdebug("\tКоличество меток таблицы секций: %d\n", hdr.shEntryCount);

    if (!hdr.sectionHeaderOff) {
        kdebug("ОШИБКА: Нет таблицы секций\n");
        kdebug("Исполнение файла отменено.");
        kfree(file);
        return RUN_ERR_NO_SECTION_TABLE;
    }

    if (hdr.encoding == 0) {
        kdebug("ОШИБКА: Неверный тип представления данных\n");
        kdebug("Исполнение файла отменено.\n");
        kfree(file);
        return RUN_ERR_INVALID_ENCODING;
    }

    if (hdr.encoding == 2) {
        kdebug("ОШИБКА: Big-endian-файлы не поддерживаются\n");
        kdebug("Исполнение файла отменено.\n");
        kfree(file);
        return RUN_ERR_BIG_ENDIAN;
    }

    Process proc = registerProcess();
    word pid = proc.pid;
    dword code = 0;
    switch (hdr.type) {
        case 1:
            kdebug("Тип файла - перемещаемый.\n");
            code = runRelocatableELF(file, pid, args);
            break;
        case 2:
            kdebug("Тип файла - загружаемый.\n");
            if (!hdr.programHeaderOff) {
                kdebug("ОШИБКА: Нет таблицы программы\n");
                kdebug("Исполнение файла отменено.\n");
                kfree(file);
                return RUN_ERR_NO_PROGRAM_TABLE;
            }
            code = runLoadableELF(file, pid, args);
            break;
        case 3:
            kdebug("Тип файла - динамично компонуемый.\n");
            code = runDynamicELF(file, pid, args);
            break;
        default:
            kdebug("ОШИБКА: Нераспознанный тип файла %d\n", hdr.type);
            kdebug("Исполнение файла отменено.\n");
            kfree(file);
            return RUN_ERR_BAD_FILE_TYPE;
    }

    kfree(file);
    return code;
}

dword runRelocatableELF(byte *file, word pid, char *args) {
    ELF_Header32 hdr = *(ELF_Header32*)file;
    ELF_SectionHeader32 *sections = (ELF_SectionHeader32*)(file + hdr.sectionHeaderOff);
    dword shSize = hdr.shEntryCount;
    kdebug("Анализ таблицы секций.\n");

    char *shStrtabPtr = nullptr;
    char *strtabPtr   = nullptr;
    ELF_SymTabEntry32 *symtabPtr = nullptr;
    dword symbolsCount = 0;
    for (dword i = 0; i < shSize; i++) {
        if (sections[i].type == ELF_SECTION_STRTAB && i == hdr.strTableIndex) {
            shStrtabPtr = (char*)file + sections[i].offset;
            kdebug("Таблица строк секций находится в секции № %d.\n", i+1);
            continue;
        }
        if (sections[i].type == ELF_SECTION_STRTAB && !strtabPtr) {
            strtabPtr = (char*)file + sections[i].offset;
            kdebug("Таблица строк файла находится в секции № %d.\n", i+1);
            continue;
        }
        if (sections[i].type == ELF_SECTION_SYMTAB && !symtabPtr) {
            symtabPtr = (ELF_SymTabEntry32*)(file + sections[i].offset);
            symbolsCount = sections[i].size / sizeof(ELF_SymTabEntry32);
            kdebug("Таблица символов файла находится в секции № %d.\n", i+1);
            continue;
        }
    }
    if (!shStrtabPtr) {
        kdebug("ВНИМАНИЕ: Не найдена таблица строк секций\n");
    }
    if (!strtabPtr) {
        kdebug("ВНИМАНИЕ: Не найдена таблица строк файла\n");
    }
    if (!symtabPtr) {
        kdebug("ВНИМАНИЕ: Не найдена таблица символов файла\n");
    } else {
        kdebug("Количество символов: %d.\n", symbolsCount);
    }

    for (dword i = 0; i < shSize; i++) {
        kdebug("Секция %d:\n", i+1);
        ELF_SectionHeader32 section = sections[i];
        kdebug("\tИмя: ");
        if (shStrtabPtr) {
            kdebug("\"");
            kdebug(shStrtabPtr + section.name);
            kdebug("\"\n");
        } else {
            kdebug("<%d>\n", section.name);
        }
        kdebug("\tТип: %d\n", section.type);
        kdebug("\tФлаги: %b\n", section.flags);
        kdebug("\tАдрес: %x\n", section.addr);
        kdebug("\tСдвиг: %d Б\n", section.offset);
        kdebug("\tРазмер: %d Б\n", section.size);
        kdebug("\tИнформация: %b\n", section.info);
        kdebug("\tРавнение адреса: %x\n", section.addrAlign);
    }

    kdebug("Анализ таблицы символов.\n");
    for (dword i = 0; i < symbolsCount; i++) {
        ELF_SymTabEntry32 symbol = symtabPtr[i];
        kdebug("Символ %d:\n", i+1);
        kdebug("\tИмя: ");
        if (strtabPtr) {
            kdebug("\"");
            kdebug(strtabPtr + symbol.name);
            kdebug("\"\n");
        } else {
            kdebug("<%d>\n", symbol.name);
        }
        kdebug("\tЗначение: %x\n", symbol.value);
        kdebug("\tРазмер: %d Б\n", symbol.size);
        kdebug("\tИнформация: %b\n", symbol.info);
        kdebug("\tВидимость: %b\n", symbol.other);
        kdebug("\tСекция: %d\n", symbol.shIndex);
    }

    byte **sectionPtrs = (byte**)kmalloc(sizeof(byte**) * shSize);
    memset(sectionPtrs, sizeof(byte**) * shSize, 0);

    for (dword i = 0; i < shSize; i++) {
        ELF_SectionHeader32 section = sections[i];
        if (section.flags & 0b100)
            kdebug("Секция %d содержит исполняемый код.\n", i+1);
        if (section.flags & 0b10) {
            kdebug("Секция %d загружаема в память.\n", i+1);
            byte *ptr = kmalloc(section.size, pid);
            memcpy(file + section.offset, ptr, section.size);
            sectionPtrs[i] = ptr;
        }
    }

    for (dword i = 0; i < shSize; i++) {
        if (sections[i].type != ELF_SECTION_RELOCATE)
            continue;

        kdebug("Секция %d содержит данные о перемещении.\n", i+1);
        dword relocStructs = sections[i].size / sizeof(ELF_RelocateData32);
        dword relocSection = maxdword;
        for (dword j = 0; j < shSize; j++) {
            if (j == i) continue;

            char *curName = shStrtabPtr + sections[i].name + 4;
            char *name = shStrtabPtr + sections[j].name;

            if (strcmp(curName, name)) {
                kdebug("Эта секция перемещает данные секции %d.\n", j+1);
                relocSection = j;
            }
        }

        if (relocSection == maxdword) {
            kdebug("ОШИБКА: Секция перемещения указывает на несуществующую секцию ");
            kdebug(shStrtabPtr + sections[i].name + 4);
            kdebugnewl();
            kdebug("Исполнение файла отменено.\n");
            kfree(file);
            for (dword i = 0; i < shSize; i++) {
                if (sectionPtrs[i])
                    kfree(sectionPtrs[i]);
            }
            kfree(sectionPtrs);
            return RUN_ERR_BAD_RELOC_SECTION;
        }
        if (sectionPtrs[relocSection] == nullptr) {
            kdebug("ОШИБКА: Секция %d не существует в памяти, перемещение невозможно\n");
            kdebug("Исполнение файла отменено.\n");
            kfree(file);
            for (dword i = 0; i < shSize; i++) {
                if (sectionPtrs[i])
                    kfree(sectionPtrs[i]);
            }
            kfree(sectionPtrs);
            return RUN_ERR_BAD_RELOC_SECTION;
        }


        for (dword r = 0; r < relocStructs; r++) {
            ELF_RelocateData32 *reloc = (ELF_RelocateData32*)(file + sections[i].offset) + r;
            kdebug("Метка %d:\n", r+1);
            kdebug("\tСдвиг: %x\n", reloc->offset);
            kdebug("\tТип: %d\n", reloc->type);
            kdebug("\tСимвол: %d\n", reloc->symbol);
            byte size = 0;

            ELF_SymTabEntry32 symbol = symtabPtr[reloc->symbol];

            dword s = symbol.value;
            dword z = symbol.size;
            

            dword value = 0;
            switch (reloc->type) {
                case 0:
                    value = 0;
                    size = 0;
                    break;
                case 1:
                    value = s;
                    size = 32;
                    break;
                case 2:
                    value = s;
                    size = 32;
                    break;
                case 6:
                    value = s;
                    size = 32;
                    break;
                case 7:
                    value = s;
                    size = 32;
                    break;
                case 20:
                    value = s;
                    size = 16;
                    break;
                case 22:
                    value = s;
                    size = 8;
                    break;
                case 38:
                    value = s;
                    size = 32;
                    break;
                default:
                    kdebug("ОШИБКА: Неподдерживаемый тип перемещения %d\n", reloc->type);
                    kdebug("Исполнение файла отменено.\n");
                    kfree(file);
                    for (dword i = 0; i < shSize; i++) {
                        if (sectionPtrs[i])
                            kfree(sectionPtrs[i]);
                    }
                    kfree(sectionPtrs);
                    return RUN_ERR_BAD_RELOC_TYPE;
            }

            value += (dword)sectionPtrs[symbol.shIndex];
            byte *writePtr = sectionPtrs[relocSection] + reloc->offset;
            kdebug("Вычисленное значение перемещения: %x.\n", value);
            for (byte i = 0; i < size/8; i++) {
                *writePtr++ += value & 0xFF;
                value >>= 8;
            }
        }
    }

    return 0;
}

dword runLoadableELF(byte *file, word pid, char *args) {
    ELF_Header32 hdr = *(ELF_Header32*)file;
    ELF_ProgramHeader32 *ph = (ELF_ProgramHeader32*)(file + hdr.programHeaderOff);

    kdebug("Процессу присвоен PID %d.\n", pid);

    byte **frags = (byte**)kmalloc(sizeof(byte**) * hdr.phEntryCount);
    for (dword i = 0; i < hdr.phEntryCount; i++) {
        if (ph[i].type == 1) {
            dword vaddr = ph[i].vAddr;
            dword size = ph[i].memSize;
            dword offset = ph[i].offset;

            kdebug("Фрагмент № %d загружаем в память.\n", i+1);
            kdebug("Адрес погрузки: %x.\n", vaddr);
            kdebug("Размер погрузки: %d Б.\n", size);

            byte *ptr = kmallocPhys(vaddr, size, pid);
            for (dword i = 0; i < (size + PAGE_SIZE - 1) / PAGE_SIZE; i++) {
                setPagePermsLevel(vaddr + i * PAGE_SIZE, 3);
            }
            memset(ptr, size, 0);
            memcpy(file + offset, ptr, ph[i].fileSize);
            frags[i] = ptr;
        }
    }

    EntryPoint entryPoint = (EntryPoint)hdr.entryPoint;
    kdebug("Адрес точки входа: %x.\n", entryPoint);

    if (!entryPoint) {
        kdebug("ОШИБКА: Нет точки входа\n");
        kdebug("Исполнение файла отменено.\n");
        for (dword i = 0; i < hdr.phEntryCount; i++) {
            if (frags[i]) kfree(frags[i], pid);
        }
        kfree(frags, pid);
        return RUN_ERR_BAD_ENTRY_POINT;
    }

    byte *stack = kmalloc(RUN_STACK_SIZE + sizeof(byte*), pid);
    byte *_stack = stack;
    for (word i = 0; i <= (RUN_STACK_SIZE + sizeof(byte*) + PAGE_SIZE - 1) / PAGE_SIZE; i++) {
        setPagePermsLevel((dword)stack + i * PAGE_SIZE, 3);
    }
    stack += RUN_STACK_SIZE;

    byte func[] = { 0x89, 0xC6, 0x31, 0xC0, 0xCD, 0xC0 };   // mov esi, eax; xor eax, eax; int 0xC0
    byte *exitFunc = kmalloc(sizeof(func), pid);
    setPagePermsLevel((dword)exitFunc, 3);
    memcpy(func, exitFunc, sizeof(func));

    char **argv = strsplit(args, " ");
    dword argc = 0;
    dword i = 0;
    while (argv[i] != nullptr) {
        char *element = argv[i];
        if (element[0] == '"') {
            char **write = argv + i;
            dword len = strlen(element);
            (*write)++;
            len--;
            char *arg = *write;
            i++;
            while (element && element[len-1] != '"') {
                element = argv[i];
                len = strlen(element);
                argv[i] = (char*)1;
                i++;
                strconcat(arg, " ", arg);
                strconcat(arg, element, arg);
            }
            if (!element) break;
            len = strlen(arg);
            arg[len-1] = 0;
            *write = arg;
            i--;
        }
        i++;
        argc++;
    }

    kdebug("Программе передаётся %d аргументов: \"", argc);
    kdebug(*argv);
    kdebug("\"");
    i = 1;
    while (argv[i] != nullptr) {
        if (argv[i] == (char*)1) continue;
        kdebug(", \"");
        kdebug(argv[i]);
        kdebug("\"");
        i++;
    }
    kdebug(".\n");

    dword exitCode;
    disableInts();
    __asm__ (
        "push $exit;"
        "push %%ebp;"
        "movl %%esp, %d1;"
        "movw %%ss, %w2;"
        "movw $0x43, %%ax;"
        "movw %%ax, %%ds;"
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;"

        "movl %d4, %%esp;"

        "push %d5;"
        "push %d6;"
        "push %d7;"

        "movl %%esp, %%eax;"
        "push $0x43;"
        "push %%eax;"
        "pushf;"
        "push $0x3B;"
        "push %d3;"
        "xor %%eax, %%eax;"
        "xor %%edx, %%edx;"
        "xor %%ebp, %%ebp;"
        "sti;"
        "xchgw %%bx, %%bx;"
        "iret;"
        "exit:"
        "movl %%eax, %d0"
        : "=m"(exitCode), "=m"(tss.esp0), "=m"(tss.ss0)
        : "m"(entryPoint), "m"(stack), "m"(argv), "m"(argc), "m"(exitFunc)
        :
    );

    kdebug("Программа завершила исполнение с кодом выхода %x (%d).\n", exitCode, exitCode);

    unregisterProcess(pid);

    return exitCode;
}

dword runDynamicELF(byte *file, word pid, char *args) {

    return 0;
}