#include "disk.hpp"
#ifndef GRAPHICS_SIG
#include "../graphics/glyphs.hpp"
#endif

DiskData identifyDisk() {
    inb(DISK_ATA_STATUS_PRIMARY);
    DiskData res{0,0,0,0};

    outb(DISK_ATA_DRIVE_HEAD_PRIMARY, 0xA0);
    outb(DISK_ATA_SECTOR_COUNT_PRIMARY, 0);
    outb(DISK_ATA_LBA_LOW_PRIMARY, 0);
    outb(DISK_ATA_LBA_MID_PRIMARY, 0);
    outb(DISK_ATA_LBA_HIGH_PRIMARY, 0);
    outb(DISK_ATA_COMMAND_PRIMARY, 0xEC);

    uint8_t status = inb(DISK_ATA_STATUS_PRIMARY);
    if (status == 0) {
        res.DiskType = DISK_TYPE_NONE;
        return res;
    }

    while (inb(DISK_ATA_STATUS_PRIMARY) & DISK_STATUS_BUSY) {
        if (inb(DISK_ATA_LBA_MID_PRIMARY) || inb(DISK_ATA_LBA_HIGH_PRIMARY))
            break;
        io_wait();
    }

    status = inb(DISK_ATA_STATUS_PRIMARY);
    uint8_t id1 = inb(DISK_ATA_LBA_MID_PRIMARY);
    uint8_t id2 = inb(DISK_ATA_LBA_HIGH_PRIMARY);
    if (status & DISK_STATUS_ERROR && id1 == 0 && id2 == 0)
        res.DiskType = DISK_TYPE_ATA;
    else if (id1 == 0 && id2 == 0) {
        while (status & DISK_STATUS_BUSY) {
            status = inb(DISK_ATA_STATUS_PRIMARY);
            if (inb(DISK_ATA_LBA_MID_PRIMARY) || inb(DISK_ATA_LBA_LOW_PRIMARY)) {
                res.DiskType = DISK_TYPE_ATAPI;
                break;
            }
        }
        res.DiskType = DISK_TYPE_ATA;
    }
    else if (id1 == 0x14 && id2 == 0xED)
        res.DiskType = DISK_TYPE_ATAPI;
    else if (id1 == 0x3C && id2 == 0xC3)
        res.DiskType = DISK_TYPE_SATA;
    uint16_t *buf = (uint16_t*)0x9500;
    for (uint16_t i = 0; i < 0x100; i++) {
        *buf = inw(DISK_ATA_DATA_PRIMARY);
        buf ++;
    }
    buf = (uint16_t*)0x9500;
    res.LBA48Supported = buf[83] & 0x400;
    res.TotalLBA28Sectors = (buf[61] << 16) + buf[60];
    res.TotalLBA48Sectors = ((uint64_t)buf[103] << 48) + ((uint64_t)buf[102] << 32) + ((uint64_t)buf[101] << 16) + buf[100];
    return res;
}