//
//  Таблицы ACPI
//
//  - Структуры, удобно хранящие в себе таблицы ACPI.
//
#include "../util/nums.hpp"

// ### RSDP
// Root System Description Pointer - штука, которая, по сути, откроет для нас ACPI.
struct RSDP {
    char signature[8];  // Подпись - "RSD PTR "
    byte checksum;   // Контрольная сумма
    char oemID[6];      // Идентификатор OEM
    byte revision;   // Ревизия ACPI
    dword rsdtAddr;  // Адрес таблицы RSDT
} __attribute__((packed));

// ### Заголовок таблицы ACPI
// Эта структура всегда есть в начале любой таблицы ACPI.
struct ACPITableHeader {
    char signature[4];          // Подпись таблицы
    dword length;            // Длина таблицы
    byte revision;           // Ревизия таблицы
    byte checksum;           // Контрольная сумма таблицы
    char oemID[6];              // Идентификатор OEM
    char oemTableID[8];         // Идентификатор таблицы OEM
    dword oemRevision;       // Ревизия OEM
    dword creatorID;         // Идентификатор создателя
    dword creatorRevision;   // Ревизия создателя
} __attribute__((packed));

// ### GAS
// Generic Address Structure - эта структура описывает положение регистров ACPI.
struct GAS {
    byte addrSpace;
    byte bitWidth;
    byte bitOffset;
    byte accessSize;
    qword addr;
};

// ### FADT
// Fixed ACPI Description Table - эта таблица описывает фиксированные параметры ACPI.
struct FADT {
    ACPITableHeader header;         // Заголовок (подпись "FACP")
    dword facsAddr;              // Адрес FACS
    dword dsdtAddr;              // Адрес DSDT
    byte reserved;               // (резервировано)
    byte preferredPMP;           // Предпочитаемый режим питания
    word sciInt;                // Номер прерывания SCI
    dword smiCmdPort;            // Порт команды SMI
    byte acpiEnable;             // Значение включения ACPI
    byte acpiDisable;            // Значение выключения ACPI
    byte s4bios_req;             
    byte pstateCtrl;
    dword pm1aEventBlock;
    dword pm1bEventBlock;
    dword pm1aCtrlBlock;
    dword pm1bCtrlBlock;
    dword pm2CtrlBlock;
    dword pmTimerBlock;
    dword gpe0block;
    dword gpe1block;
    byte pm1EventLength;
    byte pm1CtrlLength;
    byte pm2CtrlLength;
    byte pmTimerLength;
    byte gpe0length;
    byte gpe1length;
    byte gpe1base;
    byte cStateCtrl;
    word worstC2Lat;
    word worstC3Lat;
    word flushSize;
    word flushStride;
    byte dutyOffset;
    byte dutyWidth;
    byte dayAlarm;
    byte monthAlarm;
    byte century;

    // ACPI 2.0+

    word bootArchFlags;
    byte reserved2;
    dword flags;
    GAS resetRegister;
    byte resetValue;
    word armBootArchFlags;
    byte minorVer;
    qword x_facsAddr;
    qword x_dsdt;
    GAS x_pm1aEventBlock;
    GAS x_pm1bEventBlock;
    GAS x_pm1aCtrlBlock;
    GAS x_pm1bCtrlBlock;
    GAS x_pm2CtrlBlock;
    GAS x_pmTimerBlock;
    GAS x_gpe0Block;
    GAS x_gpe1Block;
} __attribute__((packed));

// ### FACS
// Firmware ACPI Control Structure - эта структура резервирована прошивкой ПК для ACPI.
struct FACS {
    char signature[4];          // Подпись ("FACS")
    dword length;            // Длина таблицы
    dword hardwareSig;       // Подпись железа
    dword wakingVector;      // Вектор пробуждения
    dword globalLock;        // Глобальный замок
    dword flags;             // Флаги
    qword xWakingVector;     // Расширенный вектор пробуждения
    byte version;            // Версия
    byte reserved[3];        // (резервировано)
    dword ospmFlags;         // Флаги OSPM
    byte reserved0[24];      // (резервировано)
} __attribute__((packed));

// ### MADT
// Multiple APIC Description Table - эта таблица описывает контроллеры прерываний, доступные ПК.
struct MADT {
    ACPITableHeader header;     // Заголовок (подпись "APIC")
    dword licAddr;           // Адрес локального контроллера прерываний
    byte controllers[];      // Контроллеры прерываний
} __attribute__((packed));

// ### DSDT
// Differentiated System Description Table - эта таблица содержит AML-код.
struct DSDT {
    ACPITableHeader header;     // Заголовок (подпись "DSDT")
    char aml[];                 // AML-код
} __attribute__((packed));

// ### SSDT
// Secondary System Description Table - эта таблица содержит AML-код.
struct SSDT {
    ACPITableHeader header;     // Заголовок (подпись "SSDT")
    char aml[];                 // AML-код
} __attribute__((packed));

// ### HPETT
// High Precision Event Timer Table - эта таблица описывает HPET, установленный в системе.
struct HPETT {
    ACPITableHeader header;         // Заголовок (подпись "HPET")
    dword eventTimerBlock;       // (TODO: уточнить)
    GAS baseAddr;                   // Основа адреса HPETа
    byte hpetNo;                 // Номер HPETа
    word minTicksPeriodic;      // Минимальная тактовая частота, с которой периодический режим будет работать без перебоев
    byte pageProtAndOemAttr;     // Страничная защита и атрибуты OEM
} __attribute__((packed));

// ### WAET
// Windows ACPI Emulated Devices Table - таблица, говорящая ОС Windows (и, получается, нам), что ОС является гостевой.
struct WAET {
    ACPITableHeader header;         // Заголовок (подпись "WAET")
    dword emDevFlags;            // Флаги эмулируемых устройств
} __attribute__((packed));