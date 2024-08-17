#include "graphics.hpp"

uint8_t *frameBufferPtr;
uint16_t pitch;
uint8_t  bpp;

uint8_t redmask;
uint8_t redshift;
uint8_t greenmask;
uint8_t greenshift;
uint8_t bluemask;
uint8_t blueshift;
uint8_t reservedmask;
uint8_t reservedshift;

/// @brief Кодирует цвет RGB в воспринимаемое видеокартой число.
/// @param r Красный канал
/// @param g Зелёный канал
/// @param b Синий канал
/// @return Цвет в формате числа
uint32_t encodeRGB(float r, float g, float b) {
    uint32_t encCol = 0;
    encCol += (uint8_t)(r * ((1 << redmask) - 1)) << redshift;
    encCol += (uint8_t)(g * ((1 << greenmask) - 1)) << greenshift;
    encCol += (uint8_t)(b * ((1 << bluemask) - 1)) << blueshift;

    return encCol;
}

void putpixel(uint16_t x, uint16_t y, uint32_t col) {
    uint32_t offset = y * pitch + (x * (bpp/8));
    putpixel(offset, col);
}

void putrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t col) {
    uint32_t offset = y1 * pitch + x1*(bpp/8);
    for (uint32_t y = y1; y <= y2; y++) {
        for (uint32_t x = x1; x <= x2; x++) {
            putpixel(offset, col);
            offset += bpp/8;
        }
        offset += pitch;
        offset -= (x2-x1+1) * (bpp/8);
    }
}