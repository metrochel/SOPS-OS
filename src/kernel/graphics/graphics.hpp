#include <stdint.h>

//
//  Графическая библиотека для адаптера VGA
//
//  - Название говорит само за себя.
//    Отвечает за графику.
//

#ifndef _GRAPHICS_INCL
#define _GRAPHICS_INCL

// ============================================== Основы ============================

// Указатель на буфер кадра
extern uint8_t* frameBufferPtr;
// Длина строки в байтах
extern uint16_t  pitch;
// Число бит на пиксел
extern uint8_t   bpp;

extern uint8_t redmask;
extern uint8_t redshift;
extern uint8_t greenmask;
extern uint8_t greenshift;
extern uint8_t bluemask;
extern uint8_t blueshift;
extern uint8_t reservedmask;
extern uint8_t reservedshift;

extern uint16_t screenWidth;
extern uint16_t screenHeight;

// Структура режима VBE
struct VBEModeInfo {
    // Основной блок данных
    uint16_t Attributes;         // Флаги режима
    uint8_t  WindowA;            // (не используется)
    uint8_t  WindowB;            // (не используется)
    uint16_t Granularity;        // (не используется)
    uint16_t WindowSize;         // (не используется)
    uint16_t SegmentA;           // (не используется)
    uint16_t SegmentB;           // (не используется)
    uint32_t BankSwitchPtr;      // (не используется)
    uint16_t Pitch;              // Количество байтов на строку
    uint16_t Width;              // Ширина экрана в пикселах
    uint16_t Height;             // Высота экрана в пикселах
    uint8_t  WChar;              // (не используется)
    uint8_t  YChar;              // (не используется)
    uint8_t  Planes;             // (не используется)
    uint8_t  BPP;                // Количество бит на пиксел
    uint8_t  NumBanks;           // (не используется)
    uint8_t  MemoryModel;        // (не используется)
    uint8_t  BankSize;           // (не используется)
    uint8_t  ImagePages;         // (не используется)
    uint8_t  Reserved0;          // (резервировано)
    
    // Блок данных режима прямого цвета
    uint8_t  RedMaskSize;        // Размер маски в коде пиксела, отвечающей за красный канал
    uint8_t  RedPos;             // Сдвиг маски в коде пиксела, отвечающей за красный канал
    uint8_t  GreenMaskSize;      // Размер маски в коде пиксела, отвечающей за зелёный канал
    uint8_t  GreenPos;           // Сдвиг маски в коде пиксела, отвечающей за зелёный канал
    uint8_t  BlueMaskSize;       // Размер маски в коде пиксела, отвечающей за синий канал
    uint8_t  BluePos;            // Сдвиг маски в коде пиксела, отвечающей за синий канал
    uint8_t  ReservedMask;       // Размер маски в коде пиксела, биты которой резервированы
    uint8_t  ReservedPos;        // Сдвиг маски в коде пиксела, биты которой резервированы
    uint8_t  DirectColorAttr;    // Свойства режима прямого цвета

    // Блок данных о буфере кадра
    uint32_t FrameBuffer;        // Буфер кадра (куда пикселы класть)
    uint32_t OffScreenMemOffset; // Сдвиг в буфере кадров, память, начиная с которого, на экран не выводится
    uint32_t OffScreenMemSize;   // Размер памяти в буфере кадров, которая не выводится на экран
    uint8_t  Reserved1[206];     // (резервировано)
} __attribute__ ((packed));
// Фууууух... Огромная куча полей была оставлена для обратной совместимости,
// но сейчас они не используются.

uint32_t encodeRGB(float r, float g, float b);

/// @brief Помещает по выбранному сдвигу пиксел выбранного цвета.
/// @param offset Сдвиг по буферу
/// @param col Цвет
void putpixel(uint32_t offset, uint32_t col);

/// @brief Помещает пиксел в видеопамять.
/// @param x Абсцисса пиксела
/// @param y Ордината пиксела
/// @param col Цвет пиксела
void putpixel(uint16_t x, uint16_t y, uint32_t col);

/// @brief Размещает на данных координатах прямоугольник.
/// @param x1 Абсцисса первой вершины
/// @param y1 Ордината первой вершины
/// @param x2 Абсцисса второй вершины
/// @param y2 Ордината второй вершины
/// @param col Цвет прямоугольника
void putrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t col);

#endif