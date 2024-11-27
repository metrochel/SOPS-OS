#include "disk.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"
#include "../util/util.hpp"
#include "../dbg/dbg.hpp"

DiskData identifyDisk() {
    DiskData res{0,0,0,0,0,0};
    kdebug("Получена команда на идентификацию диска.\n");
    outb(ATA_DRIVE_HEAD_PRIMARY, 0xE0);
    byte status = inb(ATA_ALT_STATUS_PRIMARY);
    kdebug("Статус = %x\n", status);
    if (status == 0) {
        kdebug("Диск не подключён.\n");
        res.DiskType = DISK_TYPE_NONE;
        return res;
    }

    status = inb(ATA_ALT_STATUS_PRIMARY);
    byte id1 = inb(ATA_LBA_MID_PRIMARY);
    byte id2 = inb(ATA_LBA_HIGH_PRIMARY);
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
    else if (id1 == 0x14 && id2 == 0xEB) {
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
    else {
        res.DiskType = DISK_TYPE_NONE;
        kdebug("не стандартизован.\n");
        return res;
    }

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

    while (inb(ATA_ALT_STATUS_PRIMARY) & ATA_STATUS_BUSY) {
        if (inb(ATA_LBA_MID_PRIMARY) || inb(ATA_LBA_HIGH_PRIMARY))
            break;
        io_wait();
    }

    word *buf = (word*)0x9908;
    for (word i = 0; i < 0x100; i++) {
        *buf = inw(ATA_DATA_PRIMARY);
        buf ++;
    }
    buf = (word*)0x9908;
    res.LBA48Supported = buf[83] & 0x400;
    kdebug(res.LBA48Supported ? "Диск поддерживает LBA48.\n" : "Диск не поддерживает LBA48.\n");
    res.TotalLBA28Sectors = (buf[61] << 16) + buf[60];
    kdebug("В режиме LBA28 доступно %d секторов.\n", res.TotalLBA28Sectors);
    res.TotalLBA48Sectors = ((qword)buf[103] << 48) + ((qword)buf[102] << 32) + ((qword)buf[101] << 16) + buf[100];
    kdebug("В режиме LBA48 доступно %d секторов.\n", res.TotalLBA48Sectors);

    byte udmaModes = buf[88] & 0xFF;
    byte udmaActive = buf[88] >> 8;
    byte udmaMax = 0;
    while (udmaModes) {
        udmaMax ++;
        udmaModes >>= 1;
    }
    byte udmaAct = 0;
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

void readSectors(byte *buf, dword lba, dword count, byte drive) {
    // TODO: Добавить возможность считывать с разных видов устройств
    // (ATAPI, SATA+AHCI, USB, дискеты и т.д.)
    enableInts();
    readSectorsATA(lba, count, drive, buf);
}

void readSector(byte *buf, dword lba, byte drive) {
    readSectors(buf, lba, 1, drive);
}

void writeSectors(byte *buf, dword lba, dword count, byte drive) {
    // TODO: Добавить возможность записывать в разные типы устройств
    // (ATAPI, SATA+AHCI, USB, дискеты и т.д.)
    enableInts();
    writeSectorsATA(lba, count, drive, buf);
}

void writeSector(byte *buf, dword lba, byte drive) {
    writeSectors(buf, lba, 1, drive);
}