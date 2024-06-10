/*

Наконец-то это случилось! Теперь мы можем писать
на плюсах!

Так будет сильно проще, чем на ассемблере.

*/

#include <stdint.h>
#include "graphics/graphics.hpp"
#include "graphics/glyphs.hpp"
#include "io/com.hpp"
#include "int/int.hpp"
#include "int/pic.hpp"
#include "memmgr/memmgr.hpp"
#include "timing/pit.hpp"

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
    unmaskIRQ(3);
    unmaskIRQ(4);
    identifyUART();

    kprint("Добро пожаловать в СОПС вер. 1.0.0-АЛЬФА!\n\n");
    if (initCom(1))
        kprint("COM1 успешно инициализирован!\n");
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

    setPITTimer(500000);
    unmaskIRQ(0);

    kprint("\n>");
    while (true) {
    }
}