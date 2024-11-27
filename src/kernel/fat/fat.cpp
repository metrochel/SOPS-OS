#include "fat.hpp"
#include "../io/com.hpp"
#include "../disk/disk.hpp"
#include "../dbg/dbg.hpp"
#include "../util/util.hpp"
#include "../memmgr/memmgr.hpp"
#include "../graphics/glyphs.hpp"
#include "../str/str.hpp"

bool fatInit = false;

FAT_BPB bpbs[16];
FAT32_EBPB ebpbs[16];

bool initFAT(byte driveNo) {
    kdebug("Начата инициализация FAT на диске %d.\n", driveNo);
    kdebug("Считывается загрузочный сектор.\n");
    byte *bootPtr = kmalloc(512);
    readSector(bootPtr, 0, driveNo);
    
    FAT_BPB *bpb = (FAT_BPB*)bootPtr;
    bpbs[driveNo] = *bpb;
    bootPtr += sizeof(FAT_BPB);
    FAT32_EBPB *ebpb = (FAT32_EBPB*)bootPtr;
    ebpbs[driveNo] = *ebpb;

    kdebug("Анализ BPB.\n");
    kdebug("\tИмя идентификатора: \"");
    for (byte i = 0; i < 8; i++)
        kdebug(bpb->oemName[i]);
    kdebug("\"\n");
    kdebug("\tРазмер одного сектора: %d Б\n", bpb->bytesPerSector);
    kdebug("\tРазмер одного кластера: %d секторов\n", bpb->sectorsPerCluster);
    kdebug("\tЧисло резервированных секторов: %d\n", bpb->reservedSectors);
    kdebug("\tКоличество таблиц на диске: %d\n", bpb->fatsCount);
    kdebug("\tЧисло вхождений в корневой каталог: %d\n", bpb->rootEntriesCount);
    kdebug("\tРазмер диска (FAT12/FAT16): %d секторов\n", bpb->oldSectCount);
    kdebug("\tИдентификатор носителя: %x\n", bpb->media);
    kdebug("\tРазмер одной FAT (FAT12/FAT16): %d секторов\n", bpb->oldFATSize);
    kdebug("\tКоличество головок на носителе: %d\n", bpb->headsCount);
    kdebug("\tЧисло скрытых секторов: %d\n", bpb->hiddenSects);
    kdebug("\tРазмер диска (FAT32): %d секторов\n", bpb->totalSects);
    kdebugnewl();

    kdebug("Анализ EBPB.\n");
    kdebug("\tРазмер одной FAT: %d секторов\n", ebpb->fatSize);
    kdebug("\tРасширенные флаги: %b\n", ebpb->extFlags);
    kdebug("\tВерсия FAT32: %d:%d\n", ebpb->fat32Ver >> 8, ebpb->fat32Ver & 0xFF);
    kdebug("\tКластер корневой директории: %d\n", ebpb->rootCluster);
    kdebug("\tСектор структуры FSInfo: %d\n", ebpb->fsInfoSect);
    kdebug("\tСектор запасного загрузчика: %d\n", ebpb->backupBootSect);
    kdebug("\tНомер диска: %x\n", ebpb->driveNo);
    kdebug("\tИдентификатор тома: %x\n", ebpb->driveNo);
    kdebug("\tМетка тома: \"");
    for (byte i = 0; i < 11; i++) {
        kdebug(ebpb->volumeLabel[i]);
    }
    kdebug("\"\n");
    kdebug("\tКонтрольная строка: \"");
    for (byte i = 0; i < 8; i++) {
        kdebug(ebpb->flSysType[i]);
    }
    kdebug("\"\n");
    kdebugnewl();

    if (!memcmp(ebpb->flSysType, (byte*)"FAT32   ", 8)) {
        kdebug("ОШИБКА: Содержание строки flSysType не соответствует \"FAT32   \".\n");
        kdebug("Требуется форматирование тома.\n");
        return false;
    }

    kdebug("Инициализация FAT завершена успешно.\n");
    fatInit = true;

    kfree(bootPtr - sizeof(FAT_BPB));
    return true;
}

void extractLFNName(FAT_LFNEntry *lfn, byte *&out) {
    for (byte i = 0; i < 5; i++) {
        word symbol = lfn->name1[i];
        if (symbol > 0xFF)
            *out++ = symbol >> 8;
        *out++ = symbol & 0xFF;
    }
    for (byte i = 0; i < 6; i++) {
        word symbol = lfn->name2[i];
        if (symbol > 0xFF)
            *out++ = symbol >> 8;
        *out++ = symbol & 0xFF;
    }
    for (byte i = 0; i < 2; i++) {
        word symbol = lfn->name3[i];
        if (symbol > 0xFF)
            *out++ = symbol >> 8;
        *out++ = symbol & 0xFF;
    }
}

void extractShortName(FAT_DirEntry entry, byte *&out) {
    for (byte i = 0; i < 8; i++) {
        if (entry.name[i] == 0x20)
            break;
        *out++ = entry.name[i];
    }
    *out++ = '.';
    for (byte i = 0; i < 3; i++) {
        if (entry.name[i+8] == 0x20)
            break;
        *out++ = entry.name[i+8];
    }
    *out++ = 0x00;
}

File File::construct(byte *ptr, byte driveNo) {
    File f;
    FAT_DirEntry *entries = (FAT_DirEntry*)ptr;
    kdebug("Начата сборка класса файла.\n");
    kdebug("Адрес метки: %x.\n", ptr);
    byte lfnCount = 0;
    while (is_lfn(entries)) {
        lfnCount++;
        entries++;
    }

    kdebug("Количество LFN-меток: %d.\n", lfnCount);

    f.attributes = entries->attr;
    f.startCluster = (dword)entries->clusterHi << 16;
    f.startCluster |= entries->clusterLo;
    f.size = entries->fileSize;

    f.drive = driveNo;

    kdebug("Атрибуты файла: %b.\n", f.attributes);
    kdebug("Номер первого кластера файла: %d.\n", f.startCluster);
    kdebug("Размер файла: %d Б.\n", f.size);

    f.creationDate.seconds = (entries->createTime & 0x1F) * 2;
    f.creationDate.seconds += entries->createTimeMS / 100;
    f.creationDate.minutes = (entries->createTime >> 5) & 0x3F;
    f.creationDate.hours = (entries->createTime >> 11) & 0x1F;
    f.creationDate.day = entries->createDate & 0x1F;
    f.creationDate.month = (entries->createDate >> 5) & 0xF;
    f.creationDate.year = ((entries->createDate >> 9) & 0x7F) + 1980;

    char *tmp = (char*)kmalloc(100);
    kdebug("Время создания: ");
    f.creationDate.asStringFull(tmp);
    kdebug((const char*)tmp);
    kdebug(" (МСК-3).\n");
    kfree(tmp);

    if (!lfnCount) {
        byte *name = kmalloc(12);
        f.name = (char*)name;
        extractShortName(*entries, name);
        return f;
    }

    byte *name = kmalloc(lfnCount * 13);
    f.name = (char*)name;
    entries--;
    FAT_LFNEntry *lfn = (FAT_LFNEntry*)entries;
    do {
        extractLFNName(lfn, name);
        lfn --;
    } while (~lfn->order & 0x40);
    extractLFNName(lfn, name);

    return f;
}

void File::read(byte *out) {
    kdebug("Начато считывание файла ");
    kdebug((const char*)this->name);
    kdebug(".\n");
    kdebug("Первый кластер файла: %d.\n", this->startCluster);
    dword fatFirstSect = bpbs[this->drive].reservedSectors;
    word clustsPerFatSect = (bpbs[this->drive].bytesPerSector / 4);
    dword fatSect = fatFirstSect + this->startCluster / clustsPerFatSect;
    kdebug("Сектор FAT с кластером %d: %d.\n", this->startCluster, fatSect);
    dword *fat = (dword*)kmalloc(bpbs[this->drive].bytesPerSector);
    readSector((byte*)fat, fatSect, this->drive);
    dword curClus = this->startCluster;
    while(curClus < 0xFFFFFF8) {
        dword nextFatMin = (fatSect - fatFirstSect + 1) * clustsPerFatSect;
        if (curClus > nextFatMin) {
            kdebug("Считывается новый сектор FAT.\n");
            fatSect++;
            readSector((byte*)fat, fatSect, this->drive);
        }
        kdebug("Считывается кластер %d.\n", curClus);
        readCluster(this->drive, curClus, out);
        out += clustersize(this->drive);
        curClus = fat[curClus % clustsPerFatSect];
    }
    kdebug("Считывание файла завершено.\n");
}

void readCluster(byte driveNo, dword clusterNo, byte *out) {
    word  fatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;
    byte fatCount = bpbs[driveNo].fatsCount;
    byte clusSize = bpbs[driveNo].sectorsPerCluster;

    kdebug("Сектор FAT: %d.\n", fatSect);
    kdebug("Размер одной FAT: %d секторов.\n", fatSize);
    kdebug("Количество FAT на диске: %d.\n", fatCount);
    kdebug("Размер одного кластера: %d секторов.\n", clusSize);

    dword firstClusSect = fatSect + fatSize * fatCount;
    kdebug("Сектор первого кластера: %d.\n", firstClusSect);
    dword clusSect = firstClusSect + (clusterNo - 2) * clusSize;
    kdebug("Сектор искомого кластера: %d.\n", clusSect);

    readSectors(out, clusSect, clusSize, driveNo);
}