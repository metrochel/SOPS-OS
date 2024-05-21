/*

Наконец-то это случилось! Теперь мы можем писать
на плюсах!

Так будет сильно проще, чем на ассемблере.

*/

#include <stdint.h>
#include "vga/vga.hpp"

// Структура с данными из загрузчика
struct BootLoaderData {
    char CPUID_Vendor[12];      // Имя производителя процессора
    uint32_t CPUID_Flags1;      // Флаги ЦП-1
    uint32_t CPUID_Flags2;      // Флаги ЦП-2
    VBEModeInfo VBEInfo;        // Информация о графическом режиме
};

// Указатель на данные загрузчика
BootLoaderData* bld;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
    pitch = bld->VBEInfo.Pitch;
    frameBufferPtr = (uint32_t*)(0b11111111110000000000000000000000 + bld->VBEInfo.FrameBuffer & 0xFFF);
}


/// @brief Точка входа в ядро.
int main() {
    initBLD();
    putrect(100,100,500,300,0xFFFFFF);
    return 0;
}