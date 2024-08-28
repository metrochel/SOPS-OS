//
//  Библиотека для управления контроллером прерываний.
//

#ifndef _PIC_INCL
#define _PIC_INCL
#include "../util/nums.hpp"
#include "../io/io.hpp"

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

/// @brief Устанавливает сдвиги для прерываний.
/// @param offset1 Сдвиг главного PICа
/// @param offset2 Сдвиг побочного PICа
void setPICOffsets(byte offset1, byte offset2);

/// @brief Запрещает данное IRQ.
/// @param irq Номер IRQ
void maskIRQ(byte irq);

/// @brief Разрешает данное IRQ.
/// @param irq Номер IRQ
void unmaskIRQ(byte irq);

/// @brief Достаёт регистр обрабатываемых прерываний.
word getISR();

/// @brief Достаёт регистр прерываний, ожидающих обработки.
word getIRR();

/// @brief Достаёт "маску" IRQ. 
word getIRQMask();

#endif