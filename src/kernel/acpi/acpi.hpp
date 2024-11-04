//
//  Библиотека для управления ACPI
//
//  - Используется для управления питанием компонентов ПК.
//
#include "../util/nums.hpp"
#include "acpitables.hpp"
#include "aml.hpp"
#include "sci.hpp"

/// @brief Утверждает контрольную сумму RSDP.
/// @param rsdp RSDP
bool verifyRSDP(RSDP rsdp);

bool verifyHeader(ACPITableHeader header);

/// @brief Ищет RSDP в памяти.
RSDP findRSDP();

/// @brief Инициализирует библиотеку ACPI.
bool initACPI();

/// @brief Активирует определённое состояние сна.
/// @param state Номер состояния
void enterSleepState(byte state);

word readPM1aSts();
void writePM1aSts(word val);
void writePM1aEnable(word val);
word readPM1aCtrl();
void writePM1aCtrl(word val);

word readPM1bSts();
void writePM1bSts(word val);
void writePM1bEnable(word val);
word readPM1bCtrl();
void writePM1bCtrl(word val);

void wake();

// =====================

/// @brief Выключает компьютер.
void kshutdown();

/// @brief Перезагружает компьютер.
void krestart();

/// @brief Переводит компьютер в состояние сна.
void ksleep();