#include "syscalls.hpp"
#include "process.hpp"
#include "../graphics/glyphs.hpp"
#include "../keyboard/keyboard.hpp"
#include "../disk/disk.hpp"
#include "../file/file.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"
#include "../dbg/dbg.hpp"

dword processSyscall(Syscall call, word pid, dword arg1, dword arg2, dword arg3, dword arg4, dword arg5) {
    if (call == Exit) {
        return 0;
    }

    if (call == PutCharacter) {
        setchar(arg2, arg3, arg1, arg4, arg5);
        Glyph g = getglyph((word)arg1);
        putglyph(g, arg2 * 16, arg3 * 24, arg4, arg5);
        return 0;
    }
    
    if (call == ClearScreen) {
        kdebug("Процесс %d очищает экран.\n", pid);
        Process p = getProcessData(pid);
        Character *buf = textBuffer;
        if (!p.clearedScreen) {
            buf = (Character*)kmalloc(textBufferBankSize * 2);
            auxTextCurX = textCurX;
            auxTextCurY = textCurY;
            auxTextBuffer = textBuffer;
            textBuffer = buf;
            magicBreakpoint();
            p.clearedScreen = 1;
        }
        memset(textBuffer, textBufferBankSize*2, 0);
        textCurX = 1;
        textCurY = 1;
        drawLine = 0;
        refreshScreen();
        setProcessData(pid, p);
        return 0;
    }

    if (call == Print) {
        char *text = (char*)arg1;
        printStr((const char*)text, null, defaultTextCol, defaultBGCol, false);
        return 0;
    }

    if (call == PositionedPrint) {
        word _textCurX = textCurX;
        word _textCurY = textCurY;
        disableCursor();
        textCurX = arg2;
        textCurY = arg3;
        char *text = (char*)arg1;
        printStr((const char*)text, null, defaultTextCol, defaultBGCol, false);
        textCurX = _textCurX;
        textCurY = _textCurY;
        return 0;
    }

    if (call == PositionedColouredPrint) {
        word _textCurX = textCurX;
        word _textCurY = textCurY;
        disableCursor();
        textCurX = arg2;
        textCurY = arg3;
        char *text = (char*)arg1;
        printStr((const char*)text, null, arg4, arg5, false);
        textCurX = _textCurX;
        textCurY = _textCurY;
        return 0;
    }
    
    if (call == SetScreenBounds) {
        Process p = getProcessData(pid);
        kdebug("Процесс %d изменяет границы экрана.\n");
        kdebug("Левый верхний угол: (%d; %d).\n", arg1, arg2);
        kdebug("Правый нижний угол: (%d; %d).\n", arg3, arg4);
        textLeftBoundX = arg1;
        textLeftBoundY = arg2;
        textRightBoundX = arg3;
        textRightBoundY = arg4;
        disableCursor();
        textCurX = textLeftBoundX;
        textCurY = textLeftBoundY;
        enableCursor();
        p.changedBounds = 1;
        setProcessData(pid, p);
        return 0;
    }

    if (call == GetScreenBounds) {
        return ((dword)textScreenHeight << 16) | textScreenWidth; 
    }

    if (call == DisableCursor) {
        disableCursor();
        return 0;
    }

    if (call == EnableCursor) {
        enableCursor();
        return 0;
    }

    if (call == ReadKey) {
        return kreadkey();
    }

    if (call == ReadStr) {
        word ownerPid = getBlockOwnerPID(arg1, maxInputSize);
        if (ownerPid != pid) {
            kdebug("ВНИМАНИЕ: Процесс %d запросил использование блока, ему не принадлежащего\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        kread((byte*)arg1);
    }

    if (call == OpenFile) {
        Process p = getProcessData(pid);
        word handleSlot = maxword;
        for (byte i = 0; i < PROC_MAX_FILES; i++) {
            if (!p.handles[i]) {
                handleSlot = i;
                break;
            }
        }
        if (handleSlot >= PROC_MAX_FILES) {
            kdebug("ВНИМАНИЕ: Процесс %d попытался открыть больше, чем %d файлов, одновременно", pid, PROC_MAX_FILES);
            return RUNTIME_ERROR_FILE_MAX_REACHED;
        }

        char *path = (char*)arg1;
        byte mode = arg2;
        byte drive = determineDriveNo(path);
        while (*path != '}') {path ++;}
        path ++;
        FileHandle *handle = openFile(path, drive, mode);
        if (!handle) {
            kdebug("ВНИМАНИЕ: Открытие файла провалено\n");
            return RUNTIME_ERROR_FILE_OPEN_FAILURE;
        }
        p.handles[handleSlot] = handle;
        setProcessData(pid, p);
        return (dword)handle;
    }

    if (call == CloseFile) {
        FileHandle *handle = (FileHandle*)arg1;
        word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
        if (ownerPid != PID_KERNEL) {
            kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        closeFile(handle);
        return 0;
    }

    if (call == Read) {
        byte *out = (byte*)arg1;
        dword size = arg2;
        FileHandle *handle = (FileHandle*)arg3;

        word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
        if (ownerPid != PID_KERNEL) {
            kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }

        ownerPid = getBlockOwnerPID(arg1, size);
        if (ownerPid != pid) {
            kdebug("ОШИБКА: Блок не принадлежит процессу\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        
        bool res = readFile(handle, size, out);
        if (!res) {
            kdebug("ОШИБКА: Чтение из файла провалено\n");
            return RUNTIME_ERROR_FILE_READ_FAILURE;
        }
        return 0;
    }

    if (call == ReadFull) {
        byte *out = (byte*)arg1;
        FileHandle *handle = (FileHandle*)arg2;
        dword size = handle->file->size;
        word ownerPid = getBlockOwnerPID(arg2, sizeof(FileHandle));
        if (ownerPid != PID_KERNEL) {
            kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }

        ownerPid = getBlockOwnerPID(arg1, size);
        if (ownerPid != pid) {
            kdebug("ОШИБКА: Блок не принадлежит процессу\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        
        bool res = readFile(handle, handle->file->size - handle->filePos, out);
        if (!res) {
            kdebug("ОШИБКА: Чтение из файла провалено\n");
            return RUNTIME_ERROR_FILE_READ_FAILURE;
        }
        return 0;
    }

    if (call == Write) {
        byte *in = (byte*)arg1;
        dword size = arg2;
        FileHandle *handle = (FileHandle*)arg3;
        word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
        if (ownerPid != PID_KERNEL) {
            kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }

        ownerPid = getBlockOwnerPID(arg1, size);
        if (ownerPid != pid) {
            kdebug("ОШИБКА: Блок не принадлежит процессу\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        bool res = writeFile(handle, size, in);
        if (!res) {
            kdebug("ОШИБКА: Запись в файл провалена\n");
            return RUNTIME_ERROR_FILE_WRITE_FAILURE;
        }
        return 0;
    }

    if (call == GetFileSize) {
        FileHandle *handle = (FileHandle*)arg1;
        word ownerPid = getBlockOwnerPID(arg3, sizeof(FileHandle));
        if (ownerPid != PID_KERNEL) {
            kdebug("ОШИБКА: Обработчик файла вне пространства ядра\n");
            kdebug(ownerPid == maxword ? "Блок принадлежит нескольким процессам.\n" : "Блок принадлежит процессу %d.\n", ownerPid);
            return RUNTIME_ERROR_INVALID_ACCESS;
        }
        return handle->file->size;
    }

    if (call == Malloc) {
        ptrint alloc = (ptrint)kmalloc(arg1, pid);
        if (!alloc) {
            kdebug("ВНИМАНИЕ: Выделение памяти для процесса %d провалено", pid);
            return RUNTIME_ERROR_ALLOC_FAILURE;
        }
        return alloc;
    }

    if (call == Free) {
        kfree((void*)arg1, pid);
        return 0;
    }

    return maxdword;
}