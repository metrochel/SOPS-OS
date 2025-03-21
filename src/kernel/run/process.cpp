#include "process.hpp"
#include "../util/util.hpp"
#include "../memmgr/memmgr.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"

Process *processData = nullptr;

void initProcessesLib() {
    processData = (Process*)kmalloc(sizeof(Process) * (maxword + 1));
    memset(processData, sizeof(Process) * (maxword + 1), 0);
}

Process registerProcess() {
    word pid;
    for (word i = 2; i < maxdword; i++) {
        if (processData[i].pid == 0) {
            pid = i;
            break;
        }
    }
    if (!pid)
        return {};

    Process p = processData[pid];
    p.pid = pid;
    p.clearedScreen = 0;
    p.changedBounds = 0;
    p.usedMemory = 0;
    processData[pid] = p;
    return p;
}

void unregisterProcess(word pid) {
    kdebug("Ресурсы процесса %d освобождаются.\n", pid);
    if (pid == PID_KERNEL) {
        kdebug("ОШИБКА: Невозможно удалить ядро\n");
        kdebug("PID %d равен PID ядра.\n", pid);
        return;
    }

    Process process = processData[pid];
    for (dword i = 0; i < PAGE_TABLES_COUNT; i++) {
        if (tables[i].pid == pid)
            destroyPageTable(i);
    }

    if (process.clearedScreen) {
        kfree(textBuffer);
        textBuffer = auxTextBuffer;
        auxTextBuffer = nullptr;
        textCurX = auxTextCurX;
        textCurY = auxTextCurY;
        refreshScreen();
    }

    if (process.changedBounds) {
        textLeftBoundX = 1;
        textLeftBoundY = 1;
        textRightBoundX = textScreenWidth - 1;
        textRightBoundY = textScreenHeight - 1;
    }

    processData[pid] = {};
}

word determinePID(ptrint addr) {
    dword tableNo = addr >> 22;
    return tables[tableNo].pid;
}

word getBlockOwnerPID(ptrint addr, ptrint size) {
    dword checkTable = addr >> 22;
    word pid = tables[checkTable].pid;
    while (checkTable << 22 < addr + size) {
        if (tables[checkTable].pid != pid)
            return maxword;
        checkTable ++;
    }
    return pid;
}