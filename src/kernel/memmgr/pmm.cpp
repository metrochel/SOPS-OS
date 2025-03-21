#include "pmm.hpp"
#include "../util/util.hpp"
#include "../run/process.hpp"
#include "../kernel.hpp"
#include "../io/com.hpp"

byte *pageFramesMap = nullptr;
qword mapLen = 0;

dword lastFreeByte = 0;

inline void occupyPage(ptrint paddr) {
    ptrint frameNo = paddr >> 12;
    pageFramesMap[frameNo / 8] |= 1 << (frameNo % 8);
}

void initPMM(byte *&memMgrPtr) {
    kdebug("Начата инициализация менеджера физической памяти.\n");

    MemMapEntry *entries = (MemMapEntry*)((byte*)bld + sizeof(BootLoaderData));
    word memMapC = bld->MemMapEntriesCount;

    qword memAmt = entries[memMapC-1].base + entries[memMapC-1].size;
    kdebug("Системе доступно %D МБ оперативной памяти.\n", memAmt / 1024 / 1024);
    mapLen = memAmt / 4096;
    if (memAmt == 0) {
        #ifdef __x86_64__
        mapLen = 0x10000000000000;
        #else
        mapLen = 0x100000;
        #endif
    }
    kdebug("Длина разметки физической памяти: %D Б.\n", mapLen);

    pageFramesMap = memMgrPtr;
    memMgrPtr += mapLen;
    memset(pageFramesMap, mapLen, 0);
    memset(pageFramesMap, (0x8000000 / PAGE_SIZE) / 8, maxbyte);

    kdebug("Анализ разметки памяти.\n");
    for (word i = 0; i < memMapC; i++) {
        MemMapEntry entry = entries[i];
        kdebug("Метка %d:\n", i+1);
        kdebug("\tОснование: %X\n", entry.base);
        kdebug("\tРазмер: %D Б (%X)\n", entry.size, entry.size);
        kdebug("\tТип: %d\n", entry.type);
        kdebug("\tРасширенные аттрибуты: %b\n", entry.exAttributes);
        
        if (entry.type != MemMapEntryType::Unoccupied) {
            dword pagesCount = (entry.size + PAGE_SIZE - 1) / PAGE_SIZE;
            for (dword j = 0; j < pagesCount; j++) {
                occupyPage(entry.base + j * PAGE_SIZE);
            }
        }
    }
    kdebug("Адрес карты физической памяти: %x.\n", pageFramesMap);

    kdebug("Инициализация менеджера физической памяти завершена.\n");
}

ptrint allocatePageFrame() {
    for (qword i = lastFreeByte; i < mapLen; i++) {
        for (byte j = 0; j < 8; j++) {
            if ((pageFramesMap[i] & (1 << j)) == 0) {
                ptrint addr = ((i*8) + j) * PAGE_SIZE;
                pageFramesMap[i] |= (1 << j);
                lastFreeByte = i;
                return addr;
            }
        }
    }
    return 0;
}

void freePageFrame(ptrint frame) {
    dword frameNo = frame >> 12;
    if (frameNo/8 >= mapLen) {
        return;
    }

    pageFramesMap[frameNo/8] &= ~(1 << (frameNo % 8));
    lastFreeByte = frameNo/8;
}