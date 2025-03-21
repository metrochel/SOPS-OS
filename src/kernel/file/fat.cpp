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
dword nextFreeClusters[16];

bool initFAT(byte driveNo) {
    kdebug("Начата инициализация FAT на диске %d.\n", driveNo);
    kdebug("Считывается загрузочный сектор.\n");
    byte *ptr = kmalloc(512);
    readSector(ptr, 0, driveNo);
    
    FAT_BPB *bpb = (FAT_BPB*)ptr;
    bpbs[driveNo] = *bpb;
    ptr += sizeof(FAT_BPB);
    FAT32_EBPB *ebpb = (FAT32_EBPB*)ptr;
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

    dword sectorCount;
    if (bpb->totalSects) sectorCount = bpb->totalSects;
    else sectorCount = bpb->oldSectCount;
    ptr -= sizeof(FAT_BPB);

    readSector(ptr, ebpb->fsInfoSect, driveNo);
    FAT_FSInfo *fsinfo = (FAT_FSInfo*)ptr;
    nextFreeClusters[driveNo] = fsinfo->nextFreeClus;
    if (nextFreeClusters[driveNo] == maxdword || nextFreeClusters[driveNo] < sectorCount)
        nextFreeClusters[driveNo] = 2;
    if (getCluster(driveNo, ebpbs[driveNo].rootCluster) == 0)
        setCluster(driveNo, ebpbs[driveNo].rootCluster, FAT_CLUSTER_EOF);
    kdebug("Следующий свободный кластер: %d.\n", nextFreeClusters[driveNo]);

    kdebug("Инициализация FAT завершена успешно.\n");
    fatInit = true;

    kfree(ptr);
    return true;
}

void extractLFNName(FAT_LFNEntry *lfn, byte *out) {
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
    if (entry.name[8] == ' ' && entry.name[9] == ' ' && entry.name[10] == ' ') {
        *out++ = 0x00;
        return;
    }
    *out++ = '.';
    for (byte i = 0; i < 3; i++) {
        if (entry.name[i+8] == 0x20)
            break;
        *out++ = entry.name[i+8];
    }
    *out++ = 0x00;
}

bool FAT32_File::operator!() {
    return name == nullptr;
}

FAT32_File::FAT32_File() {

}

FAT32_File::~FAT32_File() {
    if (this->name) kfree(this->name);
}

FAT32_File::FAT32_File(dword cluster, word offset, byte driveNo) {
    if (!fatInit) {
        name = nullptr;
        return;
    };

    FAT_DirEntry clusterBuf[clustersize(driveNo) / sizeof(FAT_DirEntry)];
    this->drive = driveNo;
    readCluster(drive, cluster, (byte*)clusterBuf);

    FAT_DirEntry *entries = clusterBuf + offset;

    byte lfnCount = 0;
    while (is_lfn(*entries)) {
        lfnCount++;
        entries++;
    }

    kdebug("Количество LFN-меток: %d.\n", lfnCount);

    this->attributes = entries->attr;
    this->startCluster = (dword)entries->clusterHi << 16;
    this->startCluster |= entries->clusterLo;
    this->size = entries->fileSize;

    kdebug("Атрибуты файла: %b.\n", this->attributes);
    kdebug("Номер первого кластера файла: %d.\n", this->startCluster);
    kdebug("Размер файла: %d Б.\n", this->size);

    this->creationDate.seconds = (entries->createTime & 0x1F) * 2;
    this->creationDate.seconds += entries->createTimeMS / 100;
    this->creationDate.minutes = (entries->createTime >> 5) & 0x3F;
    this->creationDate.hours = (entries->createTime >> 11) & 0x1F;
    this->creationDate.day = entries->createDate & 0x1F;
    this->creationDate.month = (entries->createDate >> 5) & 0xF;
    this->creationDate.year = ((entries->createDate >> 9) & 0x7F) + 1980;

    char *tmp = (char*)kmalloc(100);
    kdebug("Время создания: ");
    this->creationDate.asStringFull(tmp);
    kdebug((const char*)tmp);
    kdebug(" (МСК-3).\n");
    kfree(tmp);

    if (!lfnCount) {
        byte *name = kmalloc(12);
        this->name = (char*)name;
        extractShortName(*entries, name);
        this->directoryCluster = cluster;
        this->dirEntryOffset = offset + lfnCount;
        return;
    }

    byte *name = kmalloc(lfnCount * 13);
    this->name = (char*)name;
    entries--;
    FAT_LFNEntry *lfn = (FAT_LFNEntry*)entries;
    while (~lfn->order & 0x40) {
        extractLFNName(lfn, name);
        name += 13;
        lfn --;
    }
    extractLFNName(lfn, name);
    name += 13;

    this->directoryCluster = cluster;
    this->dirEntryOffset = offset + lfnCount;
}

FAT32_File::FAT32_File(char *name, byte attr, byte drive, dword size, Time creationDate, dword directoryCluster) {
    char *fname = (char*)kmalloc(strlen(name) + 1);
    strcpy(name, fname);
    this->name = fname;
    this->attributes = attr;
    this->size = size;
    this->creationDate = creationDate;
    this->lastEditDate = creationDate;
    this->startCluster = maxdword;
    this->directoryCluster = directoryCluster;
    this->drive = drive;
    this->dirEntryOffset  = maxword;
}

FAT32_File::FAT32_File(char *path, byte driveNo, bool forceFile, bool forceFolders) {
    kdebug("Начато создание File по пути ");
    kdebug(path);
    kdebug(".\n");
    drive = driveNo;

    char **pathComponents = strsplit(path, "/");
    char *strs = *pathComponents;
    directoryCluster = root(drive);

    FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];
    byte *nameBuf = kmalloc(13*64);
    memset(nameBuf, sizeof(nameBuf), 0);

    char **_pathComponents = pathComponents;
    bool created = false;

    while (*pathComponents) {
        char *ipath = *pathComponents++;
        if (*ipath == 0) continue;

        kdebug("Поиск элемента \"");
        kdebug(ipath);
        kdebug("\".\n");

        dword dirStart = directoryCluster;
        dword _clus = getCluster(drive, directoryCluster);
        bool found = false;
        while (!is_eof(directoryCluster)) {
            readCluster(drive, directoryCluster, (byte*)clusterBuf);
            for (word i = 0; i < clustersize(drive) / sizeof(FAT_DirEntry); i++) {
                if (clusterBuf[i].name[0] == 0)
                    break;
                if (clusterBuf[i].name[0] == 0xE5)
                    continue;
                if (!is_lfn(clusterBuf[i])) {
                    kdebug("Найдена главная метка файла.\n");
                    FAT_LFNEntry *ptr = (FAT_LFNEntry*)(clusterBuf + i - 1);
                    byte *_nameBuf = nameBuf;
                    if (!is_lfn(*ptr)) continue;
                    while (~ptr->order & 0x40) {
                        extractLFNName(ptr, _nameBuf);
                        _nameBuf += 13;
                        ptr --;
                    }
                    extractLFNName(ptr, _nameBuf);
                    _nameBuf += 13;
                    kdebug("Адрес первой LFN-метки: %x (%d).\n", ptr, i);
                    kdebug("Извлечённое имя: \"");
                    kdebug((char*)nameBuf);
                    kdebug("\".\n");
                    if (strcmp((char*)nameBuf, ipath)) {
                        kdebug("Найдена нужная метка.\n");
                        kdebug("Положение метки: кластер %d, сдвиг %d.\n", directoryCluster, i);
                        dword clus = (clusterBuf[i].clusterHi << 16) | clusterBuf[i].clusterLo;
                        if (!*pathComponents) startCluster = clus;
                        else directoryCluster = clus;
                        dirEntryOffset = i;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
            directoryCluster = _clus;
            _clus = getCluster(drive, _clus);
        }

        if (found)
            continue;
        
        kdebug("ВНИМАНИЕ: Элемент \"");
        kdebug(ipath);
        kdebug("\" не найден\n");

        if (!*pathComponents && !forceFile || *pathComponents && !forceFolders) {
            kdebug("ОШИБКА: Создание File провалено\n");
            directoryCluster = dirStart;
            dirEntryOffset = maxword;
            name = nullptr;
            kfree(nameBuf);
            kfree(_pathComponents);
            return;
        }

        if (!*pathComponents && forceFile || *pathComponents && forceFolders) {
            kdebug("Элемент ");
            kdebug(ipath);
            kdebug(" создаётся.\n");
            FAT32_File element(ipath, !*pathComponents ? 0 : FAT_FILEATTR_DIRECTORY, driveNo, 0, kgettime(), dirStart);
            element.create();
            if (!*pathComponents) {
                startCluster = element.startCluster;
                directoryCluster = element.directoryCluster;
                dirEntryOffset = element.dirEntryOffset;
                creationDate = kgettime();
                lastEditDate = kgettime();
                size = 0;
                attributes = 0;
                created = true;
                break;
            }
        }

        if (!*pathComponents) {
            directoryCluster = dirStart;
            dirEntryOffset = maxword;
            startCluster = maxdword;
            dword namelen = strlen(*--pathComponents);
            char *fname = (char*)kmalloc(namelen + 1);
            attributes = 0;
            strcpy(*pathComponents, fname);
            name = fname;
            kfree(_pathComponents);
            kfree(nameBuf);
            return;
        }
    }

    kdebug("Извлекаются данные файла.\n");
    FAT_DirEntry sfn = clusterBuf[dirEntryOffset];

    if (!created) {
        attributes = sfn.attr;
        size = sfn.fileSize;

        creationDate.day = sfn.createDate & 0x1F;
        creationDate.month = (sfn.createDate >> 5) & 0x0F;
        creationDate.year = (sfn.createDate >> 9) + 1980;

        creationDate.seconds = (sfn.createTime & 0x1F)*2 + sfn.createTimeMS / 100;
        creationDate.minutes = (sfn.createTime >> 5) & 0x3F;
        creationDate.hours = (sfn.createTime >> 11);

        lastEditDate.day = sfn.lastWriteDate & 0x1F;
        lastEditDate.month = (sfn.lastWriteDate >> 5) & 0x0F;
        lastEditDate.year = (sfn.lastWriteDate >> 9) + 1980;

        lastEditDate.seconds = (sfn.lastWriteTime & 0x1F)*2;
        lastEditDate.minutes = (sfn.lastWriteTime >> 5) & 0x3F;
        lastEditDate.hours = (sfn.lastWriteTime >> 11);

        startCluster = (sfn.clusterHi << 16) | sfn.clusterLo;
    }

    dword namelen = strlen(*--pathComponents);
    char *fname = (char*)kmalloc(namelen + 1);
    strcpy(*pathComponents, fname);
    name = fname;

    kdebug("Имя файла: \"");
    kdebug(fname);
    kdebug("\".\n");
    kdebug("Аттрибуты файла: %b.\n", attributes);
    kdebug("Размер файла: %d Б.\n", size);
    kdebug("Первый кластер файла: %d.\n", startCluster);
    kdebug("Положение метки файла: кластер %d, сдвиг %d.\n", directoryCluster, dirEntryOffset);

    char *tmp = (char*)nameBuf;
    creationDate.asStringFull(tmp);
    kdebug("Дата создания: ");
    kdebug(tmp);
    kdebug(".\n");
    lastEditDate.asStringFull(tmp);
    kdebug("Дата последнего редактирования: ");
    kdebug(tmp);
    kdebug(".\n");

    kfree(_pathComponents);
    kfree(strs);
    kfree(nameBuf);
}

bool FAT32_File::read(byte *out) {
    if (!fatInit) return false;
    if (!*this) return false;
    kdebug("Начато считывание файла ");
    kdebug((const char*)this->name);
    kdebug(".\n");
    kdebug("Первый кластер файла: %d.\n", this->startCluster);
    dword clus = startCluster;
    dword _clus = getCluster(drive, clus);
    while (!is_eof(clus)) {
        readCluster(drive, clus, out);
        out += clustersize(drive);
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }
    kdebug("Считывание файла завершено.\n");
    return true;
}

bool FAT32_File::read(dword start, dword size, byte *out) {
    if (!fatInit) return false;
    if (!*this) return false;
    kdebug("Начато считывание куска из файла ");
    kdebug(this->name);
    kdebug(" с %d байта размером %d Б.\n", start, size);
    if (start + size > this->size) {
        kdebug("ОШИБКА: Считываемый фрагмент не полностью существует в файле\n");
        return false;
    }
    dword skippedClusters = start / clustersize(this->drive);
    dword clus = startCluster;
    dword _clus = getCluster(drive, clus);
    kdebug("Пропускается %d кластеров.\n", skippedClusters);
    while (!is_eof(clus) && skippedClusters) {
        clus = _clus;
        _clus = getCluster(drive, _clus);
        skippedClusters --;
    }

    if (skippedClusters) return false;

    kdebug("Чтение начинается с кластера %d.\n", clus);
    kdebug("%d\n", drive);
    byte clusterBuf[clustersize(this->drive)];
    memset(clusterBuf, clustersize(this->drive), 0);
    readCluster(this->drive, clus, clusterBuf);
    dword clusStartSize = clustersize(this->drive) - (start % clustersize(this->drive));
    if (clusStartSize >= size) {
        memcpy(clusterBuf + start % clustersize(this->drive), out, size);
        return true;
    }
    memcpy(clusterBuf + (start % clustersize(this->drive)), out, clusStartSize);
    out += clusStartSize;
    dword _size = size - clusStartSize;
    clus = _clus;
    _clus = getCluster(this->drive, _clus);
    while (!is_eof(clus) && _size > clustersize(drive)) {
        kdebug("Считывается кластер %d.\n", clus);
        readCluster(drive, clus, out);
        out += clustersize(drive);
        _size -= clustersize(drive);
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }
    if (is_eof(clus)) return false;
    readCluster(this->drive, clus, clusterBuf);
    memcpy(clusterBuf, out, _size);
    return true;
}

bool FAT32_File::write(byte *in, dword dataSize) {
    if (!fatInit) return false;
    if (!*this) return false;
    kdebug("Начата запись %d байтов данных в файл ", dataSize);
    kdebug(name);
    kdebug(".\n");
    dword curClusters = getClusterChainLength(startCluster, drive);
    dword newClusters = (size + dataSize + clustersize(drive) - 1) / clustersize(drive);
    kdebug("Сейчас использовано %d кластеров.\n", curClusters);
    kdebug("Необходимо %d кластеров.\n", newClusters);

    dword clus = startCluster;
    dword _clus = getCluster(drive, clus);
    while (!is_eof(_clus)) {
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }
    kdebug("Последний кластер файла: %d.\n", clus);
    if (newClusters > curClusters) {
        kdebug("Необходимо добавить кластеры.\n", newClusters - curClusters);
        dword previousClus = clus;
        while (curClusters * clustersize(drive) < size + dataSize) {
            _clus = allocateCluster(drive);
            setCluster(drive, previousClus, _clus);
            setCluster(drive, _clus, FAT_CLUSTER_EOF);
            previousClus = _clus;
            curClusters ++;
        }
    }

    byte clusterBuf[clustersize(drive)];
    dword _size = dataSize;

    readCluster(drive, clus, clusterBuf);
    kdebug("Считывается кластер %d.\n", clus);
    word offset = size % clustersize(drive);
    word fillSize = clustersize(drive) - offset;
    if (_size < fillSize) {
        kdebug("Прописывается %d Б.\n", _size);
        memcpy(in, clusterBuf + offset, _size);
        writeCluster(drive, clus, clusterBuf);
        kdebug("Запись успешно завершена.\n");
        size += _size;
        _size = 0;
        updateDirEntry(this);
        return true;
    }
    kdebug("Прописывается %d Б.\n", fillSize);
    memcpy(in, clusterBuf + offset, fillSize);
    _size -= fillSize;
    in += fillSize;
    writeCluster(drive, clus, clusterBuf);
    clus = getCluster(drive, clus);

    while (_size > clustersize(drive)) {
        kdebug("Прописывается кластер %d.\n", clus);
        kdebug("Прописывается %d Б.\n", clustersize(drive));
        memcpy(in, clusterBuf, clustersize(drive));
        writeCluster(drive, clus, clusterBuf);
        clus = getCluster(drive, clus);
        in += clustersize(drive);
        _size -= clustersize(drive);
    }

    kdebug("Прописывается кластер %d.\n", clus);
    kdebug("Прописывается %d Б.\n", _size);
    memset(clusterBuf, clustersize(drive), 0);
    memcpy(in, clusterBuf, _size);
    writeCluster(drive, clus, clusterBuf);

    size += dataSize;
    lastEditDate = kgettime();
    updateDirEntry(this);

    kdebug("Запись успешно завершена.\n");
    return true;
}

void FAT32_File::clear() {
    if (!*this) return;
    kdebug("Получен запрос на очистку файла ");
    kdebug(name);
    kdebug(".\n");
    dword clus = startCluster;
    dword _clus = getCluster(drive, clus);
    setCluster(drive, clus, FAT_CLUSTER_EOF);
    kdebug("Кластер %d теперь помечен как последний.\n");
    clus = _clus;
    _clus = getCluster(drive, _clus);
    while (!is_eof(clus)) {
        kdebug("Освобождается кластер %d.\n", clus);
        setCluster(drive, clus, 0);
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }
    size = 0;
    updateDirEntry(this);
}

bool dirNameExists(byte *name, dword dirCluster, byte drive) {
    FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];

    while (!is_eof(dirCluster)) {
        readCluster(drive, dirCluster, (byte*)clusterBuf);
        bool stop = false;
        for (word i = 0; i < sizeof(clusterBuf) / sizeof(FAT_DirEntry); i++) {
            if (memcmp(clusterBuf[i].name, name, 11))
                return true;
            if (clusterBuf[i].name[0] == 0) {
                stop = true;
                break;
            }
        }
        if (stop) break;
        dirCluster = getCluster(drive, dirCluster);
    }
    return false;
}

bool createSFNName(char *name, char *out) {
    byte i = 0;
    bool hasExtension = false;
    while (*name) {
        if (i == 11) {
            out[9] = '~';
            out[10] = '1';
            break;
        }

        byte c = *(byte*)name++;
        if (c > 0x80) {
            c = '_';
        }
        if (c == '\'')
            c = '_';
        if (c >= 0x61) c -= 0x20;
        if (c == '.') {
            hasExtension = true;
            while (i < 8)
                out[i++] = ' ';
            continue;
        }
        out[i] = c;
        i++;
    }
    while (*name && !hasExtension) {
        if (*name == '.') {
            name ++;
            i = 8;
            while (*name && i < 11) {
                byte c = *name++;
                if (c == '\'')
                    c = '_';
                if (c >= 0x61) c -= 0x20;
                out[i++] = c;
            }
            hasExtension = true;
        }
        name ++;
    }
    while (i < 11) {
        out[i] = ' ';
        i++;
    }
    return hasExtension;
}

FAT_DirEntry craftSFN(FAT32_File *f) {
    FAT_DirEntry sfn;

    bool hasExtension = createSFNName(f->name, (char*)sfn.name);

    dword n = 0;
    while (dirNameExists(sfn.name, f->directoryCluster, f->drive)) {
        n ++;
        byte i = hasExtension ? 7 : 10;
        dword _n = n;
        while (_n) {
            sfn.name[i--] = (_n % 10) + 0x30;
            _n /= 10;
        }
        sfn.name[i] = '~';
    }

    sfn.attr = f->attributes;
    sfn.clusterHi = f->startCluster >> 16;
    sfn.clusterLo = f->startCluster & 0xFFFF;
    sfn.fileSize = f->size;
    sfn.createDate = ((f->creationDate.year - 1980) << 9) | ((f->creationDate.month) << 5) | (f->creationDate.day);
    sfn.createTime = ((f->creationDate.hours) << 11) | ((f->creationDate.minutes) << 5) | (f->creationDate.seconds >> 1);
    sfn.createTimeMS = (f->creationDate.seconds % 2) * 100;
    sfn.lastWriteDate = sfn.createDate;
    sfn.lastWriteTime = sfn.createTime;
    sfn.lastAccessDate = sfn.createDate;

    if (f->attributes & FAT_FILEATTR_DIRECTORY)
        sfn.fileSize = 0;

    return sfn;
}

word computeDirEntries(char *str) {
    dword len = strlen(str);
    return (len + 12 - 1) / 13;
}

byte computeChecksum(FAT_DirEntry sfn) {
    byte sum = 0;
    for (byte i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0x00) + (sum >> 1) + sfn.name[i];
    }
    return sum;
}

void updateDirEntry(FAT32_File *f) {
    FAT_DirEntry clusterBuf[clustersize(f->drive) / sizeof(FAT_DirEntry)];

    readCluster(f->drive, f->directoryCluster, (byte*)clusterBuf);
    word curIndex = f->dirEntryOffset;
    freeEntry(clusterBuf[curIndex--]);
    while ((directconv(clusterBuf[curIndex], FAT_LFNEntry).order & 0x40) == 0)
        freeEntry(clusterBuf[curIndex--]);
    freeEntry(clusterBuf[curIndex]);
    writeCluster(f->drive, f->directoryCluster, (byte*)clusterBuf);

    (*f).create();
}

void FAT32_File::create() {
    if (!*this) return;
    FAT_DirEntry clusterBuf[clustersize(this->drive) / sizeof(FAT_DirEntry)];

    this->creationDate = kgettime();

    word entriesNeeded = computeDirEntries(this->name);
    kdebug("Нужно %d меток.\n", entriesNeeded);

    word index = maxword;
    dword clus = this->directoryCluster;
    kdebug("Кластер директории: %d.\n", clus);
    do {
        readCluster(this->drive, clus, (byte*)clusterBuf);
        word streak = 0;
        for (word i = 0; i < clustersize(this->drive) / sizeof(FAT_DirEntry); i++) {
            byte entryFirstChar = clusterBuf[i].name[0];
            if (streak == entriesNeeded + 1) {
                index = i - 1;
                kdebug("Найдено место под метку. Индекс в буфере: %d.\n", index);
                break;
            }
            if (entryFirstChar == 0x00 && i + entriesNeeded < sizeof(clusterBuf) / sizeof(FAT_DirEntry)) {
                index = i + entriesNeeded - streak;
                kdebug("Найдено место под метку. Индекс в буфере: %d.\n", index);
                break;
            }
            if (entryFirstChar == 0xE5) {
                streak ++;
            } else
                streak = 0;
        }
        if (index != maxword) break;
        clus = getCluster(this->drive, clus);
    } while (!is_eof(clus));

    if (index == maxword) {
        kdebug("ОШИБКА: Не хватило места под метку.\n");
        return;
    }

    if (attributes & FAT_FILEATTR_VOLUME_ID)
        startCluster = 0;

    if (this->startCluster == maxdword) {
        dword clustersCount = (this->size + clustersize(this->drive) - 1) / (clustersize(this->drive));
        if (!clustersCount) clustersCount = 1;
        kdebug("Необходимо выделить %d кластеров под файл.\n");
        kdebug("Выделяются кластеры: ");
        dword firstClus = allocateCluster(drive);
        this->startCluster = firstClus;
        setCluster(drive, firstClus, 1);
        kdebug("%d", firstClus);
        dword clus = firstClus;
        for (dword i = 0; i < clustersCount - 1; i++) {
            clus = allocateCluster(drive);
            setCluster(drive, firstClus, clus);
            setCluster(drive, clus, 1);
            kdebug(", %d", clus);
            firstClus = clus;
        }
        kdebug(".\n");
        setCluster(drive, clus, FAT_CLUSTER_EOF);

        nextFreeClusters[drive] = allocateCluster(drive);
    }

    FAT_DirEntry sfn = craftSFN(this);
    clusterBuf[index] = sfn;
    index --;

    byte checksum = computeChecksum(sfn);
    byte *str = (byte*)this->name;
    for (byte i = 1; i <= entriesNeeded; i++) {
        FAT_LFNEntry lfn;
        lfn.chksum = checksum;
        lfn.order = i;
        if (i == entriesNeeded) lfn.order |= 0x40;
        lfn.attr = FAT_FILEATTR_LFN;
        lfn.type = 0;
        lfn._reserved = 0;

        byte j = 0;
        while (*str && j < 5) {
            word bigChar = *str++;
            lfn.name1[j++] = bigChar;
        }
        if (j < 5) {
            lfn.name1[j++] = 0x00;
            while (j < 5) lfn.name1[j++] = maxword;
        }

        j = 0;
        while (*str && j < 6) {
            word bigChar = *str++;
            lfn.name2[j++] = bigChar;
        }
        if (j < 6) {
            if (lfn.name1[4] != maxword) lfn.name2[j++] = 0x00;
            while (j < 6) lfn.name2[j++] = maxword;
        }

        j = 0;
        while (*str && j < 2) {
            word bigChar = *str++;
            lfn.name3[j++] = bigChar;
        }
        if (j < 2) {
            if (lfn.name2[5] != maxword) lfn.name3[j++] = 0x00;
            while (j < 2) lfn.name3[j++] = maxword;
        }

        clusterBuf[index] = directconv(lfn, FAT_DirEntry);
        index --;
    }

    kdebug("Метки расставлены. Начинается запись.\n");
    writeCluster(this->drive, clus, (byte*)clusterBuf);

    this->directoryCluster = clus;
    this->dirEntryOffset = index + entriesNeeded + 1;

    if (attributes & FAT_FILEATTR_DIRECTORY) {
        kdebug("Файл является папкой.\n");
        kdebug("Первый кластер папки: %d.\n", startCluster);
        kdebug("Создаётся метка \".\".\n");
        FAT_DirEntry dotEntry;
        for (byte i = 0; i < 11; i++)
            dotEntry.name[i] = ' ';
        dotEntry.name[0]        = '.';
        dotEntry.attr           = FAT_FILEATTR_DIRECTORY;
        dotEntry.createTimeMS   = sfn.createTimeMS;
        dotEntry.createTime     = sfn.createTime;
        dotEntry.createDate     = sfn.createDate;
        dotEntry.lastAccessDate = sfn.lastAccessDate;
        dotEntry.clusterHi      = sfn.clusterHi;
        dotEntry.lastWriteTime  = sfn.lastWriteTime;
        dotEntry.lastWriteDate  = sfn.lastWriteDate;
        dotEntry.clusterLo      = sfn.clusterLo;
        dotEntry.fileSize       = 0;
        
        kdebug("Создаётся метка \"..\".\n");
        FAT_DirEntry dotdotEntry;
        for (byte i = 0; i < 11; i++)
            dotdotEntry.name[i] = ' ';
        dotdotEntry.name[0]        = '.';
        dotdotEntry.name[1]        = '.';
        dotdotEntry.attr           = FAT_FILEATTR_DIRECTORY;
        dotdotEntry.createTimeMS   = sfn.createTimeMS;
        dotdotEntry.createTime     = sfn.createTime;
        dotdotEntry.createDate     = sfn.createDate;
        dotdotEntry.lastAccessDate = sfn.lastAccessDate;
        dotdotEntry.clusterHi      = directoryCluster >> 16;
        dotdotEntry.lastWriteTime  = sfn.lastWriteTime;
        dotdotEntry.lastWriteDate  = sfn.lastWriteDate;
        dotdotEntry.clusterLo      = directoryCluster & maxword;
        dotdotEntry.fileSize       = 0;

        if (directoryCluster == root(drive)) {
            dotdotEntry.clusterHi = 0;
            dotdotEntry.clusterLo = 0;
        }

        kdebug("Начинается запись \".\" и \"..\" в начало папки.\n");
        memset(clusterBuf, clustersize(drive), 0);
        clusterBuf[0] = dotEntry;
        clusterBuf[1] = dotdotEntry;
        writeCluster(drive, startCluster, (byte*)clusterBuf);
    }
    kdebug("Создание файла завершено.\n");
}

void FAT32_File::remove() {
    FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];

    readCluster(drive, directoryCluster, (byte*)clusterBuf);
    word offset = dirEntryOffset;
    freeEntry(clusterBuf[offset]);
    offset --;
    while ((directconv(clusterBuf[offset], FAT_LFNEntry).order & 0x40) == 0) {
        freeEntry(clusterBuf[offset]);
        offset --;
    }
    freeEntry(clusterBuf[offset]);
    writeCluster(drive, directoryCluster, (byte*)clusterBuf);

    dword clus = startCluster;
    dword _clus = getCluster(drive, clus);
    while (!is_eof(_clus)) {
        setCluster(drive, clus, FAT_CLUSTER_FREE);
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }
    setCluster(drive, clus, FAT_CLUSTER_FREE);
}

void FAT32_File::rename(char *newname) {
    this->name = newname;
    updateDirEntry(this);
}

void readCluster(byte driveNo, dword clusterNo, byte *out) {
    word  fatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;
    byte fatCount = bpbs[driveNo].fatsCount;
    byte clusSize = bpbs[driveNo].sectorsPerCluster;

    dword firstClusSect = fatSect + fatSize * fatCount;
    dword clusSect = firstClusSect + (clusterNo - 2) * clusSize;

    readSectors(out, clusSect, clusSize, driveNo);
}

void writeCluster(byte driveNo, dword clusterNo, byte *in) {
    word  fatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;
    byte fatCount = bpbs[driveNo].fatsCount;
    byte clusSize = bpbs[driveNo].sectorsPerCluster;

    dword firstClusSect = fatSect + fatSize * fatCount;
    dword clusSect = firstClusSect + (clusterNo - 2) * clusSize;

    writeSectors(in, clusSect, clusSize, driveNo);
}

dword allocateCluster(byte driveNo) {
    dword startFatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;

    // dword clus = (nextFreeClusters[driveNo] / 128) * 128;
    dword clus = 0;
    for (dword fatSect = startFatSect + clus / 128; fatSect < startFatSect + fatSize; fatSect++) {
        dword clusters[128];
        readSector((byte*)clusters, fatSect, driveNo);
        for (byte i = 0; i < 128; i++) {
            if (clusters[i] == 0 && (clus + i >= 2))
                return clus + i;
        }
        clus += 128;
    }
    return maxdword;
}

dword getCluster(byte driveNo, dword clusterNo) {
    if (is_eof(clusterNo))
        return 0;

    clusterNo &= 0x0FFFFFFF;
    dword startFatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;
    dword fatSect = startFatSect + clusterNo / 128;
    if (fatSect - startFatSect > fatSize) return FAT_CLUSTER_BAD;

    dword buf[128];
    byte fatCount = bpbs[driveNo].fatsCount;
    for (byte i = 0; i < fatCount; i++) {
        readSector((byte*)buf, fatSect, driveNo);
        dword val = buf[clusterNo % 128];
        if (val != FAT_CLUSTER_BAD)
            return val;
        fatSect += fatSize;
    }
    return FAT_CLUSTER_BAD;
}

void setCluster(byte driveNo, dword clusterNo, dword newVal) {
    clusterNo &= 0x0FFFFFFF;
    dword startFatSect = bpbs[driveNo].reservedSectors;
    dword fatSize = ebpbs[driveNo].fatSize;
    dword fatSect = startFatSect + clusterNo / 128;
    if (fatSect - startFatSect > fatSize) return;

    dword buf[128];
    byte fatCount = bpbs[driveNo].fatsCount;
    for (byte i = 0; i < fatCount; i++) {
        readSector((byte*)buf, fatSect, driveNo);
        buf[clusterNo % 128] = newVal;
        writeSector((byte*)buf, fatSect, driveNo);
        fatSect += fatSize;
    }
}

dword getClusterChainLength(dword startCluster, byte drive) {
    dword len = 0;
    dword clus = startCluster;
    do {
        len ++;
        clus = getCluster(drive, clus);
    } while (!is_eof(clus));
    return len;
}