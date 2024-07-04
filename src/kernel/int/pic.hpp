//
//  Библиотека для управления контроллером прерываний.
//
#include <stdint.h>
#ifndef IO_SIG
#include "../io/io.hpp"
#endif

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

/// @brief Устанавливает сдвиги для прерываний.
/// @param offset1 Сдвиг главного PICа
/// @param offset2 Сдвиг побочного PICа
void setPICOffsets(uint8_t offset1, uint8_t offset2);

/// @brief Запрещает данное IRQ.
/// @param irq Номер IRQ
void maskIRQ(uint8_t irq);

/// @brief Разрешает данное IRQ.
/// @param irq Номер IRQ
void unmaskIRQ(uint8_t irq);

/// @brief Достаёт регистр обрабатываемых прерываний.
uint16_t getISR();

/// @brief Достаёт регистр прерываний, ожидающих обработки.
uint16_t getIRR();

/// @brief Достаёт "маску" IRQ. 
uint16_t getIRQMask();