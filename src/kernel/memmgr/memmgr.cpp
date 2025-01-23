#include "memmgr.hpp"
#include "../kernel.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"
#include "../run/process.hpp"

const ptrint memMgrDataStart = 0xB0000000;

void initMemMgr() {
    byte *memMgrData = (byte*)memMgrDataStart;
    createPages(0xB0000000, 0x6000000, 8192);
    initPMM(memMgrData);
    initVMM(memMgrData);
}

byte *kmalloc(dword amount, word pid) {
    return virtAlloc(amount, pid);
}

byte *kmalloc(dword amount) {
    return virtAlloc(amount, PID_KERNEL);
}

byte *kmallocPhys(ptrint vaddr, dword amount, word pid) {
    return virtAlloc(vaddr, amount, pid);
}

byte *kmallocPhys(ptrint vaddr, dword amount) {
    return virtAlloc(vaddr, amount, PID_KERNEL);
}

void kfree(void *var, word pid) {
    virtFree(var, pid);
}

void kfree(void *var) {
    virtFree(var, PID_KERNEL);
}