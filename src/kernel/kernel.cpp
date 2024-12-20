/*
* 
* Наконец-то это случилось! Теперь мы можем писать
* на плюсах!
* 
* Так будет сильно проще, чем на ассемблере.
*
*/

#include "acpi/acpi.hpp"
#include "graphics/graphics.hpp"
#include "graphics/glyphs.hpp"
#include "io/com.hpp"
#include "io/ps2.hpp"
#include "int/int.hpp"
#include "int/pic.hpp"
#include "pci/pci.hpp"
#include "disk/disk.hpp"
#include "memmgr/memmgr.hpp"
#include "keyboard/keyboard.hpp"
#include "timing/pit.hpp"
#include "timing/cmos.hpp"
#include "str/str.hpp"
#include "util/util.hpp"
#include "util/nums.hpp"
#include "dbg/dbg.hpp"
#include "shell/shell.hpp"
#include "fat/fat.hpp"

// Структура с данными из загрузчика
struct BootLoaderData {
    byte DiskNo;                // Номер диска, с которого загрузилась СОпС
    char CPUID_Vendor[12];      // Имя производителя процессора
    dword CPUID_Flags1;         // Флаги ЦП-1
    dword CPUID_Flags2;         // Флаги ЦП-2
    VBEModeInfo VBEInfo;        // Информация о графическом режиме
    dword MaxAddr1;
    dword MaxAddr2;
} __attribute__((packed));

// Указатель на данные загрузчика
BootLoaderData* bld;

byte *stdin = (byte*)0x9300;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
    bld->DiskNo &= 0x7F;
}

/// @brief Инициализирует графический драйвер.
void initGraphics() {
    pitch = bld->VBEInfo.Pitch;
    frameBufferPtr = (byte*)(0xFC000000 + (bld->VBEInfo.FrameBuffer & 0xFFF));
    bpp = bld->VBEInfo.BPP;
    redmask = bld->VBEInfo.RedMaskSize;
    redshift = bld->VBEInfo.RedPos;
    greenmask = bld->VBEInfo.GreenMaskSize;
    greenshift = bld->VBEInfo.GreenPos;
    bluemask = bld->VBEInfo.BlueMaskSize;
    blueshift = bld->VBEInfo.BluePos;
    screenWidth = bld->VBEInfo.Width;
    screenHeight = bld->VBEInfo.Height;

    defaultTextCol = encodeRGB(1,1,1);
    defaultBGCol = encodeRGB(0,0,0);
    warnTextCol = encodeRGB(1,1,0);
    warnBGCol = encodeRGB(0.5,0.5,0);
    errorTextCol = encodeRGB(1,0,0);
    errorBGCol = encodeRGB(0.5,0,0);
}

/// @brief Создаёт немного временного пространства.
void initTempSpace() {
    createPages(0x2000000, 0x7000000, 8);
    dword *tmpPtr = (dword*)0x2000000;
    for (dword i = 0; i < 8*1024/4; i++) {
        tmpPtr[i] = 0;
    }
}

/// @brief Точка входа в ядро.
int main() {
    disableInts();
    initBLD();
    initTempSpace();
    initGraphics();
    setPICOffsets(0x20, 0x28);
    initInts();
    unmaskIRQ(1);
    unmaskIRQ(2);
    unmaskIRQ(3);
    unmaskIRQ(4);
    identifyUART();

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
    char* timestr = (char*)0x11000;
    timestr += time.asStringWeekday(timestr);
    *timestr++ = ',';
    *timestr++ = ' ';
    time.asStringFull(timestr);
    timestr -= 6;
    kprint(timestr);
    kprint(".\n");

    byte driveNo = bld->DiskNo;

    if (initFAT(driveNo))
        kprint("FAT32 успешно инициализирована!\n");
    else
        kerror("ОШИБКА: FAT32 не инициализирована\n");

    shellMain(driveNo);
}