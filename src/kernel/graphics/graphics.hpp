//
//  Графическая библиотека для адаптера VGA
//
//  - Название говорит само за себя.
//    Отвечает за графику.
//

#ifndef _GRAPHICS_INCL
#define _GRAPHICS_INCL

#include "../util/nums.hpp"

// ============================================== Основы ============================

// Указатель на буфер кадра
extern byte* frameBufferPtr;
// Длина строки в байтах
extern word  pitch;
// Число бит на пиксел
extern byte   bpp;

extern byte redmask;
extern byte redshift;
extern byte greenmask;
extern byte greenshift;
extern byte bluemask;
extern byte blueshift;
extern byte reservedmask;
extern byte reservedshift;

extern word screenWidth;
extern word screenHeight;

// Структура режима VBE
struct VBEModeInfo {
    // Основной блок данных
    word  Attributes;         // Флаги режима
    byte  WindowA;            // (не используется)
    byte  WindowB;            // (не используется)
    word  Granularity;        // (не используется)
    word  WindowSize;         // (не используется)
    word  SegmentA;           // (не используется)
    word  SegmentB;           // (не используется)
    dword BankSwitchPtr;      // (не используется)
    word  Pitch;              // Количество байтов на строку
    word  Width;              // Ширина экрана в пикселах
    word  Height;             // Высота экрана в пикселах
    byte  WChar;              // (не используется)
    byte  YChar;              // (не используется)
    byte  Planes;             // (не используется)
    byte  BPP;                // Количество бит на пиксел
    byte  NumBanks;           // (не используется)
    byte  MemoryModel;        // (не используется)
    byte  BankSize;           // (не используется)
    byte  ImagePages;         // (не используется)
    byte  Reserved0;          // (резервировано)
    
    // Блок данных режима прямого цвета
    byte  RedMaskSize;        // Размер маски в коде пиксела, отвечающей за красный канал
    byte  RedPos;             // Сдвиг маски в коде пиксела, отвечающей за красный канал
    byte  GreenMaskSize;      // Размер маски в коде пиксела, отвечающей за зелёный канал
    byte  GreenPos;           // Сдвиг маски в коде пиксела, отвечающей за зелёный канал
    byte  BlueMaskSize;       // Размер маски в коде пиксела, отвечающей за синий канал
    byte  BluePos;            // Сдвиг маски в коде пиксела, отвечающей за синий канал
    byte  ReservedMask;       // Размер маски в коде пиксела, биты которой резервированы
    byte  ReservedPos;        // Сдвиг маски в коде пиксела, биты которой резервированы
    byte  DirectColorAttr;    // Свойства режима прямого цвета

    // Блок данных о буфере кадра
    dword FrameBuffer;        // Буфер кадра (куда пикселы класть)
    dword OffScreenMemOffset; // Сдвиг в буфере кадров, память, начиная с которого, на экран не выводится
    dword OffScreenMemSize;   // Размер памяти в буфере кадров, которая не выводится на экран
    byte  Reserved1[206];     // (резервировано)
} __attribute__ ((packed));
// Фууууух... Огромная куча полей была оставлена для обратной совместимости,
// но сейчас они не используются.

dword encodeRGB(float r, float g, float b);

/// @brief Помещает по выбранному сдвигу пиксел выбранного цвета.
/// @param offset Сдвиг по буферу
/// @param col Цвет
inline void putpixel(dword offset, dword col) {
    byte *pixptr = frameBufferPtr + offset;
    for (int i = 0; i < bpp/8; i++) {
        *pixptr = (byte)(col & 0xFF);
        pixptr++;
        col >>= 8;
    }
};

/// @brief Помещает пиксел в видеопамять.
/// @param x Абсцисса пиксела
/// @param y Ордината пиксела
/// @param col Цвет пиксела
void putpixel(word x, word y, dword col);

/// @brief Размещает на данных координатах прямоугольник.
/// @param x1 Абсцисса первой вершины
/// @param y1 Ордината первой вершины
/// @param x2 Абсцисса второй вершины
/// @param y2 Ордината второй вершины
/// @param col Цвет прямоугольника
void putrect(word x1, word y1, word x2, word y2, dword col);

#endif