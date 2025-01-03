#include "aml.hpp"
#include "../str/str.hpp"

qword evaluateDeviceObj(dword *path, byte len, const char *objName, ...) {
    clearVarPath();
    memcpy((byte*)path, (byte*)varPath, len * 4);
    varPath += len;
    if (strlen((char*)objName) < 4) {
        ((char*)objName)[3] = '_';
    }
    *varPath++ = *(AMLName*)objName;
    kdebug("Полный путь к объекту устройства: ");
    logVarPath();
    kdebugnewl();
    byte *addr = getACPIObjAddr((dword*)varPathBase, varPathLen);
    if (!addr)
        return maxqword;
    qword value = 0;
    if (*addr == 0x14) {
        va_list argv;
        va_start(argv, objName);
        value = callMethod((AMLName*)varPathBase, varPathLen, argv);
    } else {    
        FuncFrame _frame = {};
        _frame.addr = kmalloc(0x1000);
        value = getTermArg(addr, &_frame).value;
        kfree(_frame.addr);
    }
    return value;
}

void transitionToDState(dword *path, byte len, byte state) {
    char *obj = (char*)"_PSx";
    obj[3] = 0x30 + state;
    kdebug("Устройство переводится в состояние D%d.\n", state);
    evaluateDeviceObj(path, len, (const char*)obj);
}