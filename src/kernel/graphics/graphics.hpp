#include <stdint.h>

//
//  Графическая библиотека для адаптера VGA
//
//  - Название говорит само за себя.
//    Отвечает за графику.
//

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

// RGB-цвет
struct RGBColor {
    uint8_t r;      // Красный канал
    uint8_t g;      // Зелёный канал
    uint8_t b;      // Синий канал
};

uint32_t encodeRGB(RGBColor col);

void putpixel(uint16_t x, uint16_t y, uint32_t col);
void putpixel(uint32_t offset, uint32_t col);

void putrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t col);

// ======================================== Работа с текстом ============================================

// Символ
struct Glyph {
    uint32_t lines[8];      // Бит-поля, отмечающие пикселы символа
} __attribute__((packed));

/// @brief Выдаёт разметку символа, воспринимаемую системой.
/// @param code Код символа в Юникоде
/// @return Разметка
Glyph getglyph(uint16_t code);

/// @brief Размещает символ на координатах.
/// @param glyph Символ
/// @param x Абсцисса символа (отн. левого верхнего угла)
/// @param y Ордината символа (отн. левого верхнего угла)
/// @param letter_col Цвет самого символа
/// @param back_col Цвет заднего фона символа
void putglyph(Glyph glyph, uint16_t x, uint16_t y, uint32_t letter_col, uint32_t back_col);