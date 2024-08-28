#include "pci.hpp"

dword pciConfigReadDW(byte bus, byte device, byte function, byte offset) {
    dword config_addr = (dword)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    return inl(PCI_CONFIG_DATA);
}

word pciConfigReadW(byte bus, byte device, byte function, byte offset) {
    dword dw = pciConfigReadDW(bus, device, function, offset);
    return (dw >> ((offset & 2) * 8)) & 0xFFFF;
}

byte pciConfigReadB(byte bus, byte device, byte function, byte offset) {
    word w = pciConfigReadW(bus, device, function, offset);
    return (w >> ((offset & 1) * 8)) & 0xFF;
}

void pciConfigWriteDW(byte bus, byte device, byte function, byte offset, dword value) {
    dword config_addr = (dword)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, value);
}

void pciConfigWriteDW(dword shuf, byte offset, dword value) {
    pciConfigWriteDW(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

void pciConfigWriteW(byte bus, byte device, byte function, byte offset, word value) {
    dword config_addr = (dword)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    dword configDW = inl(PCI_CONFIG_DATA);
    if (offset & 2)
        configDW = (dword)((value << 16) | (configDW & 0xFFFF));
    else
        configDW = (dword)((configDW & 0xFFFF0000) | value);
    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, configDW);
}

void pciConfigWriteW(dword shuf, byte offset, word value) {
    pciConfigWriteW(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

void pciConfigWriteB(byte bus, byte device, byte function, byte offset, byte value) {
    dword config_addr = (dword)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    dword configDW = inl(PCI_CONFIG_DATA);
    switch (offset & 3) {
        case 0:
            configDW = (dword)((configDW & 0xFFFFFF00) | value);
            break;
        case 1:
            configDW = (dword)((configDW & 0xFFFF00FF) | (value << 8));
            break;
        case 2:
            configDW = (dword)((configDW & 0xFF00FFFF) | (value << 16));
            break;
        case 3:
            configDW = (dword)((configDW & 0x00FFFFFF) | (value << 24));
            break;
    }
    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, configDW);
}

void pciConfigWriteB(dword shuf, byte offset, byte value) {
    pciConfigWriteB(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

dword pciConfigReadBAR(byte bus, byte device, byte function, byte bar) {
    return pciConfigReadDW(bus, device, function, 0x10 + 0x4 * bar) & 0xFFFFFFFC;
}

dword pciConfigReadBAR(dword shuf, byte bar) {
    return pciConfigReadBAR(shuf >> 16, shuf >> 8, shuf & 0xFF, bar);
}

void pciConfigWriteBAR(byte bus, byte device, byte function, byte bar, dword value) {
    pciConfigWriteDW(bus, device, function, 0x10 + 0x4 * bar, value);
}

void pciConfigWriteBAR(dword shuf, byte bar, dword value) {
    pciConfigWriteDW(shuf, 0x10 + 0x4 * bar, value);
}

dword pciIdentifyFunc(byte bus, byte device, byte function) {
    byte devClass = pciConfigReadB(bus, device, function, 0xB);
    byte devSubclass = pciConfigReadB(bus, device, function, 0xA);
    byte devProgIF = pciConfigReadB(bus, device, function, 0x9);
    return (dword)((devClass << 16) | (devSubclass << 8) | devProgIF);
}

dword pciIdentifyFunc(dword shuf) {
    return pciIdentifyFunc(shuf >> 16, (shuf >> 8) & 0xFF, shuf & 0xFF);
}

bool pciFuncExists(byte bus, byte device, byte function) {
    dword devVendor = pciConfigReadW(bus, device, function, 0);
    return devVendor != 0xFFFF;
}

bool pciFuncExists(dword shuf) {
    return pciFuncExists(shuf >> 16, shuf >> 8, shuf);
}

dword checkPCIFunction(byte bus, byte device, byte func, dword neededID) {
    dword funcID = pciIdentifyFunc(bus, device, func);
    if ((neededID & 0xFF) == 0xFF) {
        neededID &= 0xFFFFFF00;
        funcID &= 0xFFFFFF00;
    }
    dword res;
    if (funcID >> 8 == 0x0604) {
        byte secBus = pciConfigReadB(bus, device, func, 0x19);
        res = checkPCIBus(secBus, neededID);
        return res;
    } else if (funcID == neededID) {
        return (dword)((bus << 16) | (device << 8) | func);
    } else
        return 0;
}

dword checkPCIDevice(byte bus, byte device, dword neededID) {
    if (!pciFuncExists(bus, device, 0)) return 0;

    byte header = pciConfigReadB(bus, device, 0, 0xE);
    dword res;
    if (header & 0x80) {
        for (byte func = 0; func < 8; func ++) {
            res = checkPCIFunction(bus, device, func, neededID);
            if (res)
                return res;
        }
        return 0;
    } else {
        res = checkPCIFunction(bus, device, 0, neededID);
        return res;
    }
}

dword checkPCIBus(byte bus, dword neededID) {
    dword res;
    for (byte device = 0; device < 32; device++) {
        res = checkPCIDevice(bus, device, neededID);
        if (res)
            return res;
    }
    return 0;
}

dword pciFindDevice(byte devclass, byte devsubclass, byte progIF) {
    dword neededID = (dword)(devclass << 16 | devsubclass << 8 | progIF);
    byte hostHdrType = pciConfigReadB(0, 0, 0, 0xE);
    dword res;
    if (hostHdrType & 0x80) {
        for (byte f = 0; f < 8; f++) {
            if (!pciFuncExists(0,0,f))
                break;
            res = checkPCIBus(f, neededID);
            if (res)
                return res;
        }
    } else {
        res = checkPCIBus(0, neededID);
        return res;
    }
    return 0;
}

dword pciFindDevice(byte devclass, byte devsubclass) {
    return pciFindDevice(devclass, devsubclass, 0xFF);
}