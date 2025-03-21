#include "file.hpp"
#include "../disk/disk.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"
#include "../dbg/dbg.hpp"
#include "../str/str.hpp"

FileHandle *handles = nullptr;

void initFiles() {
    handles = (FileHandle*)kmalloc(MAX_FILE_HANDLES * sizeof(FileHandle));
    memset(handles, MAX_FILE_HANDLES * sizeof(FileHandle), 0);
}

FileHandle *openFile(char *path, byte driveNo, byte mode) {
    kdebug("Получен запрос на открытие файла ");
    kdebug(path);
    kdebug(" на диске %d.\n", driveNo);
    FileHandle handle = {};
    if (disks[driveNo].diskBus == NotConnected) {
        kdebug("ОШИБКА: Диск %d не подключён\n");
        return nullptr;
    }
    
    if (disks[driveNo].filesystem == Unrecognized) {
        kdebug("ОШИБКА: Файловая система диска %d не распознана\n");
        return nullptr;
    }

    word handleSlot = maxword;
    for (word i = 0; i < MAX_FILE_HANDLES; i++) {
        if (handles[i].file)
            continue;

        handleSlot = i;
        break;
    }

    if (handleSlot == maxword) {
        kdebug("ОШИБКА: Слот для обработчика не выделен\n");
        return nullptr;
    }

    if (disks[driveNo].filesystem == FAT32) {
        kdebug("Файловая система диска %d - FAT32.\n", driveNo);
        FAT32_File f(path, driveNo, (mode & 7) == FILE_MODE_WRITE || (mode & 7) == FILE_MODE_APPEND, false);
        if (!f || (f.name && (mode & FILE_MODE_FLAG_EX))) {
            return nullptr;
        }
        handle.file = (File*)kmalloc(sizeof(FAT32_File));
        *handle.file = f;
        memcpy((byte*)&f, (byte*)handle.file, sizeof(FAT32_File));
    } else {
        return nullptr;
    }

    handle.filePos = 0;
    handle.eof = 0;
    handle.error = 0;
    handle.buffer = nullptr;
    handle.bufferSize = 0;
    handle.mode = mode;

    if ((mode & 7) == FILE_MODE_WRITE) {
        handle.file->clear();
    }

    kdebug("Открытие завершено успешно.\n");

    handles[handleSlot] = handle;
    return handles + handleSlot;
}

void closeFile(FileHandle *handle) {
    kdebug("Файл ");
    kdebug(handle->file->name);
    kdebug(" закрывается.\n");
    kfree(handle->file);
    *handle = {};
}

bool readFile(FileHandle *handle, dword size, byte *out) {
    if ((handle->mode & FILE_MODE_FLAG_UPD) == 0 && handle->mode & 7 != FILE_MODE_READ) {
        handle->error = 1;
        return false;
    }

    bool result = handle->file->read(handle->filePos, size, out);
    if (!result) {
        handle->error = 1;
        return false;
    }
    handle->filePos += size;
    if (handle->filePos == handle->file->size - 1)
        handle->eof = 1;
    return true;
}

bool writeFile(FileHandle *handle, dword size, byte *in) {
    if ((handle->mode & FILE_MODE_FLAG_UPD) == 0 && (handle->mode & 7) != FILE_MODE_WRITE && (handle->mode & 7) != FILE_MODE_APPEND) {
        handle->error = 1;
        return false;
    }

    if ((handle->mode & FILE_MODE_FLAG_BIN) == 0 && in[size-1] == 0)
        size --;

    bool result = handle->file->write(in, size);
    if (!result) {
        handle->error = 1;
        return false;
    }
    handle->filePos += size;
    return true;
}

bool createFolder(char *path, byte drive, bool force) {
    kdebug("Начато создание папки по пути ");
    kdebug(path);
    kdebug(" на диске %d.\n", drive);
    kdebug("Файловая система диска %d - ");
    if (disks[drive].filesystem == FAT32) {
        kdebug("FAT32.\n");
        char **pathComponents = strsplit(path, "/");
        dword directoryCluster = root(drive);
        dword startCluster = 0;
        word dirEntryOffset = 0;

        FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];
        byte *nameBuf = kmalloc(13*64);
        memset(nameBuf, sizeof(nameBuf), 0);

        char **_pathComponents = pathComponents;
        char *name = nullptr;

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

            if (!*pathComponents) {
                directoryCluster = dirStart;
                dirEntryOffset = maxword;
                startCluster = 0;
                dword namelen = strlen(*--pathComponents);
                name = (char*)kmalloc(namelen + 1);
                strcpy(*pathComponents, name);
                kfree(_pathComponents);
                kfree(nameBuf);
                break;
            }

            if (*pathComponents && force) {
                FAT32_File element(ipath, FAT_FILEATTR_DIRECTORY, drive, 0, kgettime(), dirStart);
                element.create();
                directoryCluster = element.startCluster;
            }
        }

        if (startCluster) return false;

        if (!name) {
            dword namelen = strlen(*(--pathComponents));
            name = (char*)kmalloc(namelen + 1);
            strcpy(*pathComponents, name);
        }

        FAT32_File folder(name, FAT_FILEATTR_DIRECTORY, drive, 0, kgettime(), directoryCluster);

        folder.startCluster = allocateCluster(drive);
        setCluster(drive, folder.startCluster, FAT_CLUSTER_EOF);
        folder.create();
        return true;
    }
    kdebug("<не определена>.\nОШИБКА: Создание папки провалено\n");
    return false;
}