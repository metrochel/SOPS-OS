#include "com.hpp"
#include "io.hpp"

uint8_t comBuffer[256];

void setBaudRate(uint8_t portNum, uint32_t baudRate) {
    if (baudRate >= MAX_BAUD_RATE)
        baudRate = MAX_BAUD_RATE;

    uint16_t divisor = MAX_BAUD_RATE / baudRate;
    uint16_t ioPort = getIOPort(portNum);

    uint8_t line_control_reg = inb(ioPort + 3);
    line_control_reg |= 0x80;
    outb(ioPort + 3, line_control_reg);

    outb(ioPort, (uint8_t)(divisor & 0xFF));
    outb(ioPort + 1, (uint8_t)((divisor >> 8) & 0xFF));

    line_control_reg &= 0x7F;
    outb(ioPort + 3, line_control_reg);
}

uint16_t getIOPort(uint8_t portNum) {
    if (portNum == 1 or portNum == 3)
        return 0x2F8;
    if (portNum == 2 or portNum == 4)
        return 0x3F8;
    if (portNum == 5 or portNum == 7)
        return 0x5F8;
    if (portNum == 6 or portNum == 8)
        return 0x4F8;
}