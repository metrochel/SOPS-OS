//
//  Библиотека для тайминга
//
//  - Управляет временем. Ну, почти.
//
#include <stdint.h>
#ifndef IO_SIG
#include "../io/io.hpp"
#endif

#define PIT_CHANNEL_0_PORT  0x40
#define PIT_CHANNEL_1_PORT  0x41
#define PIT_CHANNEL_2_PORT  0x42
#define PIT_MODE_CMD_PORT   0x43

#define PIT_CMD_CHAN0       0b00000000
#define PIT_CMD_CHAN1       0b01000000
#define PIT_CMD_CHAN2       0b10000000
#define PIT_CMD_READBACK    0b11000000

#define PIT_CMD_ACCESS_LCVC 0b000000
#define PIT_CMD_ACCESS_LO   0b010000
#define PIT_CMD_ACCESS_HI   0b100000
#define PIT_CMD_ACCESS_HILO 0b110000

#define PIT_CMD_OPMODE_0    0b0000
#define PIT_CMD_OPMODE_1    0b0010
#define PIT_CMD_OPMODE_2    0b0100
#define PIT_CMD_OPMODE_3    0b0110
#define PIT_CMD_OPMODE_4    0b1000
#define PIT_CMD_OPMODE_5    0b1010

#define PIT_CMD_BCD         1
#define PIT_CMD_NO_BCD      0

/// @brief Устанавливает счётчик PITа.
/// @param counter Значение счётчика
void setPITCounter(uint16_t counter);

/// @brief Вычисляет значение счётчика PITа для данного периода.
/// @param mcs Период
/// @return Искомое значение счётчика
uint16_t computeCounter(uint32_t mcs);

/// @brief Задаёт PITу период.
/// @param microseconds Период
void setPITTimer(uint32_t microseconds);