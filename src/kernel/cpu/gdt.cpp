#include "gdt.hpp"
#include "../graphics/glyphs.hpp"
#include "../util/util.hpp"
#include "../dbg/dbg.hpp"

GDT_TSS tss = {};

GDTR gdtr;

void initGDT() {
    disableInts();
    __asm__ (
        "sgdtl %d0;"
        : "=m"(gdtr)
        :
        :
    );

    dword base = (dword)&tss;
    dword limit = sizeof(tss);
    tssEntry.baseLo = base & 0xFFFFFF;
    tssEntry.baseHi = base >> 24;
    tssEntry.limitLo = limit & 0xFFFF;
    tssEntry.limitHi = limit >> 16;
    tssEntry.access = 1;
    tssEntry.readWrite = 0;
    tssEntry.direction = 0;
    tssEntry.code = 1;
    tssEntry.codeDataSeg = 0;
    tssEntry.perms = 0;
    tssEntry.present = 1;
    tssEntry.available = 0;
    tssEntry.longMode = 0;
    tssEntry.big = 0;
    tssEntry.granularity = 0;
    gdtr.size = sizeof(GDT_Entry) * 10 - 1;

    __asm__ (
        "lgdtl %d0"
        :
        : "m"(gdtr)
        :
    );

    memset(&tss, sizeof(GDT_TSS), 0);
    word ss0;
    __asm__ (
        "movw %%ss, %w0;"
        "movw $0x48, %%ax;"
        "ltr  %%ax"
        : "=m"(ss0)
        :
        :
    );
    tss.ss0 = ss0;
    enableInts();
}