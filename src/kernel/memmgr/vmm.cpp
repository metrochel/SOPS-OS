#include "vmm.hpp"
#include "paging.hpp"
#include "pmm.hpp"
#include "../io/com.hpp"
#include "../util/util.hpp"
#include "../run/process.hpp"
#include "../dbg/dbg.hpp"

const dword tablesCount = 1024;

TableInfo *tables = nullptr;
MemBlock *blocksBase = nullptr;
MemBlock *blocks = blocksBase;

MemBlock *firstFree = blocksBase;

MemBlock* insertBlock(MemBlock block) {
    *firstFree = block;
    MemBlock *ret = firstFree;
    if (!tables[block.base >> 22].firstBlock) {
        tables[block.base >> 22].firstBlock = blocks;
    }

    while (firstFree->base && firstFree < blocks)
        firstFree++;
    
    if (firstFree == blocks)
        firstFree = ++blocks;
    
    return ret;
}

void allocatePage(ptrint vaddr, word pid) {
    ptrint paddr = allocatePageFrame();
    createPage(vaddr, paddr);
    setPagePermsLevel(vaddr, pid == PID_KERNEL ? 0 : 3);
}

void initVMM(byte *&memMgrPtr) {
    kdebug("Начата инициализация менеджера виртуальной памяти.\n");

    tables = (TableInfo*)memMgrPtr;
    memset(tables, sizeof(TableInfo) * tablesCount, 0);
    kdebug("Разметка таблиц установлена на адрес %x.\n", tables);
    memMgrPtr += sizeof(TableInfo) * tablesCount;

    blocksBase = (MemBlock*)memMgrPtr;
    blocks = blocksBase;
    firstFree = blocksBase;
    kdebug("Разметка блоков установлена на адрес %x.\n", blocksBase);

    kdebug("Инициализация менеджера виртуальной памяти завершена.\n");

    for (word i = 0; i < 8; i++) {
        TableInfo idPageTable
        {maxword, true, blocks, blocks, PAGE_TABLE_COUNT};
        tables[i] = idPageTable;
        MemBlock idBlock
        {nullptr, i << 22, PAGE_TABLE_SIZE, true, nullptr};
        insertBlock(idBlock);
    }

    TableInfo kernelTable
    {PID_KERNEL, true, blocks, blocks + 1, PAGE_TABLE_COUNT};
    tables[(0xC0000000 >> 22)] = kernelTable;
    
    MemBlock kernelBlock
    {nullptr, 0xC0000000, 0x100000, true, blocks + 1};
    insertBlock(kernelBlock);
    MemBlock kernelFreeBlock
    {blocks - 1, 0xC0100000, PAGE_TABLE_SIZE - 0x100000, false, nullptr};
    insertBlock(kernelFreeBlock);

    TableInfo memMgrTable
    {maxword, true, blocks, blocks, PAGE_TABLE_COUNT};
    tables[(0xB0000000 >> 22)] = memMgrTable;

    MemBlock memMgrBlock
    {nullptr, 0xB0000000, PAGE_TABLE_SIZE, true, nullptr};
    insertBlock(memMgrBlock);

    kdebug("Инициализация менеджера виртуальной памяти завершена.\n");
}

dword allocatePageTable(word pid, dword start) {
    for (dword i = start + 1; i < tablesCount; i++) {
        TableInfo table = tables[i];
        if (table.occupied)
            continue;

        ptrint baseAddr = i << 22;

        ptrint startPage = allocatePageFrame();
        createPage(baseAddr, startPage);
        setPagePermsLevel(baseAddr, pid == PID_KERNEL ? 0 : 3);
        table.occupied = true;
        table.pagesCount = 1;
        table.pid = pid;
        MemBlock block = {nullptr, baseAddr, PAGE_SIZE, false, nullptr};
        table.firstBlock = insertBlock(block);
        table.lastBlock = table.firstBlock;
        tables[i] = table;

        return i;
    }

    return 0;
}

dword allocatePageTable(word pid) {
    return allocatePageTable(pid, 0);
}

dword allocatePageTables(dword start, dword len, word pid) {
    for (dword i = start + 1; i < tablesCount - len; i++) {
        if (tables[i].occupied)
            continue;

        bool cont = false;
        for (dword j = 0; j < len; j++) {
            if (tables[i+j].occupied) {
                i += j;
                cont = true;
                break;
            }
        }
        if (cont)
            continue;

        ptrint baseAddr = i << 22;

        ptrint startPage = allocatePageFrame();
        createPage(baseAddr, startPage);
        setPagePermsLevel(baseAddr, pid == PID_KERNEL ? 0 : 3);
        tables[i].occupied = true;
        tables[i].pagesCount = 1;
        tables[i].pid = pid;
        MemBlock block = {nullptr, baseAddr, PAGE_SIZE, false, nullptr};
        tables[i].firstBlock = insertBlock(block);
        tables[i].lastBlock = tables[i].firstBlock;
        for (dword j = 1; j < len; j++)
            tables[i+j] = tables[i];

        return i;
    }
    return 0;
}

byte *virtAlloc(dword amt, word pid) {
    kdebug("Получен запрос на выделение %d Б для процесса %d.\n", amt, pid);
    
    for (dword i = 0; i < tablesCount; i++) {
        if (tables[i].pid != pid)
            continue;

        ptrint tableBase = i << 22;
        kdebug("Анализ таблицы %d (%x).\n", i, tableBase);

        kdebug("Адрес первого блока: %x.\n", tables[i].firstBlock);

        MemBlock *block = tables[i].firstBlock;
        MemBlock *_block = block;
        ptrint maxBase = tableBase;
        MemBlock *maxBlock = nullptr;
        ptrint maxPageAddr = tableBase;
        while (block) {
            if (block->base + block->size > maxPageAddr)
                maxPageAddr = block->base + block->size;

            if (block->occupied) {
                _block = block;
                block = block->nextBlock;
                continue;
            }

            if (!block->nextBlock) {
                maxBase = block->base;
                maxBlock = block;
            }

            if (block->size < amt) {
                _block = block;
                block = block->nextBlock;
                continue;
            }

            if (block->size == amt) {
                kdebug("Найден подходящий блок.\n");
                kdebug("Адрес = %x.\n", block->base);
                block->occupied = 1;
                return (byte*)block->base;
            }

            MemBlock curBlock = *block;
            curBlock.base += amt;
            curBlock.size -= amt;
            curBlock.prevBlock = block;
            curBlock.nextBlock = block->nextBlock;

            MemBlock newBlock
            {block->prevBlock, block->base, amt, 1, nullptr};
            *block = newBlock;
            block->nextBlock = insertBlock(curBlock);

            if (!curBlock.nextBlock)
                tables[i].lastBlock = block->nextBlock;

            kdebug("Найден подходящий блок.\n");
            kdebug("Адрес = %x.\n", block->base);

            return (byte*)newBlock.base;
        }
        block = _block;
        maxPageAddr = (maxPageAddr + PAGE_SIZE - 1) / PAGE_SIZE;

        if (tables[i].pagesCount > PAGE_TABLE_COUNT) {
            dword skip = (tables[i].pagesCount + PAGE_TABLE_COUNT - 1) / PAGE_TABLE_COUNT;
            i += skip - 1;
            continue;
        }

        kdebug("Требуется расширить виртуальное пространство.\n");
        ptrint nextMax = maxBase + amt;

        if (nextMax < tableBase) {
            kdebug("ОШИБКА: Произошло переполнение адресов\n");
            kdebug("Возможно, память истощена или запрошено слишком много памяти.\n");
            return nullptr;
        }

        ptrint nextPage = maxPageAddr;
        kdebug("Адрес новой страницы: %x.\n", nextPage);
        if (maxBase == tableBase) {
            maxBase = nextPage;
        }

        if (nextMax > tableBase + PAGE_TABLE_SIZE) {
            kdebug("%d\n", (amt - (nextPage - maxBase) + PAGE_TABLE_SIZE - 1) / PAGE_TABLE_SIZE);
            dword neededTables = (amt - (nextPage - maxBase) + PAGE_TABLE_SIZE - 1) / PAGE_TABLE_SIZE;
            kdebug("Необходимо добавить %d таблиц страниц.\n", neededTables);
            dword newTables = allocatePageTables(i, neededTables, pid);
            if (!newTables) {
                kdebug("ОШИБКА: Не удалось выделить таблицы\n");
                return nullptr;
            }

            if (newTables != i + 1) {
                nextPage = newTables << 22;
                maxBase = newTables << 22;
            }
        }

        dword neededPages = ((amt - (nextPage - maxBase)) + PAGE_SIZE - 1) / PAGE_SIZE;
        kdebug("Потребуется выделить %d страниц.\n", neededPages);
        for (dword j = 0; j < neededPages; j++) {
            allocatePage(nextPage + j * PAGE_SIZE, pid);
        }

        MemBlock newBlock
        {block, nextPage + amt, PAGE_SIZE * neededPages - amt, false, nullptr};
        MemBlock varBlock
        {block, nextPage, amt, true, nullptr};

        block->nextBlock = insertBlock(varBlock);
        block = block->nextBlock;
        block->nextBlock = insertBlock(newBlock);

        tables[i].pagesCount += neededPages;

        return (byte*)nextPage;
    }

    kdebug("ВНИМАНИЕ: Подходящий блок не был найден\n");
    return nullptr;
}

byte *virtAlloc(ptrint offset, dword amt, word pid) {
    kdebug("Получен запрос на выделение %d Б памяти по адресу %x для процесса %d.\n", amt, offset, pid);

    dword pageTableNo = (ptrint)offset >> 22;
    ptrint tableBase = offset & (~0x3FFFFF);
    kdebug("Номер таблицы с переменной: %d.\n", pageTableNo);

    if (tables[pageTableNo].occupied && tables[pageTableNo].pid != pid && pid != PID_KERNEL) {
        kdebug("ОШИБКА: Пространство не принадлежит процессу %d\n");
        kdebug("Пространство принадлежит процессу %d.\n", tables[pageTableNo].pid);
        return nullptr;
    }

    if (!tables[pageTableNo].occupied) {
        tables[pageTableNo].occupied = true;
        tables[pageTableNo].pid = pid;
    }

    MemBlock *block = tables[pageTableNo].firstBlock;
    MemBlock *lastFree = nullptr;
    ptrint maxPagedAddr = tableBase;
    kdebug("Анализ блоков.\n");
    while (block) {
        if (block->base + block->size > maxPagedAddr)
            maxPagedAddr = block->base + block->size;

        if (block->occupied) {
            block = block->nextBlock;
            continue;
        }

        lastFree = block;

        if (offset < block->base || offset + amt > block->base + block->size) {
            block = block->nextBlock;
            continue;
        }

        if (block->size == amt) {
            kdebug("Найден подходящий блок.\n");
            kdebug("Адрес = %x.\n", offset);
            block->occupied = 1;
            return (byte*)offset;
        }

        MemBlock freeBlock0 = *block;

        MemBlock freeBlock1 = *block;
        MemBlock *followAddr = freeBlock1.nextBlock;
        freeBlock1.size = offset - freeBlock1.base;
        if (freeBlock1.size) {
            *block = freeBlock1;
        }

        MemBlock varBlock
        {block, offset, amt, true, followAddr};
        block->nextBlock = insertBlock(varBlock);
        block = block->nextBlock;

        MemBlock freeBlock2
        {block, offset + amt, freeBlock0.size - freeBlock1.size - amt, false, followAddr};
        if (freeBlock2.size) {
            block->nextBlock = insertBlock(freeBlock2);
            block = block->nextBlock;
        }

        kdebug("Найден подходящий блок.\n");
        kdebug("Адрес = %x.\n", offset);
        return (byte*)offset;
    }

    kdebug("Требуется расширить виртуальное пространство.\n");
    
    ptrint nextMax = ((maxPagedAddr + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    kdebug("Следующая страница: %x.\n", nextMax);

    ptrint offsetPage = offset & (~0xFFF);
    kdebug("Адрес первой страницы данных: %x.\n", offsetPage);
    dword neededPages = ((offsetPage - nextMax) >> 12) + ((amt + PAGE_SIZE - 1) / PAGE_SIZE);
    kdebug("Нужно выделить %d страниц.\n", neededPages);

    if (((nextMax >> 12) & 0x3FF) + neededPages > PAGE_TABLE_COUNT) {
        dword neededTables = ((nextMax >> 12) & 0x3FF + neededPages + PAGE_TABLE_COUNT - 1) / PAGE_TABLE_COUNT;
        kdebug("Нужно выделить %d таблиц страниц.\n", neededTables);
        ptrint allocatedTables = allocatePageTables(offset, neededTables, pid);
        if (allocatedTables >> 22 != (offset >> 22) + 1) {
            kdebug("ОШИБКА: Таблицы выделены по неверному адресу\n");
            kdebug("Ожидался адрес %x, но получен %x.\n", (offset >> 22) + 1, allocatedTables >> 22);
            *tables[allocatedTables].firstBlock = {};
            for (dword i = 0; i < neededTables; i++) {
                tables[allocatedTables + i] = {};
            }
            return nullptr;
        }
    }

    for (dword i = 0; i < neededPages; i++) {
        allocatePage(nextMax + i * PAGE_SIZE, pid);
    }

    if (!tables[pageTableNo].firstBlock) {
        MemBlock firstBlock
        {nullptr, tableBase, offset - tableBase, false, nullptr};
        block = insertBlock(firstBlock);
        tables[pageTableNo].firstBlock = block;
        tables[pageTableNo].lastBlock = block;
    }

    if (lastFree == block) {
        magicBreakpoint();
        lastFree->size = offset - (lastFree->base + lastFree->size);
    }

    MemBlock varBlock
    {block, offset, amt, true, nullptr};

    block->nextBlock = insertBlock(varBlock);
    block = block->nextBlock;

    MemBlock remBlock
    {block, offset + amt, PAGE_SIZE - (offset + amt) % PAGE_SIZE,  false, nullptr};
    if (remBlock.size) {
        block->nextBlock = insertBlock(remBlock);
    }

    return (byte*)offset;
}

dword virtFree(void *var, word pid) {
    kdebug("Получен запрос от процесса %d на освобождение переменной по адресу %x.\n", pid, var);

    dword pageTableNo = (ptrint)var >> 22;
    kdebug("Номер таблицы с переменной: %d.\n", pageTableNo);

    if (tables[pageTableNo].occupied && tables[pageTableNo].pid != pid && pid != PID_KERNEL) {
        kdebug("ОШИБКА: Переменная не принадлежит процессу %d\n", pid);
        kdebug("Переменная принадлежит процессу %d.\n", tables[pageTableNo].pid);
        return 0;
    }

    MemBlock *block = tables[pageTableNo].firstBlock;
    while (block) {
        if (block->base == (ptrint)var) {
            kdebug("Найден нужный блок.\n");
            kdebug("Освобождается %d Б.\n", block->size);
            dword freed = block->size;
            block->occupied = false;
            block = block->prevBlock;
            MemBlock *nextBlock = block->nextBlock;
            if (block->occupied) {
                block = block->nextBlock;
                nextBlock = nextBlock->nextBlock;
            }
            while (nextBlock && !nextBlock->occupied && block->base + block->size == nextBlock->base) {
                block->size += nextBlock->size;
                block->nextBlock = nextBlock->nextBlock;
                nextBlock->base = 0;
                nextBlock = nextBlock->nextBlock;
                if (firstFree < nextBlock)
                    firstFree = nextBlock;
                if (nextBlock->nextBlock == nullptr)
                    break;
            }
            nextBlock->prevBlock = block;
            kdebugwait();
            return freed;
        }
        if (block->nextBlock == nullptr)
            break;
        block = block->nextBlock;
    }
    return 0;
}

void destroyPageTable(dword table) {
    MemBlock *block = tables[table].firstBlock;
    if (block == nullptr)
        return;

    firstFree = block;
    while (block) {
        block->base = null;
        if (block->nextBlock == nullptr)
            break;
        block = block->nextBlock;
    }

    ptrint addr = table << 22;
    for (dword i = 0; i < PAGE_TABLE_SIZE; i += 0x1000) {
        ptrint physAddr = getPhysAddr(addr + i);
        freePageFrame(physAddr);
        removePage(addr + i);
    }

    tables[table] = {};
}

dword getVarSz(void *var) {
    dword pageTable = (ptrint)var >> 22;

    MemBlock *block = tables[pageTable].firstBlock;
    while (block) {
        if (block->base == (ptrint)var && block->occupied)
            return block->size;

        block = block->nextBlock;
    }
}

void logBlocks(dword pageTable) {
    kdebug("Таблица %d (%x):\n", pageTable, pageTable << 22);
    MemBlock *block = tables[pageTable].firstBlock;
    dword i = 1;
    while (block) {
        kdebug("Блок %d:\n", i);
        kdebug("\tПредыдущий блок: %x\n", block->prevBlock);
        kdebug("\tОснование: %x\n", block->base);
        kdebug("\tРазмер: %d Б (%x)\n", block->size, block->size);
        kdebug("\tЗанят ли? ");
        kdebug(block->occupied ? "Да\n" : "Нет\n");
        kdebug("\tСледующий блок: %x\n", block->nextBlock);
        i ++;
        block = block->nextBlock;
    }
}