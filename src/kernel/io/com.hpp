#include <stdint.h>

//
//  COM библиотека
//
//  - Предназначена для управления устройствами,
//    подключёнными через COM-порт.
//

#ifndef _COM_INCL
#define _COM_INCL

// Максимальная скорость обмена данными
#define COM_MAX_BAUD_RATE           115200

// Флаги для управления COM-портом

#define COM_5_DATA_BITS             0b00
#define COM_6_DATA_BITS             0b01
#define COM_7_DATA_BITS             0b10
#define COM_8_DATA_BITS             0b11

#define COM_1_STOP_BIT              0b000
#define COM_2_STOP_BITS             0b100

#define COM_PARITY_NONE             0b000000
#define COM_PARITY_ODD              0b001000
#define COM_PARITY_EVEN             0b011000
#define COM_PARITY_MARK             0b101000
#define COM_PARITY_SPACE            0b111000

#define COM_INT_REC_DATA_RDY        0b1
#define COM_INT_TRA_EMPTY           0b10
#define COM_INT_REC_LINE            0b100
#define COM_INT_MODEM_STATUS        0b1000

#define COM_FIFO_ENABLE             0b1
#define COM_FIFO_CLEAR_REC          0b10
#define COM_FIFO_CLEAR_TRA          0b100
#define COM_FIFO_DMA_MODE_0         0b0000
#define COM_FIFO_DMA_MODE_1         0b1000
#define COM_FIFO_INT_TRIG1          0b00000000
#define COM_FIFO_INT_TRIG4          0b01000000
#define COM_FIFO_INT_TRIG8          0b10000000
#define COM_FIFO_INT_TRIG14         0b11000000

#define COM_MCR_DATATERMRDY         0b1
#define COM_MCR_REQTOSEND           0b10
#define COM_MCR_OUT1                0b100
#define COM_MCR_OUT2                0b1000
#define COM_MCR_LOOP                0b10000

#define COM_LSR_DATAREAD_RDY        0b1
#define COM_LSR_OVERRUN_ERR         0b10
#define COM_LSR_PARITY_ERR          0b100
#define COM_LSR_FRAMING_ERR         0b1000
#define COM_LSR_BREAK_IN            0b10000
#define COM_LSR_DATASEND_RDY        0b100000
#define COM_LSR_TRANS_EMPTY         0b1000000
#define COM_LSR_IMPEND_ERR          0b10000000

#define COM_MSR_CLEAR_TO_SEND       0b10000
#define COM_MSR_DATA_SET_READY      0b100000
#define COM_MSR_RING_INDICATOR      0b1000000
#define COM_MSR_DATA_CARRIER_DETECT 0b10000000

#define COM_IIR_RECEIVE_LINE_STATUS 0b110
#define COM_IIR_RECEIVE_DATA_AVL    0b100
#define COM_IIR_TRANS_HOL_REG_EMPTY 0b010
#define COM_IIR_MODEM_STATUS        0b000
#define COM_IIR_FIFO_UNAVAILABLE    0b00000000
#define COM_IIR_FIFO_UNUSABLE       0b10000000
#define COM_IIR_FIFO_AVAILABLE      0b11000000

// Инициализированные порты
extern bool initPorts[4];

/// @brief Устанавливает скорость обмена на порте.
/// @param portNum Номер порта
/// @param baudRate Скорость
/// @return Флаг успеха
void setBaudRate(uint8_t portNum, uint32_t baudRate);

/// @brief Инициализирует выбранный COM-порт.
/// @param port Номер порта
/// @return Флаг успеха
bool initCom(uint8_t port);

/// @brief Определяет тип UART в компьютере.
void identifyUART();

/// @brief Записывает на данный COM-порт строку.
/// @param str Строка
/// @param port Номер порта
void writeCom(const char str[], uint8_t port);

/// @brief Записывает на данный COM-порт байт.
/// @param b Байт
/// @param port Порт
void writeCom(uint8_t b, uint8_t port);

/// @brief Считывает данные COM.
/// @param am Длина блока данных, Б
/// @param dst Указатель, куда записать данные
void readCom(uint16_t am, uint8_t* dst);

/// @brief Записывает данные с порта в буфер.
/// @param port Порт
void comIn(uint8_t port);

/// @brief Получает IO-порт процессора для данного COM-порта.
/// @param port Порт
/// @return IO-порт процессора
uint16_t getIOPort(uint8_t port);

uint32_t comWriteBufferLength(uint8_t port);

uint32_t comReadBufferLength(uint8_t port);

/// @brief Высылает на COM-порт один байт.
/// @param port Порт
void comSend(uint8_t port);

/// @brief Очищает COM-буферы порта.
/// @param port Порт
void cleanComBuffers(uint8_t port);

/// @brief Очищает COM-буфер записи порта.
/// @param port Порт
void cleanComWBuffer(uint8_t port);

/// @brief Очищает COM-буфер чтения порта.
/// @param port Порт
void cleanComRBuffer(uint8_t port);

/// @brief Включает прерывание "Буфер передачи пуст" на порте.
/// @param port Порт
void enableTraInt(uint8_t port);

/// @brief Отключает прерывание "Буфер передачи пуст" на порте.
/// @param port Порт
void disableTraInt(uint8_t port);

//=========================

/// @brief Высылает отладочный текст на отладчик.
/// @param text Текст
void kdebug(const char* text, ...);

uint32_t getComBaseR(uint8_t port);

uint32_t getComBaseW(uint8_t port);

#endif