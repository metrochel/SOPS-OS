//
//  Библиотека для работы с клавиатурой
//
//  - Надстройка к библиотеке PS/2 для клавиатуры.
//

#ifndef _KB_INCL
#define _KB_INCL

#include "../util/nums.hpp"
#include "../io/ps2.hpp"

#define KB_BUF_BASE             0x9000
#define KB_CMD_BUF_BASE         0x9008

#define KB_RESET_SUCCESS        0xAA
#define KB_ECHO                 0xEE
#define KB_ACK                  0xFA
#define KB_RESEND               0xFE

#define KB_CMD_SET_LEDS         0xED
#define KB_CMD_ECHO             0xEE
#define KB_CMD_KEYSET           0xF0
#define KB_CMD_IDENTIFY         0xF2
#define KB_CMD_SET_TYPEMATIC    0xF3
#define KB_CMD_ENABLE_SCANNING  0xF4
#define KB_CMD_DISABLE_SCANNING 0xF5
#define KB_CMD_SET_DEFAULTS     0xF6
// 0xF7...0xFD - команды для скансета 3, не используются

#define KB_CMD_RESEND_LAST      0xFE
#define KB_CMD_RESET            0xFF

#define KB_STATUS_SHIFT         1
#define KB_STATUS_CTRL          2
#define KB_STATUS_ALT           4
#define KB_STATUS_CAPSLOCK      8
#define KB_STATUS_NUMLOCK       16

extern const dword maxInputSize;

// Флаг; установлен, если команда воспринята
extern bool cmdACKd;
// Флаг; установлен, если команду нужно отправить повторно
extern bool cmdNeedsResending;
// Флаг; установлен, если после команды последуют данные от клавиатуры
extern bool cmdAwaitingResponse;
// Флаг; установлен, если клавиша была отпущена
extern bool releaseScancode;
// Флаг; установлен, если нужно выводить введённый текст на экран
extern bool printInput;

// Буфер сканкодов
extern byte *kbBufPtr;
// Буфер команд
extern byte *kbCmdBufPtr;
// Статус клавиатуры
extern byte kbStatus;

/// @brief Обновляет клавиатуру в случае нового сканкода.
void updateKB();

/// @brief Инициализирует клавиатуру.
bool initKB();

/// @brief Сдвигает очередь команд клавиатуры.
void shiftKBCmdQueue();
/// @brief Отправляет на клавиатуру следующую команду в очереди.
void sendKBCmd();

/// @brief Добавляет команду в очередь на отправку.
/// @param cmd Команда
bool sendKBCommand(byte cmd);
/// @brief Добавляет команду в очередь на отправку.
/// @param cmd Команда
/// @param arg Аргумент команды
bool sendKBCommand(byte cmd, byte arg);

//==============================================

/// @brief Считывает строку с клавиатуры.
/// @param in Буфер вводимых данных
/// @attention Для буфера всегда должно быть отделено как минимум 0x200 (512) байтов!
void kread(byte* in);

/// @brief Считывает одну клавишу с клавиатуры.
/// @return - Низший байт - код нажатой клавиши
/// @return - Высший байт - статус клавиатуры во время нажатия
word kreadkey();

#endif