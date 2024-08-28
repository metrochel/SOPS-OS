
//
//  Библиотека для работы с PS/2
//
//  - Обрабатывает сигналы с PS/2-контроллера.
//
#ifndef _PS2_INCL
#define _PS2_INCL

#include "../util/nums.hpp"
#include "../io/io.hpp"

#define PS2_DATA                    0x60
#define PS2_STATUS                  0x64
#define PS2_COMMAND                 0x64

#define PS2_CMD_READFROMRAM         0x20
#define PS2_CMD_WRITETORAM          0x60
#define PS2_CMD_DISABLE_SEC_PORT    0xA7
#define PS2_CMD_ENABLE_SEC_PORT     0xA8
#define PS2_CMD_TEST_SEC_PORT       0xA9
#define PS2_CMD_TEST_CONTROLLER     0xAA
#define PS2_CMD_TEST_FIR_PORT       0xAB
#define PS2_CMD_DUMP_RAM            0xAC
#define PS2_CMD_DISABLE_FIR_PORT    0xAD
#define PS2_CMD_ENABLE_FIR_PORT     0xAE
#define PS2_CMD_READ_CONT_INPUT     0xC0
#define PS2_CMD_COPY_I03TOS47       0xC1
#define PS2_CMD_COPY_I47TOS47       0xC2
#define PS2_CMD_READCOP             0xD0
#define PS2_CMD_WRITETOCOP          0xD1
#define PS2_CMD_WRITETOFIRPORTOUT   0xD2
#define PS2_CMD_WRITETOSECPORTOUT   0xD3
#define PS2_CMD_WRITETOSECPORTIN    0xD4

#define PS2_DEVCMD_RESET            0xFF

#define PS2_CCB_FIRSTPORT_INT       0b1
#define PS2_CCB_SECONDPORT_INT      0b10
#define PS2_CCB_POST_PASSED         0b100
#define PS2_CCB_FIRSTPORT_CLOCK     0b10000
#define PS2_CCB_SECONDPORT_CLOCK    0b100000
#define PS2_CCB_FIRSTPORT_TRANSLATE 0b1000000

#define PS2_COP_SYSRESET            0b1
#define PS2_COP_A20                 0b10
#define PS2_COP_SECPORT_CLOCK       0b100
#define PS2_COP_SECPORT_DATA        0b1000
#define PS2_COP_OUTBUFFULL1         0b10000
#define PS2_COP_OUTBUFFULL2         0b100000
#define PS2_COP_FIRPORT_CLOCK       0b1000000
#define PS2_COP_FIRPORT_DATA        0b10000000

// Флаг доступности первого порта PS/2
extern bool firstPortAvailable;
// Флаг доступности второго порта PS/2
extern bool secondPortAvailable;

/// @brief Инициализирует PS/2-контроллер.
/// @return Успешность операции
bool initPS2();

/// @brief Отправляет команду на PS/2-контроллер.
/// @param cmd Команда
/// @return Успешность операции
bool sendPS2ConCommand(byte cmd);

/// @brief Отправляет команду на PS/2-устройство.
/// @param port Номер порта устройства
/// @param cmd Команда
/// @return Успешность операции
bool sendPS2DevCommand(byte port, byte cmd);

#endif