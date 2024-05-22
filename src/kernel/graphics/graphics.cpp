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
/// @param col Цвет
/// @return Цвет в формате числа
uint32_t encodeRGB(RGBColor col) {
    uint32_t encCol = 0;

    encCol += (col.r << ((1 << redmask) - 1)) << redshift;
    encCol += (col.g << ((1 << greenmask) - 1)) << greenshift;
    encCol += (col.b << ((1 << bluemask) - 1)) << blueshift;

    return encCol;
}

/// @brief Помещает пиксел в видеопамять.
/// @param x Абсцисса пиксела
/// @param y Ордината пиксела
/// @param col Цвет пиксела
void putpixel(uint16_t x, uint16_t y, uint32_t col) {
    uint32_t offset = y * pitch + (x * (bpp/8));
    putpixel(offset, col);
}

/// @brief Помещает по выбранному сдвигу пиксел выбранного цвета.
/// @param offset Сдвиг по буферу
/// @param col Цвет
void putpixel(uint32_t offset, uint32_t col) {
    uint8_t *pixptr = frameBufferPtr + offset;
    for (int i = 0; i < bpp/8; i++) {
        *pixptr = (uint8_t)(col & 0xFF);
        pixptr++;
        col >>= 8;
    }
}

/// @brief Размещает на данных координатах прямоугольник.
/// @param x1 Абсцисса первой вершины
/// @param y1 Ордината первой вершины
/// @param x2 Абсцисса второй вершины
/// @param y2 Ордината второй вершины
/// @param col Цвет прямоугольника
void putrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t col) {
    uint32_t offset = y1 * pitch + x1;
    uint32_t *dbgPtr = (uint32_t*)0x100300;
    *dbgPtr = pitch;
    dbgPtr++;
    for (uint32_t y = y1; y <= y2; y++) {
        for (uint32_t x = x1; x <= x2; x++) {
            putpixel(offset, col);
            offset += bpp/8;
        }
        offset += pitch;
        offset -= (x2-x1+1) * (bpp/8);
        *dbgPtr = offset;
        dbgPtr++;
    }
}