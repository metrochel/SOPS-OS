#include "memmgr.hpp"
#include "../kernel.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"
#include "../run/process.hpp"

const ptrint memMgrDataStart = 0xB0000000;

void initMemMgr() {
    byte *memMgrData = (byte*)memMgrDataStart;
    createPages(memMgrDataStart, 0x6000000, 8192);
    initPMM(memMgrData);
    initVMM(memMgrData);
}

byte *kmalloc(dword amount, word pid) {
    byte *alloc = virtAlloc(amount, pid);
    if (!alloc)
        return nullptr;

    Process p = getProcessData(pid);
    if (!p.startAddress) {
        p.startAddress = (ptrint)alloc;
    }

    p.usedMemory += amount;
    setProcessData(pid, p);
    return alloc;
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

void krealloc(void *&ptr, dword newSize) {
    void *_ptr = ptr;
    ptr = kmalloc(newSize);
    dword prevSz = getVarSz(ptr);
    memcpy((byte*)_ptr, (byte*)ptr, prevSz >= newSize ? newSize : prevSz);
    kfree(ptr);
}

void kfree(void *var, word pid) {
    dword freed = virtFree(var, pid);
    Process p = getProcessData(pid);
    p.usedMemory -= freed;
    setProcessData(pid, p);
}

void kfree(void *var) {
    virtFree(var, PID_KERNEL);
}