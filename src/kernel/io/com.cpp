#include "com.hpp"
#include "io.hpp"

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
    *comWriteBuffers[port-1] = 0x0D;
    comWriteBuffers[port-1] ++;
    *comWriteBuffers[port-1] = 0x0A;
    comWriteBuffers[port-1] ++;
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
    outb(ioPort + 1, inb(ioPort+1) | COM_INT_TRA_EMPTY);
}