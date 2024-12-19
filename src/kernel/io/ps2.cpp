#include "ps2.hpp"
#include "../graphics/glyphs.hpp"
#include "../util/util.hpp"

bool firstPortAvailable = false;
bool secondPortAvailable = false;

bool initPS2() {
    disableInts();
    sendPS2ConCommand(PS2_CMD_DISABLE_FIR_PORT);
    sendPS2ConCommand(PS2_CMD_DISABLE_SEC_PORT);

    inb(PS2_DATA);
    io_wait();

    sendPS2ConCommand(PS2_CMD_READFROMRAM);
    while (!(inb(PS2_STATUS) & 1)) {io_wait();}
    byte ccb = inb(PS2_DATA);
    ccb &= ~(PS2_CCB_FIRSTPORT_INT | PS2_CCB_SECONDPORT_INT | PS2_CCB_FIRSTPORT_TRANSLATE | PS2_CCB_FIRSTPORT_CLOCK);
    sendPS2ConCommand(PS2_CMD_WRITETORAM);
    while (inb(PS2_STATUS) & 2) {io_wait();}
    outb(PS2_DATA, ccb);
    io_wait();

    sendPS2ConCommand(PS2_CMD_TEST_CONTROLLER);
    while (!(inb(PS2_STATUS) & 1)) {io_wait();}
    if (inb(PS2_DATA) != 0x55) {
        enableInts();
        return false;
    }
    io_wait();
    sendPS2ConCommand(PS2_CMD_WRITETORAM);
    while (inb(PS2_STATUS) & 2) {io_wait();}
    outb(PS2_DATA, ccb);
    io_wait();

    sendPS2ConCommand(PS2_CMD_ENABLE_SEC_PORT);
    io_wait();
    sendPS2ConCommand(PS2_CMD_READFROMRAM);
    while (!(inb(PS2_STATUS) & 1)) {io_wait();}
    ccb = inb(PS2_DATA);
    if (!(ccb & 32)) {
        secondPortAvailable = true;
        ccb &= ~(PS2_CCB_SECONDPORT_INT | PS2_CCB_SECONDPORT_CLOCK);
        sendPS2ConCommand(PS2_CMD_WRITETORAM);
        while (inb(PS2_STATUS) & 2) {io_wait();}
        outb(PS2_DATA, ccb);
    } else
        secondPortAvailable = false;

    sendPS2ConCommand(PS2_CMD_TEST_FIR_PORT);
    firstPortAvailable = !inb(PS2_DATA);
    io_wait();
    
    sendPS2ConCommand(PS2_CMD_TEST_SEC_PORT);
    secondPortAvailable = !inb(PS2_DATA);
    io_wait();

    if (!firstPortAvailable && !secondPortAvailable) {
        enableInts();
        return false;
    }

    if (firstPortAvailable) {
        sendPS2ConCommand(PS2_CMD_ENABLE_FIR_PORT);
        sendPS2ConCommand(PS2_CMD_READFROMRAM);
        while (!(inb(PS2_STATUS) & 1)) {io_wait();}
        ccb = inb(PS2_DATA);
        ccb |= PS2_CCB_FIRSTPORT_INT;
        sendPS2ConCommand(PS2_CMD_WRITETORAM);
        while (inb(PS2_STATUS) & 2) {io_wait();}
        outb(PS2_DATA, ccb);
        io_wait();
    }

    if (secondPortAvailable) {
        sendPS2ConCommand(PS2_CMD_ENABLE_SEC_PORT);
        sendPS2ConCommand(PS2_CMD_READFROMRAM);
        ccb = inb(PS2_DATA);
        ccb |= PS2_CCB_SECONDPORT_INT;
        sendPS2ConCommand(PS2_CMD_WRITETORAM);
        while (inb(PS2_STATUS) & 2) {io_wait();}
        outb(PS2_DATA, ccb);
        io_wait();
    }

    enableInts();
    return firstPortAvailable || secondPortAvailable;
}

bool sendPS2ConCommand(byte cmd) {
    while (inb(PS2_STATUS) & 2) {io_wait();}
    outb(PS2_COMMAND, cmd);
    io_wait();
    return true;
}

bool sendPS2DevCommand(byte port, byte cmd) {
    if (port == 2)
        sendPS2ConCommand(PS2_CMD_WRITETOSECPORTIN);
    for (byte timeout = 0; timeout < 0xFF; timeout ++) {
        if (!(inb(PS2_STATUS) & 1))
            break;
        if (timeout == 0xFE)
            return false;
        io_wait();
    }
    outb(PS2_DATA, cmd);
    io_wait();
    return true;
}