#include "disk.hpp"
#ifndef GRAPHICS_SIG
#include "../graphics/glyphs.hpp"
#endif
#ifndef COM_MAX_BAUD_RATE
#include "../io/com.hpp"
#endif

DiskData identifyDisk() {
    DiskData res{0,0,0,0,0,0};
    kdebug("Получена команда на идентификацию диска.\n");
    outb(ATA_DRIVE_HEAD_PRIMARY, 0xE0);
    kdebug("Регистр выбора диска = %x\n", inb(ATA_DRIVE_HEAD_PRIMARY));
    outb(ATA_SECTOR_COUNT_PRIMARY, 0);
    kdebug("Число секторов = %x\n", inb(ATA_SECTOR_COUNT_PRIMARY));
    outb(ATA_LBA_LOW_PRIMARY, 0);
    kdebug("LBA низший = %x\n", inb(ATA_LBA_LOW_PRIMARY));
    outb(ATA_LBA_MID_PRIMARY, 0);
    kdebug("LBA средний = %x\n", inb(ATA_LBA_MID_PRIMARY));
    outb(ATA_LBA_HIGH_PRIMARY, 0);
    kdebug("LBA высший = %x\n", inb(ATA_LBA_HIGH_PRIMARY));
    outb(ATA_DRIVE_CONTROL_PRIMARY, 2);
    outb(ATA_COMMAND_PRIMARY, 0xEC);
    kdebug("Выслана команда 0xEC.\n");

    uint8_t status = inb(ATA_ALT_STATUS_PRIMARY);
    kdebug("Статус = %x\n", status);
    if (status == 0) {
        kdebug("Диск не подключён.\n");
        res.DiskType = DISK_TYPE_NONE;
        return res;
    }

    while (inb(ATA_ALT_STATUS_PRIMARY) & ATA_STATUS_BUSY) {
        if (inb(ATA_LBA_MID_PRIMARY) || inb(ATA_LBA_HIGH_PRIMARY))
            break;
        io_wait();
    }

    status = inb(ATA_ALT_STATUS_PRIMARY);
    uint8_t id1 = inb(ATA_LBA_MID_PRIMARY);
    uint8_t id2 = inb(ATA_LBA_HIGH_PRIMARY);
    kdebug("Идентификаторы: %x и %x. Тип диска - ", id1, id2);
    if (status & ATA_STATUS_ERROR && id1 == 0 && id2 == 0) {
        res.DiskType = DISK_TYPE_ATA;
        kdebug("ATA.\n");
    }
    else if (id1 == 0 && id2 == 0) {
        while (status & ATA_STATUS_BUSY) {
            status = inb(ATA_ALT_STATUS_PRIMARY);
            if (inb(ATA_LBA_MID_PRIMARY) || inb(ATA_LBA_LOW_PRIMARY)) {
                res.DiskType = DISK_TYPE_ATAPI;
                break;
            }
        }
        res.DiskType = DISK_TYPE_ATA;
        kdebug("ATA.\n");
    }
    else if (id1 == 0x14 && id2 == 0xED) {
        res.DiskType = DISK_TYPE_ATAPI;
        kdebug("ATAPI.\n");
    }
    else if (id1 == 0x3C && id2 == 0xC3) {
        res.DiskType = DISK_TYPE_SATA;
        kdebug("SATA.\n");
    }
    else if (id1 == 0x69 && id2 == 0x96) {
        res.DiskType = DISK_TYPE_SATAPI;
        kdebug("SATAPI.\n");
    }
    uint16_t *buf = (uint16_t*)0x9908;
    for (uint16_t i = 0; i < 0x100; i++) {
        *buf = inw(ATA_DATA_PRIMARY);
        buf ++;
    }
    buf = (uint16_t*)0x9908;
    res.LBA48Supported = buf[83] & 0x400;
    kdebug(res.LBA48Supported ? "Диск поддерживает LBA48.\n" : "Диск не поддерживает LBA48.\n");
    res.TotalLBA28Sectors = (buf[61] << 16) + buf[60];
    kdebug("В режиме LBA28 доступно %d секторов.\n", res.TotalLBA28Sectors);
    res.TotalLBA48Sectors = ((uint64_t)buf[103] << 48) + ((uint64_t)buf[102] << 32) + ((uint64_t)buf[101] << 16) + buf[100];
    kdebug("В режиме LBA48 доступно %d секторов.\n", res.TotalLBA48Sectors);

    uint8_t udmaModes = buf[88] & 0xFF;
    uint8_t udmaActive = buf[88] >> 8;
    uint8_t udmaMax = 0;
    while (udmaModes) {
        udmaMax ++;
        udmaModes >>= 1;
    }
    uint8_t udmaAct = 0;
    while (udmaActive) {
        udmaAct ++;
        udmaActive >>= 1;
    }

    res.MaxUDMAMode = udmaMax;
    res.ActUDMAMode = udmaAct;
    kdebug("Максимальный поддерживаемый режим UDMA - %d, активен %d.\n\n", udmaMax, udmaAct);

    outb(ATA_DRIVE_CONTROL_PRIMARY, 0);
    return res;
}