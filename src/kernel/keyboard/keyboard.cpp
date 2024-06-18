#include "keyboard.hpp"
#include "keycodes.hpp"
#include <stdarg.h>
#ifndef GRAPHICS_SIG
#include "../graphics/glyphs.hpp"
#endif

bool cmdAwaitingResponse = false;
bool releaseScancode = false;
bool inputFinished = false;
bool inputAllowed = false;

uint8_t *kbBufPtr = (uint8_t*)KB_BUF_BASE;
uint8_t *kbCmdBufPtr = (uint8_t*)KB_CMD_BUF_BASE;
uint8_t *stdinPtr = (uint8_t*)0x9100;
uint32_t stdinBase = 0x9100;

uint8_t kbStatus = 0;

uint8_t getSymbol(uint8_t keycode) {
    if (kbStatus & (KB_STATUS_CTRL | KB_STATUS_ALT))
        return 0;
    bool shift = ((kbStatus & KB_STATUS_SHIFT) && !(kbStatus & KB_STATUS_CAPSLOCK)) || (!(kbStatus & KB_STATUS_SHIFT) && kbStatus & KB_STATUS_CAPSLOCK);
    switch (keycode) {
        case KEYCODE_SPACEBAR: return ' ';
        case KEYCODE_A:
            if (shift)
                return 'A';
            else
                return 'a';
        case KEYCODE_B:
            if (shift)
                return 'B';
            else
                return 'b';
        case KEYCODE_C:
            if (shift)
                return 'C';
            else
                return 'c';
        case KEYCODE_D:
            if (shift)
                return 'D';
            else
                return 'd';
        case KEYCODE_E:
            if (shift)
                return 'E';
            else
                return 'e';
        case KEYCODE_F:
            if (shift)
                return 'F';
            else
                return 'f';
        case KEYCODE_G:
            if (shift)
                return 'G';
            else
                return 'g';
        case KEYCODE_H:
            if (shift)
                return 'H';
            else
                return 'h';
        case KEYCODE_I:
            if (shift)
                return 'I';
            else
                return 'i';
        case KEYCODE_J:
            if (shift)
                return 'J';
            else
                return 'j';
        case KEYCODE_K:
            if (shift)
                return 'K';
            else
                return 'k';
        case KEYCODE_L:
            if (shift)
                return 'L';
            else
                return 'l';
        case KEYCODE_M:
            if (shift)
                return 'M';
            else
                return 'm';
        case KEYCODE_N:
            if (shift)
                return 'N';
            else
                return 'n';
        case KEYCODE_O:
            if (shift)
                return 'O';
            else
                return 'o';
        case KEYCODE_P:
            if (shift)
                return 'P';
            else
                return 'p';
        case KEYCODE_Q:
            if (shift)
                return 'Q';
            else
                return 'q';
        case KEYCODE_R:
            if (shift)
                return 'R';
            else
                return 'r';
        case KEYCODE_S:
            if (shift)
                return 'S';
            else
                return 's';
        case KEYCODE_T:
            if (shift)
                return 'T';
            else
                return 't';
        case KEYCODE_U:
            if (shift)
                return 'U';
            else
                return 'u';
        case KEYCODE_V:
            if (shift)
                return 'V';
            else
                return 'v';
        case KEYCODE_W:
            if (shift)
                return 'W';
            else
                return 'w';
        case KEYCODE_X:
            if (shift)
                return 'X';
            else
                return 'x';
        case KEYCODE_Y:
            if (shift)
                return 'Y';
            else
                return 'y';
        case KEYCODE_Z:
            if (shift)
                return 'Z';
            else
                return 'z';
        case KEYCODE_1:
            if (kbStatus & KB_STATUS_SHIFT)
                return '!';
            else
                return '1';
        case KEYCODE_2:
            if (kbStatus & KB_STATUS_SHIFT)
                return '@';
            else
                return '2';
        case KEYCODE_3:
            if (kbStatus & KB_STATUS_SHIFT)
                return '#';
            else
                return '3';
        case KEYCODE_4:
            if (kbStatus & KB_STATUS_SHIFT)
                return '$';
            else
                return '4';
        case KEYCODE_5:
            if (kbStatus & KB_STATUS_SHIFT)
                return '%';
            else
                return '5';
        case KEYCODE_6:
            if (kbStatus & KB_STATUS_SHIFT)
                return '^';
            else
                return '6';
        case KEYCODE_7:
            if (kbStatus & KB_STATUS_SHIFT)
                return '&';
            else
                return '7';
        case KEYCODE_8:
            if (kbStatus & KB_STATUS_SHIFT)
                return '*';
            else
                return '8';
        case KEYCODE_9:
            if (kbStatus & KB_STATUS_SHIFT)
                return '(';
            else
                return '9';
        case KEYCODE_0:
            if (kbStatus & KB_STATUS_SHIFT)
                return ')';
            else
                return '0';
        case KEYCODE_DOT:
            if (kbStatus & KB_STATUS_SHIFT)
                return '>';
            else
                return '.';
        case KEYCODE_COMMA:
            if (kbStatus & KB_STATUS_SHIFT)
                return '<';
            else
                return ',';
        case KEYCODE_SLASH:
            if (kbStatus & KB_STATUS_SHIFT)
                return '?';
            else
                return '/';
        case KEYCODE_SEMICOLON:
            if (kbStatus & KB_STATUS_SHIFT)
                return ':';
            else
                return ';';
        case KEYCODE_APOSTROPHE:
            if (kbStatus & KB_STATUS_SHIFT)
                return '"';
            else
                return '\'';
        case KEYCODE_LEFT_BRACKET:
            if (kbStatus & KB_STATUS_SHIFT)
                return '{';
            else
                return '[';
        case KEYCODE_RIGHT_BRACKET:
            if (kbStatus & KB_STATUS_SHIFT)
                return '}';
            else
                return ']';
        case KEYCODE_BACKSLASH:
            if (kbStatus & KB_STATUS_SHIFT)
                return '|';
            else
                return '\\';
        case KEYCODE_MINUS:
            if (kbStatus & KB_STATUS_SHIFT)
                return '_';
            else
                return '-';
        case KEYCODE_EQUALS:
            if (kbStatus & KB_STATUS_SHIFT)
                return '+';
            else
                return '=';
        case KEYCODE_ACUTE:
            if (kbStatus & KB_STATUS_SHIFT)
                return '~';
            else
                return '`';
        default: return 0;
    }
}

void shiftKBCmdQueue() {
    uint8_t cmd = *(uint8_t*)KB_CMD_BUF_BASE;
    uint8_t bufLen = (uint32_t)kbCmdBufPtr - KB_CMD_BUF_BASE;
    for (uint8_t i = 0; i < bufLen; i++) {
        *(uint16_t*)(KB_CMD_BUF_BASE + i) >>= 8;
    }
    bufLen --;
    kbCmdBufPtr --;
    if (cmd == KB_CMD_KEYSET || cmd == KB_CMD_SET_LEDS || cmd == KB_CMD_SET_TYPEMATIC) {
        for (uint8_t i = 0; i < bufLen; i++) {
            *(uint16_t*)(KB_CMD_BUF_BASE + i) >>= 8;
        }
        kbCmdBufPtr --;
    }
}

void sendKBCmd() {
    uint8_t nextCmd = *(uint8_t*)KB_CMD_BUF_BASE;
    if (nextCmd == 0) {
        return;
    }
    sendPS2DevCommand(1, nextCmd);
    if (nextCmd == KB_CMD_KEYSET || nextCmd == KB_CMD_SET_LEDS || nextCmd == KB_CMD_SET_TYPEMATIC) {
        uint8_t arg = *(uint8_t*)(KB_CMD_BUF_BASE + 1);
        sendPS2DevCommand(1, arg);
    }
}

void updateKB() {
    if (!inputAllowed) {
        releaseScancode = false;
        return;
    }
    uint8_t kbBufLen = (uint32_t)kbBufPtr - KB_BUF_BASE;
    uint8_t xchgBuf;
    kbBufPtr = (uint8_t*)KB_BUF_BASE;
    for (uint8_t i = 0; i < kbBufLen / 2; i++) {
        xchgBuf = *(kbBufPtr + i);
        *(kbBufPtr + i) = *(kbBufPtr + kbBufLen - i - 1);
        *(kbBufPtr + kbBufLen - i - 1) = xchgBuf;
    }
    uint64_t scancode = *(uint64_t*)kbBufPtr;
    for (uint8_t i = 0; i < 8; i++) {
        *(kbBufPtr + i) = 0;
    }
    uint8_t keycode = getKeyCode(scancode);

    if (keycode == KEYCODE_LEFT_SHIFT || keycode == KEYCODE_RIGHT_SHIFT) {
        if (releaseScancode)
            kbStatus &= ~KB_STATUS_SHIFT;
        else
            kbStatus |= KB_STATUS_SHIFT;
    }
    if (keycode == KEYCODE_LEFT_CTRL || keycode == KEYCODE_RIGHT_CTRL) {
        if (releaseScancode)
            kbStatus &= ~KB_STATUS_CTRL;
        else
            kbStatus |= KB_STATUS_CTRL;
    }
    if (keycode == KEYCODE_LEFT_ALT || keycode == KEYCODE_RIGHT_CTRL) {
        if (releaseScancode)
            kbStatus &= ~KB_STATUS_ALT;
        else
            kbStatus |= KB_STATUS_ALT;
    }
    if (keycode == KEYCODE_CAPS_LOCK && !releaseScancode) {
        if (kbStatus & KB_STATUS_CAPSLOCK)
            kbStatus &= ~KB_STATUS_CAPSLOCK;
        else
            kbStatus |= KB_STATUS_CAPSLOCK;
    }
    if (keycode == KEYCODE_BACKSPACE && !releaseScancode && ((uint32_t)stdinPtr - stdinBase > 0)) {
        stdinPtr --;
        *stdinPtr = 0;
        eraseChar();
        releaseScancode = false;
        return;
    }
    if (keycode == KEYCODE_ENTER && !releaseScancode) {
        inputFinished = true;
        releaseScancode = false;
        return;
    }
    uint8_t symbol = getSymbol(keycode);
    uint32_t stdinLen = (uint32_t)stdinPtr - stdinBase;
    if (stdinLen + 1 > 0x200) {
        releaseScancode = false;
        return;
    }
    if (!releaseScancode && symbol != 0) {
        *stdinPtr = symbol;
        if (symbol == '%')
            kprint("%%");
        else
            kprint((const char*)stdinPtr);
        stdinPtr ++;
    }
    releaseScancode = false;
}

bool initKB() {
    kbBufPtr = (uint8_t*)KB_BUF_BASE;
    for (uint8_t i = 0; i < 8; i++) {
        *(kbBufPtr + i) = 0;
    }
    kbCmdBufPtr = (uint8_t*)KB_CMD_BUF_BASE;
    for (uint16_t i = 0; i < 0x100; i ++) {
        *(kbCmdBufPtr + i) = 0;
    }
    if (!sendKBCommand(KB_CMD_RESET))
        return false;
    if (!sendKBCommand(KB_CMD_SET_LEDS, 0))
        return false;
    if (!sendKBCommand(KB_CMD_KEYSET, 2))
        return false;
    if (!sendKBCommand(KB_CMD_SET_TYPEMATIC, 0b00110101))
        return false;
    if (!sendKBCommand(KB_CMD_ENABLE_SCANNING))
        return false;
    while (*(uint8_t*)KB_BUF_BASE == 0) {io_wait();}
    if (*(uint8_t*)KB_BUF_BASE != 0xAA)
        return false;
    *(uint8_t*)KB_BUF_BASE = 0;
    return true;
}

bool sendKBCommand(uint8_t cmd) {
    if ((uint32_t)kbCmdBufPtr - KB_CMD_BUF_BASE + 1 > 128)
        return false;
    *kbCmdBufPtr = cmd;
    kbCmdBufPtr ++;
    if ((uint32_t)kbCmdBufPtr == KB_CMD_BUF_BASE + 1) {
        sendPS2DevCommand(1, cmd);
    }
    return true;
}

bool sendKBCommand(uint8_t cmd, uint8_t arg) {
    if ((uint32_t)kbCmdBufPtr - KB_CMD_BUF_BASE + 2 > 128)
        return false;
    *kbCmdBufPtr = cmd;
    kbCmdBufPtr ++;
    *kbCmdBufPtr = arg;
    kbCmdBufPtr ++;
    if ((uint32_t)kbCmdBufPtr == KB_CMD_BUF_BASE + 2) {
        sendPS2DevCommand(1, cmd);
        sendPS2DevCommand(1, arg);
    }
    return true;
}

void kread(uint8_t *in) {
    stdinBase = (uint32_t)in;
    stdinPtr = in;
    for (uint16_t i = 0; i < 0x200; i++) {
        *(stdinPtr + i) = 0;
    }
    inputAllowed = true;
    while (!inputFinished) {io_wait();}
    inputAllowed = false;
    inputFinished = false;
}