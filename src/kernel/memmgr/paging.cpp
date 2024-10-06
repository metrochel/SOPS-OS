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

void createPages(dword vaddr, dword paddr, word count) {
    for (word i = 0; i < count; i++) {
        createPage(vaddr + i * 0x1000, paddr + i * 0x1000);
    }
}

void createPage(dword vaddr, dword paddr) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        createPageTable(vaddr);
    dword *pagePtr = (dword*)(PAGING_BASE + ((vaddr & 0x3FF000) >> 12) * 4 + (tableNo + 1) * 0x1000);
    *pagePtr = paddr | 3;
}

dword getPhysAddr(dword vaddr) {
    word tableNo = (vaddr & 0xFFC00000) >> 22;
    dword *dirEntryPtr = (dword*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        return 0;
    dword *pagePtr = (dword*)(PAGING_BASE + ((vaddr & 0x3FF000) >> 12) * 4 + (tableNo + 1) * 0x1000);
    return (*pagePtr & 0xFFFF000) + (vaddr & 0xFFF);
}