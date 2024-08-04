//
//  Таблицы ACPI
//
//  - Структуры, удобно хранящие в себе таблицы ACPI.
//
#include <stdint.h>

// ### RSDP
// Root System Description Pointer - штука, которая, по сути, откроет для нас ACPI.
struct RSDP {
    char signature[8];  // Подпись - "RSD PTR "
    uint8_t checksum;   // Контрольная сумма
    char oemID[6];      // Идентификатор OEM
    uint8_t revision;   // Ревизия ACPI
    uint32_t rsdtAddr;  // Адрес таблицы RSDT
} __attribute__((packed));

// ### Заголовок таблицы ACPI
// Эта структура всегда есть в начале любой таблицы ACPI.
struct ACPITableHeader {
    char signature[4];          // Подпись таблицы
    uint32_t length;            // Длина таблицы
    uint8_t revision;           // Ревизия таблицы
    uint8_t checksum;           // Контрольная сумма таблицы
    char oemID[6];              // Идентификатор OEM
    char oemTableID[8];         // Идентификатор таблицы OEM
    uint32_t oemRevision;       // Ревизия OEM
    uint32_t creatorID;         // Идентификатор создателя
    uint32_t creatorRevision;   // Ревизия создателя
} __attribute__((packed));

// ### GAS
// Generic Address Structure - эта структура описывает положение регистров ACPI.
struct GAS {
    uint8_t addrSpace;
    uint8_t bitWidth;
    uint8_t bitOffset;
    uint8_t accessSize;
    uint64_t addr;
};

// ### FADT
// Fixed ACPI Description Table - эта таблица описывает фиксированные параметры ACPI.
struct FADT {
    ACPITableHeader header;
    uint32_t facsAddr;
    uint32_t dsdtAddr;
    uint8_t reserved;
    uint8_t preferredPMP;
    uint16_t sciInt;
    uint32_t smiCmdPort;
    uint8_t acpiEnable;
    uint8_t acpiDisable;
    uint8_t s4bios_req;
    uint8_t pstateCtrl;
    uint32_t pm1aEventBlock;
    uint32_t pm1bEventBlock;
    uint32_t pm1aCtrlBlock;
    uint32_t pm1bCtrlBlock;
    uint32_t pm2CtrlBlock;
    uint32_t pmTimerBlock;
    uint32_t gpe0block;
    uint32_t gpe1block;
    uint8_t pm1EventLength;
    uint8_t pm1CtrlLength;
    uint8_t pm2CtrlLength;
    uint8_t pmTimerLength;
    uint8_t gpe0length;
    uint8_t gpe1length;
    uint8_t gpe1base;
    uint8_t cStateCtrl;
    uint16_t worstC2Lat;
    uint16_t worstC3Lat;
    uint16_t flushSize;
    uint16_t flushStride;
    uint8_t dutyOffset;
    uint8_t dutyWidth;
    uint8_t dayAlarm;
    uint8_t monthAlarm;
    uint8_t century;

    // ACPI 2.0+

    uint16_t bootArchFlags;
    uint8_t reserved2;
    uint32_t flags;
    GAS resetRegister;
    uint8_t resetValue;
    uint8_t reserved3[3];
    uint64_t x_facsAddr;
    uint64_t x_dsdt;
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
    uint32_t length;            // Длина таблицы
    uint32_t hardwareSig;       // Подпись железа
    uint32_t wakingVector;      // Вектор пробуждения
    uint32_t globalLock;        // Глобальный замок
    uint32_t flags;             // Флаги
    uint64_t xWakingVector;     // Расширенный вектор пробуждения
    uint8_t version;            // Версия
    uint8_t reserved[3];        // (резервировано)
    uint32_t ospmFlags;         // Флаги OSPM
    uint8_t reserved0[24];      // (резервировано)
} __attribute__((packed));

// ### MADT
// Multiple APIC Description Table - эта таблица описывает контроллеры прерываний, доступные ПК.
struct MADT {
    ACPITableHeader header;     // Заголовок (подпись "APIC")
    uint32_t licAddr;           // Адрес локального контроллера прерываний
    uint8_t controllers[];      // Контроллеры прерываний
} __attribute__((packed));

// ### DSDT
// Differentiated System Description Table - эта таблица содержит AML-код.
struct DSDT {
    ACPITableHeader header;
    char aml[];
};

// ### SSDT
// Secondary System Description Table - эта таблица содержит AML-код.
struct SSDT {
    ACPITableHeader header;
    char aml[];
};