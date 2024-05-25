#include <stdint.h>

//
//  COM библиотека
//
//  - Предназначена для управления устройствами,
//    подключёнными через COM-порт.
//

// Максимальная скорость обмена данными
#define MAX_BAUD_RATE 115200

// Буфер обмена данными
extern uint8_t comBuffer[256];

/// @brief Устанавливает скорость обмена на порте.
/// @param portNum Номер порта
/// @param baudRate Скорость
/// @return Флаг успеха
void setBaudRate(uint8_t portNum, uint32_t baudRate);

/// @brief Получает IO-порт процессора для данного COM-порта.
/// @param port Номер порта
/// @return IO-порт процессора
uint16_t getIOPort(uint8_t port);

