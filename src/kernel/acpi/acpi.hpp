//
//  Библиотека для управления ACPI
//
//  - Используется для управления питанием компонентов ПК.
//
#include "../util/nums.hpp"
#include "acpitables.hpp"
#include "aml.hpp"

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

/// @brief Выключает компьютер.
void shutdownPC();