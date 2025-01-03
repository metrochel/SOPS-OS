#include "graphics.hpp"
#include "../kernel.hpp"

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