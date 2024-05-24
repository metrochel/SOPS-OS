/*

Наконец-то это случилось! Теперь мы можем писать
на плюсах!

Так будет сильно проще, чем на ассемблере.

*/

#include <stdint.h>
#include "graphics/graphics.hpp"
#include "graphics/glyphs.hpp"

// Структура с данными из загрузчика
struct BootLoaderData {
    char CPUID_Vendor[12];      // Имя производителя процессора
    uint32_t CPUID_Flags1;      // Флаги ЦП-1
    uint32_t CPUID_Flags2;      // Флаги ЦП-2
    VBEModeInfo VBEInfo;        // Информация о графическом режиме
} __attribute__((packed));

// Указатель на данные загрузчика
BootLoaderData* bld;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
    pitch = bld->VBEInfo.Pitch;
    frameBufferPtr = (uint8_t*)(0xFC000000 + (bld->VBEInfo.FrameBuffer & 0xFFF));
    bpp = bld->VBEInfo.BPP;
    redmask = bld->VBEInfo.RedMaskSize;
    redshift = bld->VBEInfo.RedPos;
    greenmask = bld->VBEInfo.GreenMaskSize;
    greenshift = bld->VBEInfo.GreenPos;
    bluemask = bld->VBEInfo.BlueMaskSize;
    blueshift = bld->VBEInfo.BluePos;
}

/// @brief Точка входа в ядро.
int main() {
    initBLD();
    kprint("Добро пожаловать в СОПС вер. 1.0.0-АЛЬФА!");
    return 0;
}