#include "pit.hpp"

uint16_t computeCounter(uint32_t mcs) {
    uint32_t count = 1193182 * (float)(mcs / 1000000);
    if (count >= 0x10000)
        return 0;
    return (uint16_t)count;
}

void setPITCounter(uint16_t count) {
    __asm__ ("cli");
    outb(PIT_MODE_CMD_PORT, PIT_CMD_CHAN0 | PIT_CMD_ACCESS_HILO | PIT_CMD_OPMODE_2);

    outb(PIT_CHANNEL_0_PORT, count & 0xFF);
    outb(PIT_CHANNEL_0_PORT, (count & 0xFF00) >> 8);
    __asm__ ("sti");
}

void setPITTimer(uint32_t microseconds) {
    uint16_t counter = computeCounter(microseconds);
    setPITCounter(counter);
}