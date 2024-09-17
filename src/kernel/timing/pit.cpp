#include "pit.hpp"
#include "../graphics/glyphs.hpp"

word computeCounter(dword mcs) {
    if (mcs < 50) {
        return mcs;
    } 
    dword count = 1193182 * (float)(mcs / 1000000);
    if (count >= 0x10000)
        return 0;
    return (word)count;
}

void setPITCounter(word count) {
    __asm__ ("cli");
    outb(PIT_MODE_CMD_PORT, PIT_CMD_CHAN0 | PIT_CMD_ACCESS_HILO | PIT_CMD_OPMODE_2 | PIT_CMD_BIN);

    outb(PIT_CHANNEL_0_PORT, count & 0xFF);
    outb(PIT_CHANNEL_0_PORT, (count & 0xFF00) >> 8);
    __asm__ ("sti");
}

void setPITTimer(dword microseconds) {
    word counter = computeCounter(microseconds);
    setPITCounter(counter);
}