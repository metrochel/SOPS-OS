#include "memmgr.hpp"
#include "../kernel.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"
#include "../run/process.hpp"

const qword memBlocksStart = 0xB0000000;
const qword memMgrSize = 0x100000;
MemBlock *memBlocks = (MemBlock*)memBlocksStart;

inline dword getBlocksCount() {
    return ((dword)memBlocks - memBlocksStart) / sizeof(MemBlock);
}

inline void insertBlock(MemBlock block) {
    memBlocks--;
    memBlocks->nextBlock = memBlocks + 1;
    memBlocks++;
    block.previousBlock = memBlocks - 1;
    *memBlocks++ = block;
}

void initMemMgr() {
    // kdebug("Начата инициализация менеджера памяти.\n");
    // kdebug("Адрес начала блоков: %x.\n", memBlocks);
    createPages(memBlocksStart, 0x6000000, memMgrSize / PAGE_SIZE);
    memset((byte*)memBlocks, 0x100000, 0);

    MemBlock idPagedBlock
    {nullptr, 0x2000000, 0x0, 0x0, maxword, MEMBLOCK_FLAG_OCCUPIED, memBlocks + 1};
    *memBlocks++ = idPagedBlock;
    
    MemBlock kernelBlock
    {memBlocks - 1, 0x4000000, (byte*)0xC0000000, 0x2000000, PID_KERNEL, MEMBLOCK_FLAG_OCCUPIED, memBlocks + 1};
    *memBlocks++ = kernelBlock;

    MemBlock memMgrBlock
    {memBlocks - 1, memMgrSize, (byte*)0xB0000000, 0x6000000, PID_KERNEL, MEMBLOCK_FLAG_OCCUPIED, memBlocks + 1};
    *memBlocks++ = memMgrBlock;

    MemBlock videoMemBlock
    {memBlocks - 1, 0x4000000, (byte*)0xFC000000, bld->VBEInfo.FrameBuffer, maxword, MEMBLOCK_FLAG_OCCUPIED, nullptr};
    *memBlocks++ = videoMemBlock;

    word memMapC = bld->MemMapEntriesCount;

    // kdebug("Анализ разметки памяти.\n");
    MemMapEntry *entries = (MemMapEntry*)((byte*)bld + sizeof(BootLoaderData));
    word _memMapC = memMapC;
    for (word i = 0; i < memMapC - 1; i++) {
        if ((entries[i].base + entries[i].length == entries[i+1].base) && (entries[i].type == entries[i+1].type)) {
            // kdebug("Метки %d и %d можно объединить.\n", i+1, i+2);
            entries[i].length += entries[i+1].length;
            entries[i+1] = {};
            for (word j = i+1; j < memMapC - 1; j++) {
                entries[j] = entries[j+1];
            }
            entries[memMapC - 1] = {};
            _memMapC --;
        }
    }
    memMapC = _memMapC;

    // kdebug("Количество меток: %d.\n", memMapC);
    for (word i = 0; i < memMapC; i++) {
        // kdebug("Метка %d:\n", i+1);
        // kdebug("\tОснование: %X\n", entries[i].base);
        // kdebug("\tДлина: %D Б\n", entries[i].length);
        // kdebug("\tТип: %d\n", entries[i].type);
        // kdebug("\tРасширенные аттрибуты: %b\n", entries[i].exAttributes);

        if (entries[i].base >= 0x2000000 && entries[i].type != 1) {
            // kdebug("Метка %d требует отдельного блока.\n", i+1);
            MemBlock entryBlock
            {memBlocks - 1, entries[i].length, (byte*)entries[i].base, entries[i].base, maxword, MEMBLOCK_FLAG_OCCUPIED, nullptr};
            insertBlock(entryBlock);
        }
    }

    qword memory = entries[memMapC-1].base + entries[memMapC-1].length;
    // kdebug("Системе доступно %D МБ оперативной памяти.\n", (memory / 1024) / 1024);

    MemBlock freeMemBlock
    {memBlocks - 1, 0x8000000, (byte*)HEAP_VIRTADDR, HEAP_PHYSADDR, 0, 0, nullptr};
    MemBlock *ptr = (MemBlock*)memBlocksStart;
    while (ptr < memBlocks) {
        MemBlock block = *ptr++;

        if ((block.flags & MEMBLOCK_FLAG_OCCUPIED) == 0)
            continue;

        if (block.physaddr >= freeMemBlock.physaddr) {
            freeMemBlock.size = block.physaddr - freeMemBlock.physaddr;
            insertBlock(freeMemBlock);
            freeMemBlock.physaddr = block.physaddr + block.size;
            freeMemBlock.memPtr += freeMemBlock.size;
            freeMemBlock.memPtr += block.size;
        }

        if (freeMemBlock.memPtr >= (byte*)0xA000000) {
            break;
        }

        if (freeMemBlock.memPtr + freeMemBlock.size > (byte*)0xA000000) {
            freeMemBlock.size = 0xA0000000 - (qword)freeMemBlock.memPtr;
            insertBlock(freeMemBlock);
            break;
        }
    }

    // kdebug("Все блоки размещены.\n");
    MemBlock block = *(MemBlock*)memBlocksStart;
    word i = 1;
    while (true) {
        // kdebug("Блок %d:\n", i);
        // kdebug("\tПредыдущий блок: %x\n", block.previousBlock);
        // kdebug("\tЛогический адрес блока: %x\n", block.memPtr);
        // kdebug("\tФизический адрес блока: %X\n", block.physaddr);
        // kdebug("\tДлина блока: %D (%X) Б\n", block.size, block.size);
        // kdebug("\tPID владельца: %x\n", block.pid);
        // kdebug("\tЗанят ли? ");
        // kdebug((block.flags & MEMBLOCK_FLAG_OCCUPIED) ? "Да\n" : "Нет\n");
        // kdebug("\tСледующий блок: %x\n", block.nextBlock);
        if (block.nextBlock == nullptr)
            break;
        i ++;
        block = *(block.nextBlock);
    }

    // kdebug("Инициализация завершена.\n");
}

byte *kmalloc(dword amount, word pid) {
    // kdebug("Получен запрос на выделение %d Б для процесса %x.\n", amount, pid);

    MemBlock *ptr = (MemBlock*)memBlocksStart;
    while (ptr < memBlocks) {
        MemBlock block = *ptr++;
        if (block.flags & MEMBLOCK_FLAG_OCCUPIED)
            continue;

        if (block.size == amount) {
            // kdebug("Найден свободный блок.\n");
            // kdebug("Адрес: %x.\n", block.memPtr);
            block.flags |= MEMBLOCK_FLAG_OCCUPIED;
            block.pid = pid;
            *(--ptr) = block;
            createPages((dword)block.memPtr, block.physaddr, (block.size + PAGE_SIZE - 1) / PAGE_SIZE);
            return block.memPtr;
        }

        if (block.size > amount) {
            // kdebug("Найден свободный блок.\n");
            // kdebug("Адрес: %x.\n", block.memPtr);
            byte *ret = block.memPtr;
            MemBlock chop
            {memBlocks - 1, amount, block.memPtr, block.physaddr, pid, MEMBLOCK_FLAG_OCCUPIED, nullptr};
            insertBlock(chop);
            createPages((dword)block.memPtr, block.physaddr, (amount + PAGE_SIZE - 1) / PAGE_SIZE);
            ptr --;
            ptr->physaddr += amount;
            ptr->memPtr += amount;
            ptr->size -= amount;
            return ret;
        } 
    }
    // kdebug("ВНИМАНИЕ: Не было найдено подходящего блока\n");
    return nullptr;
}

byte* kmalloc(dword amount) {
    return kmalloc(amount, PID_KERNEL);
}

byte *kmallocPhys(dword virtAddr, dword amount, word pid) {
    dword pageAddr = virtAddr &= (~0xFFF);

    MemBlock *ptr = (MemBlock*)memBlocksStart;
    dword allocSize = ((amount + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    while (ptr < memBlocks) {
        MemBlock block = *ptr++;

        if (block.flags & MEMBLOCK_FLAG_OCCUPIED)
            continue;

        if ((dword)block.memPtr > pageAddr)
            continue;

        if ((dword)(block.memPtr + block.size) < pageAddr)
            continue;

        if (block.size < allocSize)
            continue;

        MemBlock freeLeftBlock = block;
        freeLeftBlock.size = pageAddr - (dword)block.memPtr;
        freeLeftBlock.previousBlock = nullptr;
        freeLeftBlock.nextBlock = nullptr;
        dword physAddr = freeLeftBlock.physaddr + freeLeftBlock.size;
        MemBlock freeRightBlock = block;
        dword rightStart = pageAddr + allocSize;
        freeRightBlock.size = (dword)freeRightBlock.memPtr + freeRightBlock.size - rightStart;
        freeRightBlock.memPtr = (byte*)pageAddr + allocSize;
        freeRightBlock.physaddr = physAddr + allocSize;
        freeRightBlock.previousBlock = nullptr;
        freeRightBlock.nextBlock = nullptr;
        ptr--;
        MemBlock dataBlock
        {ptr - 1, allocSize, (byte*)pageAddr, physAddr, pid, MEMBLOCK_FLAG_OCCUPIED, ptr->nextBlock};
        *(--ptr) = dataBlock;
        if (freeLeftBlock.size)
            insertBlock(freeLeftBlock);
        if (freeRightBlock.size)
            insertBlock(freeRightBlock);

        createPages(pageAddr, physAddr, allocSize / PAGE_SIZE);

        return (byte*)virtAddr;
    }
    return nullptr;
}

byte *kmallocPhys(dword virtAddr, dword amount) {
    return kmallocPhys(virtAddr, amount, PID_KERNEL);
}

void kfree(void *var, word pid) {
    // kdebug("Получен запрос на высвобождение переменной по адресу %x от процесса %x.\n", var, pid);
    MemBlock *ptr = (MemBlock*)memBlocksStart;

    while (ptr < memBlocks) {
        MemBlock block = *ptr++;
        
        if (block.pid != pid && pid != PID_KERNEL)
            continue;

        if (block.memPtr != var)
            continue;

        // kdebug("Найден нужный блок в памяти.\n");
        // kdebug("Размер блока: %d Б.\n", block.size);
        block.flags &= ~MEMBLOCK_FLAG_OCCUPIED;
        block.pid = 0;
        *(--ptr) = block;
        return;
    }
    // kdebug("ВНИМАНИЕ: Подходящий блок не был найден\n");
}

void kfree(void* var) {
    kfree(var, PID_KERNEL);
}

void logBlocks() {
    MemBlock block = *(MemBlock*)memBlocksStart;
    word i = 1;
    while (true) {
        kdebug("Блок %d:\n", i);
        kdebug("\tПредыдущий блок: %x\n", block.previousBlock);
        kdebug("\tЛогический адрес блока: %x\n", block.memPtr);
        kdebug("\tФизический адрес блока: %X\n", block.physaddr);
        kdebug("\tДлина блока: %D (%X) Б\n", block.size, block.size);
        kdebug("\tPID владельца: %x\n", block.pid);
        kdebug("\tЗанят ли? ");
        kdebug((block.flags & MEMBLOCK_FLAG_OCCUPIED) ? "Да\n" : "Нет\n");
        kdebug("\tСледующий блок: %x\n", block.nextBlock);
        if (block.nextBlock == nullptr)
            break;
        i ++;
        block = *(block.nextBlock);
    }
}

void refreshBlocks() {
    disableInts();

    MemBlock *ptr = (MemBlock*)memBlocksStart;
    dword memBlocksCount = getBlocksCount();

    for (dword i = 0; i < memBlocksCount; i++) {
        for (dword j = i+1; j < memBlocksCount; j++) {
            if (ptr[i].memPtr > ptr[j].memPtr) {
                MemBlock _block = ptr[i];
                ptr[i] = ptr[j];
                ptr[j] = _block;
            }
        }
    }

    dword deletedBlocks = 0;
    for (dword i = 0; i < memBlocksCount - 1; i++) {
        if (ptr[i].flags & MEMBLOCK_FLAG_OCCUPIED)
            continue;

        if (ptr[i+1].flags & MEMBLOCK_FLAG_OCCUPIED)
            continue;

        ptr[i].size += ptr[i+1].size;
        deletedBlocks ++;
        for (dword j = i+1; j < memBlocksCount - 1; j++) {
            ptr[j] = ptr[j+1];
        }
    }

    memBlocks -= deletedBlocks;
    memBlocksCount -= deletedBlocks;

    ptr[0].previousBlock = nullptr;
    ptr[0].nextBlock = ptr + 1;
    for (dword i = 1; i < memBlocksCount - 1; i ++) {
        ptr[i].previousBlock = ptr + i - 1;
        ptr[i].nextBlock = ptr + i + 1;
    }
    ptr[memBlocksCount-1].previousBlock = ptr + memBlocksCount - 1 - 1;
    ptr[memBlocksCount-1].nextBlock = nullptr;

    enableInts();
}