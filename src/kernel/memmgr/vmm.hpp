#ifndef _VMM_INCL
#define _VMM_INCL
//
//  Менеджер виртуальной памяти
//
//  - Раздаёт виртуальную память процессам.
//
#include "../util/nums.hpp"

// ### MemBlock
// Эта структура описывает один блок виртуальной памяти.
struct MemBlock {
    MemBlock *prevBlock;
    ptrint base;
    ptrint size;
    bool occupied;
    MemBlock *nextBlock;
};

// ### TableInfo
// Эта структура описывает данные об одной таблице страниц.
struct TableInfo {
    word pid;
    bool occupied;
    MemBlock *firstBlock;
    MemBlock *lastBlock;
    word pagesCount;
};

extern TableInfo *tables;

/// @brief Инициализирует виртуальный менеджер памяти.
/// @param memMgrPtr Указатель на данные виртуального менеджера
void initVMM(byte *&memMgrPtr);

/// @brief Выделяет для процесса одну таблицу страниц в памяти.
/// @param pid PID процесса-получателя
/// @return Номер выделенной страницы
/// @note Если выделение провалено, возвращается 0.
dword allocatePageTable(word pid);

/// @brief Выделяет для процесса место в виртуальной памяти.
/// @param amt Длина нужного блока, Б
/// @param pid PID процесса-получателя
byte *virtAlloc(dword amt, word pid);

/// @brief Освобождает ресурсы, занятые таблицей.
/// @param table Номер таблицы
void destroyPageTable(dword table);

/// @brief Выделяет для процесса место в виртуальной памяти
/// @param offset Адрес необходимого блока
/// @param amt Длина блока, Б
/// @param pid PID процесса-получателя
byte *virtAlloc(ptrint offset, dword amt, word pid);

/// @brief Освобождает место, занимаемое переменной в виртуальной памяти.
/// @param var Указатель на переменную в памяти
/// @param pid PID процесса-владельца
/// @return Размер освобождённого блока, Б
dword virtFree(void *var, word pid);

/// @brief Определяет размер переменной.
/// @param var Указатель на переменную
/// @return Размер переменной в байтах
dword getVarSz(void *var);

void logBlocks(dword pageTable);


#endif