/*

Наконец-то это случилось! Теперь мы можем писать
на плюсах!

Так будет сильно проще, чем на ассемблере.

*/

#include <stdint.h>
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
#include "str/str.hpp"
#include "llutil/llutil.hpp"

// Структура с данными из загрузчика
struct BootLoaderData {
    char CPUID_Vendor[12];      // Имя производителя процессора
    uint32_t CPUID_Flags1;      // Флаги ЦП-1
    uint32_t CPUID_Flags2;      // Флаги ЦП-2
    VBEModeInfo VBEInfo;        // Информация о графическом режиме
    uint32_t MaxAddr1;
    uint32_t MaxAddr2;
} __attribute__((packed));

// Указатель на данные загрузчика
BootLoaderData* bld;

uint8_t *stdin = (uint8_t*)0x9300;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
}

/// @brief Инициализирует графический драйвер.
void initGraphics() {
    pitch = bld->VBEInfo.Pitch;
    frameBufferPtr = (uint8_t*)(0xFC000000 + (bld->VBEInfo.FrameBuffer & 0xFFF));
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


/// @brief Точка входа в ядро.
int main() {
    disableInts();
    initBLD();
    initGraphics();
    setPICOffsets(0x20, 0x28);
    initInts();
    unmaskIRQ(1);
    unmaskIRQ(2);
    unmaskIRQ(3);
    unmaskIRQ(4);
    identifyUART();

    kprint("Добро пожаловать в СОПС вер. 1.0.0-АЛЬФА!\n\n");
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

    createPages(0x2000000, 0x5000000, 10);
    uint8_t* out = (uint8_t*)0x2000000;
    readSectorsATA(1, 2, 0, out);
    writeSectorsATA(500, 2, 0, out);
    readSectorsATA(19, 10, 0, out);
    readSectorsATA(1, 2, 0, out);
    writeSectorsATA(345, 5, 0, out);
    while (true) {
        kprint("\n>");
        kread(stdin);
        kprint("\n");
        stdin = (uint8_t*)0x9300;
        if (strcmp((char*)stdin, (char*)"iddisk")) {
            DiskData d = identifyDisk();
            switch (d.DiskType) {
                case DISK_TYPE_NONE:
                    kprint("Диск А не подключён");
                    break;
                case DISK_TYPE_ATA:
                    kprint("Тип диска А - ATA");
                    break;
                case DISK_TYPE_ATAPI:
                    kprint("Тип диска А - ATAPI");
                    break;
                case DISK_TYPE_SATA:
                    kprint("Тип диска А - SATA");
                    break;
                case DISK_TYPE_SATAPI:
                    kprint("Тип диска А - SATAPI");
                    break;
            }
            if (d.DiskType == 0)
                continue;
            if (d.LBA48Supported)
                kprint("\nДиск А поддерживает LBA48");
            else
                kprint("\nДиск А не поддерживает LBA48");
            kprint("\nНа диске А доступно\n    %d секторов в режиме LBA28;\n    %d секторов в режиме LBA48", d.TotalLBA28Sectors, d.TotalLBA48Sectors);
            kprint("\nМаксимальный режим UDMA - %d, активен %d", d.MaxUDMAMode, d.ActUDMAMode);
        } else {
            kerror("ОШИБКА: Команды или исполняемого файла \"");
            kerror((const char*)stdin);
            kerror("\" не существует.\nПроверьте правильность написания команды.");
        }
    }
}