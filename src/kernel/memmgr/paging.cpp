#include "paging.hpp"
#include "../io/com.hpp"

void createPageTable(uint32_t vaddr) {
    uint16_t tableNo = (vaddr & 0xFFC00000) >> 22;
    uint32_t *dirEntryPtr = (uint32_t*)(PAGING_BASE + tableNo * 4);
    uint32_t tableAddr = PAGING_BASE + 0x1000 + tableNo * 0x1000;
    *dirEntryPtr = tableAddr | 3;
    uint32_t *tableEntryPtr = (uint32_t*)tableAddr;
    for (uint16_t i = 0; i < 1024; i++) {
        *tableEntryPtr = 2;
        tableEntryPtr ++;
    }
}

void createPages(uint32_t vaddr, uint32_t paddr, uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        createPage(vaddr + i * 0x1000, paddr + i * 0x1000);
    }
}

void createPage(uint32_t vaddr, uint32_t paddr) {
    uint16_t tableNo = (vaddr & 0xFFC00000) >> 22;
    uint32_t *dirEntryPtr = (uint32_t*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        createPageTable(vaddr);
    uint32_t *pagePtr = (uint32_t*)(PAGING_BASE + ((vaddr & 0x3FF000) >> 12) * 4 + (tableNo + 1) * 0x1000);
    *pagePtr = paddr | 3;
}

uint32_t getPhysAddr(uint32_t vaddr) {
    uint16_t tableNo = (vaddr & 0xFFC00000) >> 22;
    uint32_t *dirEntryPtr = (uint32_t*)(PAGING_BASE + tableNo * 4);
    if (!(*dirEntryPtr & 0x1))
        return 0;
    uint32_t *pagePtr = (uint32_t*)(PAGING_BASE + ((vaddr & 0x3FF000) >> 12) * 4 + (tableNo + 1) * 0x1000);
    return (*pagePtr & 0xFFFF000) + (vaddr & 0xFFF);
}