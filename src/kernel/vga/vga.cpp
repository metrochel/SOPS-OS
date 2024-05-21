#include "vga.hpp"

uint32_t *frameBufferPtr;
uint16_t pitch;
uint8_t  bpp;

/// @brief Кодирует цвет RGB в воспринимаемое видеокартой число.
/// @param col Цвет
/// @return Цвет в формате числа
uint32_t encodeRGB(RGBColor col) {
    if (bpp == 32 || bpp == 24) {
        return col.r << 16 + col.g << 8 + col.b;
    }
    col.r %= (1 << bpp / 3);
    col.g %= (1 << (bpp / 3 + bpp % 3));
    col.b %= (1 << bpp / 3);
    uint32_t res = col.r << (2*(bpp / 3) + 1) + col.g << (bpp / 3) + col.b;
    return res;
}

/// @brief Помещает пиксел в видеопамять.
/// @param x Абсцисса пиксела
/// @param y Ордината пиксела
/// @param col Цвет пиксела
void putpixel(uint16_t x, uint16_t y, uint32_t col) {
    uint32_t offset = y * pitch + x;
    uint32_t *pixptr = frameBufferPtr + offset;
    *pixptr = col;
}

/// @brief Помещает по выбранному сдвигу пиксел выбранного цвета.
/// @param offset Сдвиг по буферу
/// @param col Цвет
void putpixel(uint32_t offset, uint32_t col) {
    uint32_t *pixptr = frameBufferPtr + offset;
    *pixptr = col;
}

/// @brief Размещает на данных координатах прямоугольник.
/// @param x1 Абсцисса первой вершины
/// @param y1 Ордината первой вершины
/// @param x2 Абсцисса второй вершины
/// @param y2 Ордината второй вершины
/// @param col Цвет прямоугольника
void putrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t col) {
    uint32_t offset1 = y1 * pitch + x1;
    for (uint32_t y = 0; y <= y2-y1; y++) {
        for (uint32_t x = 0; x <= x2-x1; x++) {
            putpixel(offset1 + y * pitch + x, col);
        }
    }
}