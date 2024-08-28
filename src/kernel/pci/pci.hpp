//
//  Библиотека для работы с PCI-устройствами
//
//  - Контроллирует работу устройств, подключённых через шину PCI.
//

#ifndef _PCI_INCL
#define _PCI_INCL

#include "../util/nums.hpp"
#include "../io/io.hpp"
#include "../graphics/glyphs.hpp"

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
dword pciConfigReadDW(byte bus, byte device, byte function, byte offset);

/// @brief Считывает слово из конфигурации PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @return Считанное двойное слово
word pciConfigReadW(byte bus, byte device, byte function, byte offset);

/// @brief Считывает байт из конфигурации PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @return Считанный байт
byte pciConfigReadB(byte bus, byte device, byte function, byte offset);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteDW(byte bus, byte device, byte function, byte offset, dword value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteDW(dword shuf, byte offset, dword value);

/// @brief Записывает слово в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteW(byte bus, byte device, byte function, byte offset, word value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteW(dword shuf, byte offset, word value);

/// @brief Записывает байт в конфигурацию PCI для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteB(byte bus, byte device, byte function, byte offset, byte value);

/// @brief Записывает двойное слово в конфигурацию PCI для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param offset Сдвиг по конфигурации
/// @param value Новое значение
void pciConfigWriteB(dword shuf, byte offset, byte value);

dword pciConfigReadBAR(byte bus, byte device, byte function, byte bar);

dword pciConfigReadBAR(dword shuf, byte bar);

/// @brief Записывает BAR для данной функции.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @param bar Номер BARа
/// @param value Новое значение
void pciConfigWriteBAR(byte bus, byte device, byte function, byte bar, dword value);

/// @brief Записывает BAR для данной функции.
/// @param shuf ШУФ-ключ функции
/// @param bar Номер BARа
/// @param value Новое значение
void pciConfigWriteBAR(dword shuf, byte bar, dword value);

/// @brief Определяет назначение данной функции PCI-устройства.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
/// @return 1 байт - класс устройства, 2 байт - подкласс устройства, 3 байт - интерфейс программирования (если есть)
dword pciIdentifyFunc(byte bus, byte device, byte function);

/// @brief Определяет назначение данной функции PCI-устройства.
/// @param shuf ШУФ-ключ функции
/// @return 1 байт - класс устройства, 2 байт - подкласс устройства, 3 байт - интерфейс программирования (если есть)
dword pciIdentifyFunc(dword shuf);

/// @brief Проверяет, существует ли данная функция устройства PCI.
/// @param bus Номер шины
/// @param device Номер устройства (слота)
/// @param function Номер функции
bool pciFuncExists(byte bus, byte device, byte function);

/// @brief Проверяет, существует ли данная функция устройства PCI.
/// @param shuf ШУФ-ключ функции
bool pciFuncExists(dword shuf);

dword checkPCIBus(byte bus, dword neededID);

/// @brief Ищет PCI-устройство среди существующих с определённым функционалом.
/// @param devclass Класс устройства
/// @param devsubclass Подкласс устройства
/// @param progIF Интерфейс программирования устройства
/// @return ШУФ-ключ искомого устройства
dword pciFindDevice(byte devclass, byte devsubclass, byte progIF);

/// @brief Ищет PCI-устройство среди существующих с определённым функционалом.
/// @param devclass Класс устройства
/// @param devsubclass Подкласс устройства
/// @return ШУФ-ключ искомого устройства
dword pciFindDevice(byte devclass, byte devsubclass);

#endif