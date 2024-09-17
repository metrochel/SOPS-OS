#include "com.hpp"
#include "io.hpp"
#include "../graphics/glyphs.hpp"
#include "../str/str.hpp"
#include "../util/util.hpp"
#include <stdarg.h>

byte *comReadBuffers[]  = {nullptr, nullptr, nullptr, nullptr};
byte *comWriteBuffers[] = {nullptr, nullptr, nullptr, nullptr};
byte *comBasePtrs[] = {nullptr, nullptr, nullptr, nullptr};

bool initPorts[] = {false,false,false,false};
bool traIntActive[] = {false,false,false,false};
word uart;
bool debugActive = false;

void setBaudRate(byte port, dword baudRate) {
    if (baudRate >= COM_MAX_BAUD_RATE)
        baudRate = COM_MAX_BAUD_RATE;

    word divisor = COM_MAX_BAUD_RATE / baudRate;
    word ioPort = getIOPort(port);

    byte line_control_reg = inb(ioPort + 3);
    line_control_reg |= 0x80;
    outb(ioPort + 3, line_control_reg);

    outb(ioPort, (byte)(divisor & 0xFF));
    outb(ioPort + 1, (byte)((divisor >> 8) & 0xFF));

    line_control_reg &= 0x7F;
    outb(ioPort + 3, line_control_reg);
}

word getIOPort(byte portNum) {
    switch (portNum) {
        case 1:  return 0x3F8;
        case 2:  return 0x2F8;
        case 3:  return 0x3E8;
        case 4:  return 0x2E8;
        default: return 0x3F8;
    }
}

void cleanComBuffers(byte port) {
    byte* comBase = (byte*)getComBaseR(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase + i) = 0;
        *(comBase + i + 0x5000) = 0;
    }
    comReadBuffers[port-1] = comBase;
    comWriteBuffers[port-1] = comBase + 0x5000;
    comBasePtrs[port-1] = comBase + 0x5000;
}

void cleanComRBuffer(byte port) {
    byte *comBase = (byte*)getComBaseR(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase+i) = 0;
    }
    comReadBuffers[port-1] = comBase;
}

void cleanComWBuffer(byte port) {
    byte *comBase = (byte*)getComBaseW(port);
    for (int i = 0; i < 0x5000; i++) {
        *(comBase+i) = 0;
    }
    comWriteBuffers[port-1] = comBase;
    comBasePtrs[port-1] = comBase;
}

inline dword getComBaseR(byte port) {
    return 0x20000 + 0xA000 * (port-1);
}

inline dword getComBaseW(byte port) {
    return 0x25000 + 0xA000 * (port-1);
}

bool initCom(byte port) {
    word ioPort = getIOPort(port);

    outb(ioPort + 1, 0);

    setBaudRate(port, 57600);

    outb(ioPort + 3, COM_8_DATA_BITS | COM_1_STOP_BIT | COM_PARITY_NONE);

    outb(ioPort + 4, COM_MCR_REQTOSEND | COM_MCR_OUT1 | COM_MCR_OUT2 | COM_MCR_LOOP);

    outb(ioPort + 2, 0);
    if (uart >= 16550)
        outb(ioPort + 2, COM_FIFO_ENABLE | COM_FIFO_CLEAR_REC | COM_FIFO_CLEAR_TRA | COM_FIFO_INT_TRIG1);

    outb(ioPort, 0xAE);
    for (byte i = 0; i < 0x10; i++)
        io_wait();
    if (inb(ioPort) != 0xAE) {
        return false;
    }

    byte* comBufBase = (byte*)getComBaseR(port);
    comReadBuffers[port-1] = comBufBase;
    comBufBase += 0x5000;
    comWriteBuffers[port-1] = comBufBase;
    comBasePtrs[port-1] = comBufBase;
    cleanComBuffers(port);
    initPorts[port-1] = true;
    outb(ioPort + 4, COM_MCR_DATATERMRDY | COM_MCR_REQTOSEND | COM_MCR_OUT2);
    outb(ioPort + 1, COM_INT_REC_DATA_RDY | COM_INT_REC_LINE | COM_INT_MODEM_STATUS);
    if (uart >= 16550)
        outb(ioPort + 2, COM_FIFO_ENABLE | COM_FIFO_INT_TRIG8);
    if (port == 1)
        debugActive = true;
    return true;
}

void identifyUART() {
    word ioPort = getIOPort(1);
    outb(ioPort + 1, 0);
    outb(ioPort + 2, 0xE7);
    byte iir = inb(ioPort + 2);
    
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

dword comReadBufferLength(byte port) {
    return (dword)comReadBuffers[port-1]  - getComBaseR(port);
}

dword comWriteBufferLength(byte port) {
    return (dword)comWriteBuffers[port-1] - (dword)comBasePtrs[port-1];
}

void writeCom(const char str[], byte port) {
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

void writeCom(byte b, byte port) {
    if (!initPorts[port-1])
        return;
    while (comWriteBufferLength(port) >= 0x9FFF) {__asm__ ("nop");} 
    *comWriteBuffers[port-1] = b;
    comWriteBuffers[port-1] ++;
    enableTraInt(port);
}

void readCom(byte port, word am, byte* dst) {
    if (am > comReadBufferLength(port))
        return;
    dst += am;
    for (int i = 0; i < am; i++) {
        *dst = *comReadBuffers[port-1];
        comReadBuffers[port-1]--;
        dst--;
    }
}

void comIn(byte port) {
    word ioPort = getIOPort(port);
    for (int i = 0; i < 8; i++) {
        *comReadBuffers[port-1] = inb(ioPort);
        comReadBuffers[port-1] ++;
    }
}

void comSend(byte port) {
    word ioPort = getIOPort(port);
    outb(ioPort, *comBasePtrs[port-1]);
    comBasePtrs[port-1] ++;
    if (uart >= 16551) {
        for (byte i = 0; i < 7 && comWriteBufferLength(port) > 0; i++) {
            outb(ioPort, *comBasePtrs[port-1]);
            comBasePtrs[port-1] ++;
            if (!comWriteBufferLength(port))
                break;
        }
    }
    if (!comWriteBufferLength(port)) {
        disableTraInt(port);
        cleanComWBuffer(port);
    }
}

inline void disableTraInt(byte port) {
    if (!traIntActive[port-1]) return;
    disableInts();
    word ioPort = getIOPort(port);
    outb(ioPort + 1, 0b1101);
    traIntActive[port-1] = false;
    enableInts();
}

inline void enableTraInt(byte port) {
    if (traIntActive[port-1]) return;
    disableInts();
    word ioPort = getIOPort(port);
    if (comWriteBufferLength(port)) {
        outb(ioPort + 1, 0b1111);
        traIntActive[port-1] = true;
    }
    enableInts();
}

void writeComBinUInt(qword num, byte port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'b';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    qword mask = 1;
    byte digits = 0;
    while (mask <= num && mask < (qword)(0x8000000000000000)) {
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

void writeComOctUInt(qword num, byte port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'o';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    qword mask = 7;
    byte digits = 1;
    while (mask < num && mask < (qword)(0x8000000000000000)) {
        mask <<= 3;
        digits ++;
    }
    if (!(num & mask))
        mask >>= 3;
    *comWriteBuffers[port-1]++ = '0';
    *comWriteBuffers[port-1]++ = 'o';
    while (mask > 0) {
        qword digit = num & mask;
        while (digit > 7)
            digit >>= 3;
        *comWriteBuffers[port-1]++ = digit + 0x30;
        mask >>= 3;
    }
}

void writeComHexUInt(qword num, byte port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = 'x';
        *comWriteBuffers[port-1]++ = '0';
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    qword mask = 0xF;
    byte digits = 1;
    while (mask < num && mask < (qword)(0x8000000000000000)) {
        mask <<= 4;
        digits ++;
    }
    if (!(num & mask))
        mask >>= 4;
    *comWriteBuffers[port-1]++ = '0';
    *comWriteBuffers[port-1]++ = 'x';
    if (mask == 0xF)
        *comWriteBuffers[port-1]++ = '0';
    while (mask > 0) {
        qword digit = num & mask;
        while (digit > 0xF)
            digit >>= 4;
        if (digit < 10)
            *comWriteBuffers[port-1]++ = digit + 0x30;
        else
            *comWriteBuffers[port-1]++ = digit - 10 + 0x41;
        mask >>= 4;
    }
}

void writeComDecUInt(qword num, byte port) {
    if (num == 0) {
        *comWriteBuffers[port-1]++ = '0';
        return;
    }
    byte digits = 0;
    qword numclone = num;
    while (numclone > 0) {
        digits ++;
        numclone /= 10;
    }
    comWriteBuffers[port-1] += digits - 1;
    for (byte i = 0; i < digits; i++) {
        *comWriteBuffers[port-1]-- = num % 10 + 0x30;
        num /= 10;
    }
    comWriteBuffers[port-1] += digits + 1;
}

void kdebug(const char* text, ...) {
    if (!initPorts[0] || !debugActive)
        return;
    qword len = strlen((char*)text);
    if (comWriteBufferLength(1) + len > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + len > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    va_list l;
    va_start(l, text);
    byte state;
    byte *msg = (byte*)text;
    while (*msg != 0) {
        byte c = *msg;
        if (c == '%') {
            c = *(++msg);
            if (c == 'd')
                state = 10;
            else if (c == 'D')
                state = 100;
            else if (c == 'x')
                state = 16;
            else if (c == 'X')
                state = 64;
            else if (c == 'o')
                state = 8;
            else if (c == 'O')
                state = 88;
            else if (c == 'b')
                state = 2;
            else if (c == 'B')
                state = 22;
            else
                state = 0;
        }
        else
            state = 0;
        qword arg;
        switch (state) {
            case 0:
                if (c == 0x0A)
                    *comWriteBuffers[0]++ = 0x0D;
                *comWriteBuffers[0]++ = c;
                break;
            case 2:
                arg = va_arg(l, dword);
                arg &= 0xFFFFFFFF;
                writeComBinUInt(arg, 1);
                break;
            case 22:
                arg = va_arg(l, qword);
                writeComBinUInt(arg, 1);
                break;
            case 8:
                arg = va_arg(l, dword);
                arg &= 0xFFFFFFFF;
                writeComOctUInt(arg, 1);
                break;
            case 88:
                arg = va_arg(l, qword);
                writeComOctUInt(arg, 1);
                break;
            case 10:
                arg = va_arg(l, dword);
                arg &= 0xFFFFFFFF;
                writeComDecUInt(arg, 1);
                break;
            case 100:
                arg = va_arg(l, qword);
                writeComDecUInt(arg, 1);
                break;
            case 16:
                arg = va_arg(l, dword);
                arg &= 0xFFFFFFFF;
                writeComHexUInt(arg, 1);
                break;
            case 64:
                arg = va_arg(l, qword);
                writeComHexUInt(arg, 1);
                break;
        }
        msg ++;
    }
    enableTraInt(1);
}

void kdebug(byte num) {
    if (!initPorts[0] || !debugActive)
        return;
    if (comWriteBufferLength(1) + 1 > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + 1 > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    *comWriteBuffers[0]++ = num;
    enableTraInt(1);
}

void kdebug(word num) {
    if (!initPorts[0] || !debugActive)
        return;
    if (comWriteBufferLength(1) + 1 > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + 1 > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    *comWriteBuffers[0]++ = num & 0xFF;
    *comWriteBuffers[0]++ = (num >> 8) & 0xFF;
    enableTraInt(1);
}

void kdebug(dword num) {
    if (!initPorts[0] || !debugActive)
        return;
    if (comWriteBufferLength(1) + 1 > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + 1 > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    *comWriteBuffers[0]++ = num & 0xFF;
    *comWriteBuffers[0]++ = (num >> 8) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 16) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 24) & 0xFF;
    enableTraInt(1);
}

void kdebug(qword num) {
    if (!initPorts[0] || !debugActive)
        return;
    if (comWriteBufferLength(1) + 1 > 0xA000) {
        enableTraInt(1);
        while(comWriteBufferLength(1) + 1 > 0xA000) {io_wait();}
        disableTraInt(1);
    }
    *comWriteBuffers[0]++ = num & 0xFF;
    *comWriteBuffers[0]++ = (num >> 8) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 16) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 24) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 32) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 40) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 48) & 0xFF;
    *comWriteBuffers[0]++ = (num >> 56) & 0xFF;
    enableTraInt(1);
}

void kdebugdisable() {
    debugActive = false;
}

void kdebugenable() {
    if (initPorts[0])
        debugActive = true;
}