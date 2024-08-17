//
//  Библиотека для обработки AML
//
//  - Перерабатывает AML-код.
//
#include <stdint.h>
#ifndef _AML_INCL
#define _AML_INCL

// ### Одиночное имя
// 4 символа, определяющие объект в AML.
typedef unsigned int AMLName;

// ### Двойное имя
// Двойное имя - это два имени. Всё просто.
struct AMLDualName {
    AMLName name1;          // Имя 1
    AMLName name2;          // Имя 2
} __attribute__((packed));

// ### Множественное имя
// Множественное имя - это последовательность имён переменной длины.
struct AMLMultiName {
    uint8_t count;          // Количество имён
    AMLName names[];        // Список имён
} __attribute__((packed));

// ### EISA ID
// EISA-идентификатор устройства.
struct EISAId {
    char mfg[3];            // Идентификатор
    uint8_t prodNo[4];      // Код устройства
    uint8_t reserved = 0;   // (резервирован для сохранения ровнения)
} __attribute__((packed));

/// @brief Обрабатывает Definition Block.
/// @param aml Указатель на AML-код
void parseDefBlock(uint8_t *aml);

uint16_t parseName(uint8_t *aml);

uint32_t parseScope(uint8_t *aml);

uint32_t parseDevice(uint8_t *aml);

uint32_t parseOpRegion(uint8_t *aml);

void parseTermList(uint8_t *aml, uint32_t len);

/// @brief Извлекает длину Package.
/// @param pkg Указатель на начало
/// @return Длина Package, Б
uint32_t getPkgLength(uint8_t *pkg);

/// @brief Извлекает адрес переменной ACPI.
/// @param strPath Путь (NameSegы, разделённые '.')
/// @return Указатель на переменную
uint8_t* getACPIVarAddr(const char* strPath);

#endif