#include "process.hpp"
#include "../libk/util.hpp"
#include "../memmgr/memmgr.hpp"
#include "../graphics/glyphs.hpp"
#include "../io/com.hpp"

Process *processData = nullptr;

void initProcessesLib() {
    processData = (Process*)kmalloc(sizeof(Process) * (maxword + 1));
    memset(processData, sizeof(Process) * (maxword + 1), 0);
}

Process getProcessData(word pid) {
    return processData[pid];
}

void setProcessData(word pid, Process p) {
    processData[pid] = p;
}

Process registerProcess() {
    if (!processData) return {};

    word pid = 0;
    for (word i = 2; i < maxword; i++) {
        if (processData[i].pid == 0) {
            pid = i;
            break;
        }
    }
    if (!pid)
        return {};

    Process *process_ptr = processData + pid;

    Process p = processData[pid];
    memset(&p, sizeof(Process), 0);
    p.pid = pid;

    stdin_file stdin(process_ptr);
    stdout_file stdout(process_ptr);
    stderr_file stderr(process_ptr);

    stdin_file *stdin_ptr = (stdin_file*)kmalloc(sizeof stdin);
    if (!stdin_ptr)
        return {};
    memcpy((byte*)&stdin, (byte*)stdin_ptr, sizeof stdin);
    p.stdin = stdin_ptr;

    stdout_file *stdout_ptr = (stdout_file*)kmalloc(sizeof stdout);
    if (!stdout_ptr)
        return {};
    memcpy((byte*)&stdout, (byte*)stdout_ptr, sizeof stdout);
    p.stdout = stdout_ptr;

    stderr_file *stderr_ptr = (stderr_file*)kmalloc(sizeof stderr);
    if (!stderr_ptr)
        return {};
    memcpy((byte*)&stderr, (byte*)stderr_ptr, sizeof stderr);
    p.stderr = stderr_ptr;

    p.parent = nullptr;

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

    for (byte i = 0; i < PROC_MAX_FILES; i++) {
        if (process.files[i])
            closeFile(process.files[i]);
    }

    processData[pid] = {};
}

word determine_pid(ptrint addr) {
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