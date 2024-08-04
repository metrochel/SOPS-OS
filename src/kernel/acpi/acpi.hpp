//
//  Библиотека для управления ACPI
//
//  - Используется для управления питанием компонентов ПК.
//
#include <stdint.h>
#include "acpitables.hpp"


/// @brief Утверждает контрольную сумму RSDP.
/// @param rsdp RSDP
bool verifyRSDP(RSDP rsdp);

bool verifyHeader(ACPITableHeader header);

/// @brief Ищет RSDP в памяти.
RSDP findRSDP();

/// @brief Инициализирует библиотеку ACPI.
bool initACPI();