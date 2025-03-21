#include "disk.hpp"
#include "../memmgr/memmgr.hpp"
#include "../io/com.hpp"
#include "../util/util.hpp"
#include "../dbg/dbg.hpp"
#include "../str/str.hpp"

const dword maxDisks = 100;

DiskData *disks = nullptr;

void initDisks() {
    // TODO
    // Вся инфраструктура дисков идёт под снос и
    // переделку, но конкретно сейчас я хочу доделать
    // запуск программ.
    // К этому ещё обязательно вернёмся.

    disks = (DiskData*)kmalloc(sizeof(DiskData) * maxDisks);
    memset(disks, sizeof(DiskData) * maxDisks, 0);

    DiskData firstDisk = {ATA, 0, toUTF16('А'), FAT32, 512, 0, maxqword};
    disks[0] = firstDisk;
}

byte determineDriveNo(char *path) {
    if (*path != '{') return maxbyte;

    path++;
    if (*path >= 0x30 && *path <= 0x39) {
        byte drive = strdectoint(path);
        if (drive >= maxDisks) return maxbyte;
        return drive;
    }
    word id = toUTF16((byte*&)path);
    for (word i = 0; i < maxDisks; i++) {
        if (disks[i].diskIdentifier == id) return i;
    }
    return maxbyte;
}

DiskData identifyDisk(byte diskNo) {
    // TODO
    // Вся инфраструктура дисков идёт под снос и
    // переделку, но конкретно сейчас я хочу доделать
    // запуск программ.
    // К этому ещё обязательно вернёмся.
    return {};
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