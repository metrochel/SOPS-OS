#include "com.hpp"
#include "io.hpp"
#ifndef STR_SIG
#include "../str/str.hpp"
#endif
#include <stdarg.h>

uint8_t *comReadBuffers[]  = {nullptr, nullptr, nullptr, nullptr};
uint8_t *comWriteBuffers[] = {nullptr, nullptr, nullptr, nullptr};
uint8_t *comBasePtrs[] = {nullptr, nullptr, nullptr, nullptr};

bool initPorts[] = {false,false,false,false};
uint16_t uart;

void setBaudRate(uint8_t port, uint32_t baudRate) {
    if (baudRate >= COM_MAX_BAUD_RATE)
        baudRate = COM_MAX_BAUD_RATE;

    uint16_t divisor = COM_MAX_BAUD_RATE / baudRate;
    uint16_t ioPort = getIOPort(port);

    uint8_t line_control_reg = inb(ioPort + 3);
    line_control_reg |= 0x80;
    outb(ioPort + 3, line_control_reg);

    outb(ioPort, (uint8_t)(divisor & 0xFF));
    outb(ioPort + 1, (uint8_t)((divisor >> 8) & 0xFF));

    line_control_reg &= 0x7F;
    outb(ioPort + 3, line_control_reg);
}

uint16_t getIOPort(uint8_t portNum) {
    switch (portNum) {
        case 1:  return 0x3F8;
        case 2:  return 0x2F8;
        case 3:  return 0x3E8;
        case 4:  return 0x2E8;
        default: return 0x3F8;
    }
}

void cleanComBuffers(uint8_t port) {
    uint8_t* comBase = (uint8_t*)getComBaseR(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase + i) = 0;
        *(comBase + i + 0x5000) = 0;
    }
    comReadBuffers[port-1] = comBase;
    comWriteBuffers[port-1] = comBase + 0x5000;
    comBasePtrs[port-1] = comBase + 0x5000;
}

void cleanComRBuffer(uint8_t port) {
    uint8_t *comBase = (uint8_t*)getComBaseR(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase+i) = 0;
    }
    comReadBuffers[port-1] = comBase;
}

void cleanComWBuffer(uint8_t port) {
    uint8_t *comBase = (uint8_t*)getComBaseW(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase+i) = 0;
    }
    comWriteBuffers[port-1] = comBase;
    comBasePtrs[port-1] = comBase;
}

inline uint32_t getComBaseR(uint8_t port) {
    return 0x20000 + 0xA000 * (port-1);
}

inline uint32_t getComBaseW(uint8_t port) {
    return 0x25000 + 0xA000 * (port-1);
}

bool initCom(uint8_t port) {
    uint16_t ioPort = getIOPort(port);

    outb(ioPort + 1, 0);

    setBaudRate(port, 38400);

    outb(ioPort + 3, COM_8_DATA_BITS | COM_1_STOP_BIT | COM_PARITY_NONE);

    outb(ioPort + 4, COM_MCR_REQTOSEND | COM_MCR_OUT1 | COM_MCR_OUT2 | COM_MCR_LOOP);

    outb(ioPort + 2, 0);
    if (uart >= 16550)
        outb(ioPort + 2, COM_FIFO_ENABLE | COM_FIFO_CLEAR_REC | COM_FIFO_CLEAR_TRA | COM_FIFO_INT_TRIG1);

    outb(ioPort, 0xAE);
    if (inb(ioPort) != 0xAE)
        return false;

    uint8_t* comBufBase = (uint8_t*)getComBaseR(port);
    comReadBuffers[port-1] = comBufBase;
    comBufBase += 0x5000;
    comWriteBuffers[port-1] = comBufBase;
    cleanComBuffers(port);
    initPorts[port-1] = true;
    outb(ioPort + 4, COM_MCR_DATATERMRDY | COM_MCR_REQTOSEND | COM_MCR_OUT2);
    outb(ioPort + 1, COM_INT_REC_DATA_RDY | COM_INT_REC_LINE | COM_INT_MODEM_STATUS);
    if (uart >= 16550)
        outb(ioPort + 2, COM_FIFO_ENABLE | COM_FIFO_INT_TRIG8);
    return true;
}

void identifyUART() {
    uint16_t ioPort = getIOPort(1);
    outb(ioPort + 1, 0);
    outb(ioPort + 2, 0xE7);
    uint8_t iir = inb(ioPort + 2);
    
    if (iir & (1 << 7)) {
        if (iir & (1 << 6)) {
            if (iir & (1 << 5))
                uart = 16750;
            else
                uart = 16551;
        } else
            uart = 16550;
        return;
    }

    outb(ioPort + 7, 0x69);
    if (inb(ioPort + 7) == 0x69)
        uart = 16450;
    else
        uart = 8250;
}

uint32_t comReadBufferLength(uint8_t port) {
    return (uint32_t)comReadBuffers[port-1]  - getComBaseR(port);
}

uint32_t comWriteBufferLength(uint8_t port) {
    return (uint32_t)comWriteBuffers[port-1] - (uint32_t)comBasePtrs[port-1];
}

void writeCom(const char str[], uint8_t port) {
    if (!initPorts[port-1])
        return;
    while (*str != 0) {
        if (*str == 0x0A) {
            *comWriteBuffers[port-1] = 0x0D;
            comWriteBuffers[port-1]++;
        }
        *comWriteBuffers[port-1] = *str;
        comWriteBuffers[port-1]++;
        str++;
    }
    enableTraInt(port);
}

void writeCom(uint8_t b, uint8_t port) {
    if (!initPorts[port-1])
        return;
    *comWriteBuffers[port-1] = b;
    comWriteBuffers[port-1] ++;
    enableTraInt(port);
}

void readCom(uint8_t port, uint16_t am, uint8_t* dst) {
    if (am > comReadBufferLength(port))
        return;
    dst += am;
    for (int i = 0; i < am; i++) {
        *dst = *comReadBuffers[port-1];
        comReadBuffers[port-1]--;
        dst--;
    }
}

void comIn(uint8_t port) {
    uint16_t ioPort = getIOPort(port);
    for (int i = 0; i < 8; i++) {
        *comReadBuffers[port-1] = inb(ioPort);
        comReadBuffers[port-1] ++;
    }
}

void comSend(uint8_t port) {
    uint16_t ioPort = getIOPort(port);
    outb(ioPort, *comBasePtrs[port-1]);
    comBasePtrs[port-1] ++;
    if (uart >= 16551) {
        for (uint8_t i = 0; i < 7 && comWriteBufferLength(port) > 0; i++) {
            outb(ioPort, *comBasePtrs[port-1]);
            comBasePtrs[port-1] ++;
        }
    }
    if (!comWriteBufferLength(port)) {
        disableTraInt(port);
        cleanComWBuffer(port);
    }
}

inline void disableTraInt(uint8_t port) {
    uint16_t ioPort = getIOPort(port);
    outb(ioPort + 1, inb(ioPort+1) & ~COM_INT_TRA_EMPTY);
}

inline void enableTraInt(uint8_t port) {
    uint16_t ioPort = getIOPort(port);
    if (comWriteBufferLength(port))
        outb(ioPort + 1, inb(ioPort+1) | COM_INT_TRA_EMPTY);
}

void writeComBinUInt(uint32_t num, uint8_t port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'b';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    uint32_t mask = 1;
    uint8_t digits = 0;
    while (mask <= num && mask < (uint32_t)(1 << 31)) {
        mask <<= 1;
        digits ++;
    }
    mask >>= 1;
    digits --;
    *comWriteBuffers[port-1]++ = '0';
    *comWriteBuffers[port-1]++ = 'b';
    while (mask > 0) {
        if (num & mask)
            *comWriteBuffers[port-1]++ = '1';
        else
            *comWriteBuffers[port-1]++ = '0';
        mask >>= 1;
    }
}

void writeComOctUInt(uint32_t num, uint8_t port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'o';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    uint32_t mask = 7;
    uint8_t digits = 1;
    while (mask < num && mask < (uint32_t)(1 << 31)) {
        mask <<= 3;
        digits ++;
    }
    if (!(num & mask))
        mask >>= 3;
    *comWriteBuffers[port-1]++ = '0';
    *comWriteBuffers[port-1]++ = 'o';
    while (mask > 0) {
        uint32_t digit = num & mask;
        while (digit > 7)
            digit >>= 3;
        *comWriteBuffers[port-1]++ = digit + 0x30;
        mask >>= 3;
    }
}

void writeComDecUInt(uint32_t num, uint8_t port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    uint8_t digits = 0;
    uint32_t numclone = num;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    comWriteBuffers[port-1] += digits - 1;
    for (uint8_t i = 0; i < digits; i++) {
        *comWriteBuffers[port-1]-- = num % 10 + 0x30;
        num /= 10;
    }
    comWriteBuffers[port-1] += digits + 1;
}

void writeComHexUInt(uint32_t num, uint8_t port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'x';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    uint32_t mask = 0xF;
    uint8_t digits = 1;
    while (mask < num && mask < (uint32_t)(1 << 31)) {
        mask <<= 4;
        digits ++;
    }
    if (!(num & mask))
        mask >>= 4;
    *comWriteBuffers[port-1]++ = '0';
    *comWriteBuffers[port-1]++ = 'x';
    while (mask > 0) {
        uint32_t digit = num & mask;
        while (digit > 0xF)
            digit >>= 4;
        if (digit < 10)
            *comWriteBuffers[port-1]++ = digit + 0x30;
        else
            *comWriteBuffers[port-1]++ = digit - 10 + 0x41;
        mask >>= 4;
    }
}

void kdebug(const char* text, ...) {
    if (!initPorts[0])
        return;
    uint32_t len = strlen((char*)text);
    if (comWriteBufferLength(1) + len > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + len > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    va_list l;
    va_start(l, text);
    uint8_t state;
    while (*text != 0) {
        char c = *text;
        if (c == '%') {
            c = *(++text);
            if (c == 'd')
                state = 10;
            else if (c == 'x')
                state = 16;
            else if (c == 'o')
                state = 8;
            else if (c == 'b')
                state = 2;
            else
                state = 0;
        }
        else
            state = 0;
        uint32_t arg;
        switch (state) {
            case 0:
                if (c == 0x0A)
                    *comWriteBuffers[0]++ = 0x0D;
                *comWriteBuffers[0]++ = c;
                break;
            case 2:
                arg = va_arg(l, unsigned int);
                writeComBinUInt(arg, 1);
                break;
            case 8:
                arg = va_arg(l, unsigned int);
                writeComOctUInt(arg, 1);
                break;
            case 10:
                arg = va_arg(l, unsigned int);
                writeComDecUInt(arg, 1);
                break;
            case 16:
                arg = va_arg(l, unsigned int);
                writeComHexUInt(arg, 1);
                break;
        }
        text ++;
    }
    enableTraInt(1);
}