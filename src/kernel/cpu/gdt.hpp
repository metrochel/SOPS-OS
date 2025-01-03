#ifndef _GDT_INCL
#define _GDT_INCL
//
//  Работа с GDT
//
//  - Позволяет изменять Таблицу Глобальных Дескрипторов (ТГД или GDT).
//
#include "../util/nums.hpp"

#define tssEntry gdtr.base[9]

// ### GDT_Entry
// Эта структура описывает одну метку в GDT.
struct GDT_Entry {
    qword limitLo       : 16;       // Нижние 16 бит предела
    qword baseLo        : 24;       // Нижние 24 бита основания
    qword access        : 1;        // Флаг использованности
    qword readWrite     : 1;        // Флаг чтения-записи
    qword direction     : 1;        // Флаг направления
    qword code          : 1;        // Флаг кода
    qword codeDataSeg   : 1;        // Флаг сегмента кода или данных
    qword perms         : 2;        // Наименьшее кольцо полномочий, имеющее доступ к региону
    qword present       : 1;        // Флаг настоящности
    qword limitHi       : 4;        // Верхние 4 бита предела
    qword available     : 1;        // Флаг доступности
    qword longMode      : 1;        // Флаг длинного режима
    qword big           : 1;        // Флаг размерности региона
    qword granularity   : 1;        // Флаг зернистости
    qword baseHi        : 8;        // Верхние 8 бит основания
} __attribute__((packed));

// ### GDT_TSS
// Эта структура описывает один TSS (Task State Segment).
struct GDT_TSS {
    dword prevTSS;          // Селектор предыдущего TSS
    dword esp0;             // ESP в 0 кольце
    dword ss0;              // SS в 0 кольце
    dword esp1;             // ESP в 1 кольце
    dword ss1;              // SS в 1 кольце
    dword esp2;             // ESP в 2 кольце
    dword ss2;              // SS в 2 кольце
    dword cr3;              // Значение CR3
    dword eip;              // Значение EIP
    dword eflags;           // Значение EFLAGS
    dword eax;              // Значение EAX
    dword ecx;              // Значение ECX
    dword edx;              // Значение EDX
    dword ebx;              // Значение EBX
    dword esp;              // Значение ESP в 3 кольце
    dword ebp;              // Значение EBP
    dword esi;              // Значение ESI
    dword edi;              // Значение EDI
    dword es;               // Значение ES
    dword cs;               // Значение CS
    dword ss;               // Значение SS в 3 кольце
    dword ds;               // Значение DS
    dword fs;               // Значение FS
    dword gs;               // Значение GS
    dword ldt;              // Селектор, указывающий на LDT
    word trap;              // Ловушка (не используется)
    word iomapBase;         // (не зна- ой, то есть не используется)
} __attribute__((packed));

// ### GDTR
// Описывает структуру регистра GDTR.
struct GDTR {
    word size;
    GDT_Entry *base;
} __attribute__((packed));

extern GDT_TSS tss;
extern GDTR gdtr;

/// @brief Инициализирует GDT для работы с СОпС.
void initGDT();

/// @brief Устанавливает поле ESP0 внутри TSS.
inline void setESP0(dword esp) {
    tss.esp0 = esp;
}

/// @brief Устанавливает поле SS0 внутри TSS.
inline void setSS0(word ss) {
    tss.ss0 = ss;
}

#endif