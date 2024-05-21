#include <stdint.h>

//
//  Графическая библиотека для адаптера VGA
//
//  - Название говорит само за себя.
//    Отвечает за графику.
//

// Указатель на буфер кадра
extern uint32_t* frameBufferPtr;
// Длина строки в байтах
extern uint16_t  pitch;
// Число бит на пиксел
extern uint8_t   bpp;

// Структура режима VBE
struct VBEModeInfo {
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
    
    uint8_t  RedMask;            // (не используется)
    uint8_t  RedPos;             // (не используется)
    uint8_t  GreenMask;          // (не используется)
    uint8_t  GreenPos;           // (не используется)
    uint8_t  BlueMask;           // (не используется)
    uint8_t  BluePos;            // (не используется)
    uint8_t  ReservedMask;       // (не используется)
    uint8_t  ReservedPos;        // (не используется)
    uint8_t  DirectColorAttr;    // (не используется)

    uint32_t FrameBuffer;        // Буфер кадра (куда пиксели класть)
    uint32_t OffScreenMemOffset; // Сдвиг в буфере кадров, память, начиная с которого, на экран не выводится
    uint32_t OffScreenMemSize;   // Размер памяти в буфере кадров, которая не выводится на экран
    uint8_t  Reserved1[206];      // (резервировано)
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