#include "shell.hpp"
#include "../graphics/glyphs.hpp"
#include "../keyboard/keyboard.hpp"
#include "../timing/time.hpp"
#include "../str/str.hpp"
#include "../acpi/acpi.hpp"
#include "../fat/fat.hpp"

char path[1000];
dword directoryCluster;
byte drive;

void shellMain(byte driveNo) {
    byte *stdin = (byte*)0x9300;
    memset((byte*)path, 1000, 0);
    path[0] = '/';
    drive = driveNo;
    directoryCluster = root(drive);
    while (true) {
        kprint("\n");
        kprint(path);
        kprint(">");
        kread(stdin);
        kprint("\n");
        stdin = (byte*)0x9300;
        if (strcmp((char*)stdin, (char*)"time")) {
            cmdTime();
        } 
        else if (strcmp((char*)stdin, (char*)"shutdown")) {
            cmdShutdown();
        } 
        else if (strcmp((char*)stdin, (char*)"reboot")) {
            cmdReboot();
        } 
        else if (strcmp((char*)stdin, (char*)"ls")) {
            cmdLs();
        }
        else if (strstartswith((char*)stdin, (char*)"cd ")) {
            cmdCd((char*)(stdin + 3));
        }
        else {
            kerror("ОШИБКА: Команды или исполняемого файла \"");
            kerror((const char*)stdin);
            kerror("\" не существует.\nПроверьте правильность написания команды.");
        }
    }
}

void cmdTime() {
    kprint("Сейчас ");
    char *out = (char*)0x11000;
    Time time = kgettime();
    out = (char*)0x11000;
    out += time.asStringWeekday(out);
    *out++ = ',';
    *out++ = ' ';
    time.asStringFull(out);
    out -= 6;
    kprint(out);
    kprint(".");
}

void cmdShutdown() {
    kprint("До свидания!");
    kshutdown();
}

void cmdReboot() {
    kprint("Перезагрузка!");
    krestart();
}

void cmdLs() {
    FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];
    dword entriesCount = 0;
    dword clus = directoryCluster;
    dword _clus = getCluster(drive, clus);
    while (!is_eof(clus)) {
        readCluster(drive, clus, (byte*)clusterBuf);
        for (word i = 0; i < clustersize(drive) / sizeof(FAT_DirEntry); i++) {
            if (clusterBuf[i].name[0] == 0xE5)
                continue;
            if (clusterBuf[i].name[0] == 0x00)
                break;
            if (!is_lfn(clusterBuf[i]))
                entriesCount ++;
        }
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }

    char **files = (char**)kmalloc(sizeof(char**) * (entriesCount + 1));
    char **_files = files;
    dword *sizes = (dword*)kmalloc(sizeof(dword) * entriesCount);
    dword *_sizes = sizes;
    bool *folders = (bool*)kmalloc(entriesCount);
    bool *_folders = folders;

    char buf[1000];
    dword filesCount = 0;
    dword foldersCount = 0;
    clus = directoryCluster;
    _clus = getCluster(drive, clus);
    while (!is_eof(clus)) {
        readCluster(drive, clus, (byte*)clusterBuf);
        for (word i = 0; i < clustersize(drive) / sizeof(FAT_DirEntry); i++) {
            if (clusterBuf[i].name[0] == 0xE5)
                continue;
            if (clusterBuf[i].name[0] == 0x00)
                break;
            if (is_lfn(clusterBuf[i]))
                continue;

            *sizes++ = clusterBuf[i].fileSize;
            if (clusterBuf[i].attr & FAT_FILEATTR_DIRECTORY) {
                foldersCount ++;
                *folders++ = 1;
            } else {
                *folders++ = 0;
                filesCount ++;
            }

            FAT_LFNEntry *entries = (FAT_LFNEntry*)(clusterBuf + i - 1);
            byte *ptr = (byte*)buf;
            if (entries->attr != FAT_FILEATTR_LFN) {
                extractShortName(clusterBuf[i], ptr);
                char *name = (char*)kmalloc(strlen(buf)+1);
                strcpy(buf, name);
                *files++ = name;
                continue;
            }

            while (entries->order < 0x40) {
                extractLFNName(entries, ptr);
                ptr += 13;
                entries --;
            }
            extractLFNName(entries, ptr);
            ptr += 13;
            char *name = (char*)kmalloc(strlen(buf)+1);
            strcpy(buf, name);
            *files++ = name;
        }
        clus = _clus;
        _clus = getCluster(drive, _clus);
    }

    *files = 0;
    files = _files;
    sizes = _sizes;
    folders = _folders;

    for (dword i = 0; i < entriesCount; i++) {
        for (dword j = i + 1; j < entriesCount; j++) {
            if (strcmpS(files[j], files[i], true) == 0x00) {
                char *_fname = files[j];
                files[j] = files[i];
                files[i] = _fname;

                dword _fsize = sizes[j];
                sizes[j] = sizes[i];
                sizes[i] = _fsize;

                bool _ffold = folders[j];
                folders[j] = folders[i];
                folders[i] = _ffold;
            }
        }
    }

    kprint("Папка ");
    kprint(path);
    kprint("\n");
    if (filesCount % 10 == 1 && (filesCount / 10) % 10 != 1)
        kprint("%d файл\n", filesCount);
    else if (filesCount % 10 < 5 && filesCount > 1 && (filesCount / 10) % 10 != 1)
        kprint("%d файла\n", filesCount);
    else if (filesCount)
        kprint("%d файлов\n", filesCount);

    if (foldersCount % 10 == 1 && (foldersCount / 10) % 10 != 1)
        kprint("%d папка\n", foldersCount);
    else if (foldersCount % 10 < 5 && foldersCount > 1 && (foldersCount / 10) % 10 != 1)
        kprint("%d папки\n", foldersCount);
    else if (filesCount)
        kprint("%d папок\n", foldersCount);

    kprint("  ");
    while (*files) {
        char *file = *files++;
        dword size = *sizes++;
        bool folder = *folders++;

        if (!folder) {
            kprint(file);
            kprint(" (%d Б)", size);
        } else {
            kprint("[");
            kprint(file);
            kprint("]");
        }
        kprint("\n  ");
        kfree(file);
        kfree(&size);
        kfree(&folder);
    }
    kfree(_files);
    kfree(_sizes);
    kfree(_folders);
}

void cmdCd(char *newPath) {
    if (newPath[0] == '/')
        directoryCluster = root(drive);
    
    char _path[1000];
    memcpy((byte*)path, (byte*)_path, 1000);

    char **pathComponents = strsplit(newPath, "/");
    char **_pathComponents = pathComponents;

    FAT_DirEntry clusterBuf[clustersize(drive) / sizeof(FAT_DirEntry)];
    char buf[13*64];

    while (*pathComponents) {
        char *ipath = *pathComponents++;

        dword clus = directoryCluster;
        dword _clus = getCluster(drive, clus);
        char nameBuf[13*64];
        bool found = false;
        while (!is_eof(clus)) {
            readCluster(drive, clus, (byte*)clusterBuf);
            for (word i = 0; i < clustersize(drive) / sizeof(FAT_DirEntry); i++) {
                if (clusterBuf[i].name[0] == 0xE5)
                    continue;
                if (clusterBuf[i].name[0] == 0x00)
                    break;
                if (is_lfn(clusterBuf[i]))
                    continue;

                FAT_LFNEntry *entries = (FAT_LFNEntry*)(clusterBuf + i - 1);
                byte *ptr = (byte*)nameBuf;
                if (entries->attr != FAT_FILEATTR_LFN) {
                    extractShortName(clusterBuf[i], ptr);
                } else {
                    while (entries->order < 0x40) {
                        extractLFNName(entries--, ptr);
                        ptr += 13;
                    }
                    extractLFNName(entries, ptr);
                    ptr += 13;
                }
                if (strcmp(nameBuf, ipath)) {
                    if ((clusterBuf[i].attr & FAT_FILEATTR_DIRECTORY) == 0) {
                        kerror("ОШИБКА: ");
                        kerror(ipath);
                        kerror(" - это файл\n");
                        memcpy((byte*)_path, (byte*)path, 1000);
                        kfree(_pathComponents);
                        kfree(++pathComponents);
                        return;
                    }
                    dword clus = (clusterBuf[i].clusterHi << 16) | clusterBuf[i].clusterLo;
                    if (clus == 0) clus = root(drive);
                    directoryCluster = clus;
                    found = true;
                    break;
                }
            }
            if (found) break;
            clus = _clus;
            _clus = getCluster(drive, _clus);
        }
        
        if (!found) {
            kerror("ОШИБКА: ");
            kerror(ipath);
            kerror(": нет такой папки\n");
            memcpy((byte*)_path, (byte*)path, 1000);
            kfree(_pathComponents);
            kfree(++pathComponents);
            return;
        }

        if (strcmp(ipath, "."))
            continue;
        if (strcmp(ipath, "..")) {
            word i = strlen(path);
            while (path[i] != '/')
                path[i--] = 0;
            path[i] = 0;
            if (i == 0)
                path[i] = '/';
            continue;
        }

        char *ptr = (char*)path + strlen(path);
        if (ptr != path+1)
            *ptr++ = '/';
        strcpy(ipath, ptr);
    }
    if (path[strlen(path)] == '/')
        path[strlen(path)] = 0x00;
    kfree(_pathComponents);
    kfree(++pathComponents);
}