//
//  Библиотека для работы с PCI-устройствами
//
//  - Контроллирует работу устройств, подключённых через шину PCI.
//
#include <stdint.h>
#ifndef IO_SIG
#include "../io/io.hpp"
#endif
#ifndef GRAPHICS_SIG
#include "../graphics/glyphs.hpp"
#endif

#define PCI_CONFIG_ADDRESS                  0xCF8
#define PCI_CONFIG_DATA                     0xCFC

#define PCI_CLASS_UNCLASSIFIED              0
#define PCI_CLASS_MASS_STORAGE_DEVICE       1
// ...

#define PCI_SUBCLASS_SCSI_CONTROLLER        0
#define PCI_SUBCLASS_IDE_CONTROLLER         1
#define PCI_SUBCLASS_FLOPPY_CONTROLLER      2
#define PCI_SUBCLASS_IPI_BUS_CONTROLLER     3
#define PCI_SUBCLASS_RAID_CONTROLLER        4
#define PCI_SUBCLASS_ATA_CONTROLLER         5
#define PCI_SUBCLASS_SATA_CONTROLLER        6
#define PCI_SUBCLASS_SER_SCSI_CONTROLLER    7
#define PCU_SUBCLASS_NVM_CONTROLLER         8

/// @brief Считывает двойное слово из конфигурации PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @return Считанное двойное слово
uint32_t pciConfigReadDW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

/// @brief Считывает слово из конфигурации PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @return Считанное двойное слово
uint16_t pciConfigReadW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

/// @brief Считывает байт из конфигурации PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @return Считанный байт
uint8_t  pciConfigReadB(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteDW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteDW(uint32_t shuf, uint8_t offset, uint32_t value);

/// @brief Записывает слово в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteW(uint32_t shuf, uint8_t offset, uint16_t value);

/// @brief Записывает байт в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteB(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteB(uint32_t shuf, uint8_t offset, uint8_t value);

uint32_t pciConfigReadBAR(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar);

uint32_t pciConfigReadBAR(uint32_t shuf, uint8_t bar);

/// @brief Записывает BAR для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param bar Номер BARа
/// @param value Новое значение
void pciConfigWriteBAR(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar, uint32_t value);

/// @brief Записывает BAR для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param bar Номер BARа
/// @param value Новое значение
void pciConfigWriteBAR(uint32_t shuf, uint8_t bar, uint32_t value);

/// @brief Определяет назначение данной функции PCI-устройства.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @return 1 байт - класс устройства, 2 байт - подкласс устройства, 3 байт - интерфейс программирования (если есть)
uint32_t pciIdentifyFunc(uint8_t bus, uint8_t device, uint8_t function);

/// @brief Определяет назначение данной функции PCI-устройства.
/// @param shuf ШУФ-ключ функции
/// @return 1 байт - класс устройства, 2 байт - подкласс устройства, 3 байт - интерфейс программирования (если есть)
uint32_t pciIdentifyFunc(uint32_t shuf);

/// @brief Проверяет, существует ли данная функция устройства PCI.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
bool pciFuncExists(uint8_t bus, uint8_t device, uint8_t function);

/// @brief Проверяет, существует ли данная функция устройства PCI.
/// @param shuf ШУФ-ключ функции
bool pciFuncExists(uint32_t shuf);

uint32_t checkPCIBus(uint8_t bus, uint32_t neededID);

/// @brief Ищет PCI-устройство среди существующих с определённым функционалом.
/// @param devclass Класс устройства
/// @param devsubclass Подкласс устройства
/// @param progIF Интерфейс программирования устройства
/// @return ШУФ-ключ искомого устройства
uint32_t pciFindDevice(uint8_t devclass, uint8_t devsubclass, uint8_t progIF);

/// @brief Ищет PCI-устройство среди существующих с определённым функционалом.
/// @param devclass Класс устройства
/// @param devsubclass Подкласс устройства
/// @return ШУФ-ключ искомого устройства
uint32_t pciFindDevice(uint8_t devclass, uint8_t devsubclass);