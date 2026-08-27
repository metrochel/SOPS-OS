#include "memmgr.hpp"
#include "../libk/util.hpp"

#define MEM_MGR_DATA_START  0xB0000000
#define PID_KERNEL 0

void initMemMgr() {
    byte *memMgrData = (byte*)MEM_MGR_DATA_START;
    createPages(MEM_MGR_DATA_START, 0x6000000, 8192);
    initPMM(memMgrData);
    initVMM(memMgrData);
}

byte *kmalloc(dword amount, word pid) {
    byte *alloc = virtAlloc(amount, pid);
    if (!alloc)
        return nullptr;

//    Process p = getProcessData(pid);
//    if (!p.startAddress) {
//        p.startAddress = (ptrint)alloc;
//    }

//    p.usedMemory += amount;
//    setProcessData(pid, p);
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
    memcpy(_ptr, ptr, prevSz >= newSize ? newSize : prevSz);
    kfree(_ptr);
}

void kfree(void *var, word pid) {
    if (!var) return;
    dword freed = virtFree(var, pid);
//    Process p = getProcessData(pid);
//    p.usedMemory -= freed;
//    setProcessData(pid, p);
}

void kfree(void *var) {
    virtFree(var, PID_KERNEL);
}