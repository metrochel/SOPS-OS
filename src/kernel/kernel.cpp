/*
* 
* Наконец-то это случилось! Теперь мы можем писать
* на плюсах!
* 
* Так будет сильно проще, чем на ассемблере.
*
*/

#include "kernel.hpp"
#include "acpi/acpi.hpp"
#include "graphics/graphics.hpp"
#include "graphics/glyphs.hpp"
#include "io/com.hpp"
#include "io/ps2.hpp"
#include "int/int.hpp"
#include "int/pic.hpp"
#include "pci/pci.hpp"
#include "disk/disk.hpp"
#include "file/file.hpp"
#include "memmgr/memmgr.hpp"
#include "keyboard/keyboard.hpp"
#include "run/process.hpp"
#include "timing/pit.hpp"
#include "timing/cmos.hpp"
#include "str/str.hpp"
#include "util/util.hpp"
#include "util/nums.hpp"
#include "util/math.hpp"
#include "dbg/dbg.hpp"
#include "shell/shell.hpp"
#include "cpu/gdt.hpp"

// Указатель на данные загрузчика
BootLoaderData* bld;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
    bld->DiskNo &= 0x7F;
}

/// @brief Точка входа в ядро.
int main() {
    disableInts();
    initBLD();
    initMemMgr();
    initGraphics();
    initText();
    setPICOffsets(0x20, 0x28);
    initInts();
    initGDT();
    unmaskIRQ(1);
    unmaskIRQ(2);
    unmaskIRQ(3);
    unmaskIRQ(4);
    identifyUART();
    initProcessesLib();
    initDisks();
    initFiles();

    kprint("Добро пожаловать в СОПС вер. 1.0.0-АЛЬФА!\n\n");

    disableInts();
    outb(0x70, 0x8B);
    byte cmosStatusB = inb(0x71);
    cmosStatusB |= 16;
    outb(0x70, 0x8B);
    outb(0x71, cmosStatusB);
    outb(0x70, 0x8C);
    inb(0x71);
    outb(0x70, 0x00);
    unmaskIRQ(8);
    enableInts();

    if (initCom(1)) {
        kprint("COM1 успешно инициализирован!\n");
        kdebug("=============== ОТЛАДЧИК СОПС ===============\nВерсия 1.0.0-АЛЬФА\n\n");
    }
    if (initCom(2))
        kprint("COM2 успешно инициализирован!\n");
    if (initCom(3)) {
        kprint("COM3 успешно инициализирован!\n");
        kwarn("ВНИМАНИЕ: COM3 на данный момент не работает.\nИзвините, пока не доделали.\n");
    }
    if (initCom(4)) {
        kprint("COM4 успешно инициализирован!\n");
        kwarn("ВНИМАНИЕ: COM4 на данный момент не работает.\nИзвините, пока не доделали.\n");
    }


    if (!initACPI()) {
        kerror("ОШИБКА: ACPI не инициализирован\n");
    }
    else
        kprint("ACPI успешно инициализирован!\n");

    if (!initPS2())
        kerror("ОШИБКА: Контроллер PS/2 не инициализирован\n");
    else {
        kprint("Контроллер PS/2 успешно инициализирован!\n");
        if (firstPortAvailable && initKB()) {
            kprint("Клавиатура успешно инициализирована!\n");
        }
        else {
            kerror("ОШИБКА: Не удалось инициализировать клавиатуру\n");
            maskIRQ(1);
        }
    }

    setPITTimer(500000);
    unmaskIRQ(0);

    unmaskIRQ(14);
    unmaskIRQ(15);
    if (initIDE()) {
        kprint("Контроллер IDE успешно инициализирован!\n");
    } else {
        kerror("ОШИБКА: Контроллер IDE не инициализирован\n");
        maskIRQ(14);
        maskIRQ(15);
    }

    while (kgettime() == Time()) {tinyWait();}
    kprint("Сейчас ");
    Time time = kgettime();
    char* timestr = (char*)kmalloc(256);
    timestr += time.asStringWeekday(timestr);
    *timestr++ = ',';
    *timestr++ = ' ';
    time.asStringFull(timestr);
    timestr -= 6;
    kprint(timestr);
    kprint(".\n");
    kfree(timestr);

    byte driveNo = bld->DiskNo;

    if (initFAT(driveNo)) {
        kprint("FAT32 успешно инициализирована!\n");
        initKernelMap(driveNo);
    } else
        kerror("ОШИБКА: FAT32 не инициализирована\n");
        
    kdebugwait();
    shellMain(driveNo);
}