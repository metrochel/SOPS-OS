#include "graphics_old.hpp"
#include "kernel.hpp"
#include "memmgr/memmgr.hpp"
#include "libk/util.hpp"

byte *frameBufferPtr;
word pitch;
byte bpp;

byte redmask;
byte redshift;
byte greenmask;
byte greenshift;
byte bluemask;
byte blueshift;
byte reservedmask;
byte reservedshift;

Character *textBuffer;
Character *auxTextBuffer;
dword textBufferBankSize;

void initGraphics() {
    pitch = bld->VBEInfo.pitch;
    frameBufferPtr = (byte*)(0xFC000000 + (bld->VBEInfo.frame_buffer & 0xFFF));
    bpp = bld->VBEInfo.bpp;
    redmask = bld->VBEInfo.red_mask_size;
    redshift = bld->VBEInfo.red_pos;
    greenmask = bld->VBEInfo.green_mask_size;
    greenshift = bld->VBEInfo.green_pos;
    bluemask = bld->VBEInfo.blue_mask_size;
    blueshift = bld->VBEInfo.blue_pos;
    screenWidth = bld->VBEInfo.width;
    screenHeight = bld->VBEInfo.height;
}

/// @brief Кодирует цвет RGB в воспринимаемое видеокартой число.
/// @param r Красный канал
/// @param g Зелёный канал
/// @param b Синий канал
/// @return Цвет в формате числа
dword encodeRGB(float r, float g, float b) {
    dword encCol = 0;
    encCol += (byte)(r * ((1 << redmask) - 1)) << redshift;
    encCol += (byte)(g * ((1 << greenmask) - 1)) << greenshift;
    encCol += (byte)(b * ((1 << bluemask) - 1)) << blueshift;

    return encCol;
}

void putpixel(word x, word y, dword col) {
    dword offset = y * pitch + (x * (bpp/8));
    putpixel(offset, col);
}

void putrect(word x1, word y1, word x2, word y2, dword col) {
    dword offset = y1 * pitch + x1*(bpp/8);
    for (dword y = y1; y <= y2; y++) {
        for (dword x = x1; x <= x2; x++) {
            putpixel(offset, col);
            offset += bpp/8;
        }
        offset += pitch;
        offset -= (x2-x1+1) * (bpp/8);
    }
}