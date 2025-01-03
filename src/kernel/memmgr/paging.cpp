#include "paging.hpp"

void createPageTable(dword vaddr) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    dword tableAddr = PAGING_BASE + 0x1000 + tableNo * 0x1000;
    *dirEntryPtr = tableAddr | 3;
    dword *tableEntryPtr = (dword*)tableAddr;
    for (word i = 0; i < 1024; i++) {
        *tableEntryPtr = 2;
        tableEntryPtr ++;
    }
}

void createPages(dword vaddr, dword paddr, dword count) {
    for (word i = 0; i < count; i++) {
        createPage(vaddr + i * 0x1000, paddr + i * 0x1000);
    }
}

void createPage(dword vaddr, dword paddr) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        createPageTable(vaddr);
    dword *pagePtr = (dword*)(PAGING_BASE + (tableNo + 1) * 0x1000 + ((vaddr & 0x3FF000) >> 12) * 4);
    *pagePtr = paddr | 3;
}

dword getPhysAddr(dword vaddr) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        return 0;
    dword *pagePtr = (dword*)(PAGING_BASE + (tableNo + 1) * 0x1000 + ((vaddr & 0x3FF000) >> 12) * 4);
    return (*pagePtr & 0xFFFF000) + (vaddr & 0xFFF);
}

void setPagePermsLevel(dword vaddr, byte cpl) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (cpl) *dirEntryPtr |= 4;
    else *dirEntryPtr &= ~4;
    dword tableAddr = PAGING_BASE + 0x1000 + tableNo * 0x1000;
    dword *pagePtr = (dword*)(PAGING_BASE + (tableNo + 1) * 0x1000 + ((vaddr & 0x3FF000) >> 12) * 4);
    if (cpl) *pagePtr |= 4;
    else *pagePtr &= ~4;
}

void setPageReadWrite(dword vaddr, bool rw) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (rw) *dirEntryPtr |= 2;
    else *dirEntryPtr &= ~2;
    dword tableAddr = PAGING_BASE + 0x1000 + tableNo * 0x1000;
    dword *pagePtr = (dword*)(PAGING_BASE + (tableNo + 1) * 0x1000 + ((vaddr & 0x3FF000) >> 12) * 4);
    if (rw) *pagePtr |= 2;
    else *pagePtr &= ~2;
}