#include "pci.hpp"

uint32_t pciConfigReadDW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t config_addr = (uint32_t)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    return inl(PCI_CONFIG_DATA);
}

uint16_t pciConfigReadW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t dw = pciConfigReadDW(bus, device, function, offset);
    return (dw >> ((offset & 2) * 8)) & 0xFFFF;
}

uint8_t pciConfigReadB(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint16_t w = pciConfigReadW(bus, device, function, offset);
    return (w >> ((offset & 1) * 8)) & 0xFF;
}

void pciConfigWriteDW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t config_addr = (uint32_t)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, value);
}

void pciConfigWriteDW(uint32_t shuf, uint8_t offset, uint32_t value) {
    pciConfigWriteDW(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

void pciConfigWriteW(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value) {
    uint32_t config_addr = (uint32_t)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    uint32_t configDW = inl(PCI_CONFIG_DATA);
    if (offset & 2)
        configDW = (uint32_t)((value << 16) | (configDW & 0xFFFF));
    else
        configDW = (uint32_t)((configDW & 0xFFFF0000) | value);
    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, configDW);
}

void pciConfigWriteW(uint32_t shuf, uint8_t offset, uint16_t value) {
    pciConfigWriteW(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

void pciConfigWriteB(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t value) {
    uint32_t config_addr = (uint32_t)((1 << 31) | (bus << 16) | ((device & 0x0F) << 11) | ((function & 0x7) << 8) | (offset & 0xFC));

    outl(PCI_CONFIG_ADDRESS, config_addr);
    uint32_t configDW = inl(PCI_CONFIG_DATA);
    switch (offset & 3) {
        case 0:
            configDW = (uint32_t)((configDW & 0xFFFFFF00) | value);
            break;
        case 1:
            configDW = (uint32_t)((configDW & 0xFFFF00FF) | (value << 8));
            break;
        case 2:
            configDW = (uint32_t)((configDW & 0xFF00FFFF) | (value << 16));
            break;
        case 3:
            configDW = (uint32_t)((configDW & 0x00FFFFFF) | (value << 24));
            break;
    }
    outl(PCI_CONFIG_ADDRESS, config_addr);
    outl(PCI_CONFIG_DATA, configDW);
}

void pciConfigWriteB(uint32_t shuf, uint8_t offset, uint8_t value) {
    pciConfigWriteB(shuf >> 16, shuf >> 8, shuf & 0xFF, offset, value);
}

uint32_t pciConfigReadBAR(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar) {
    return pciConfigReadDW(bus, device, function, 0x10 + 0x4 * bar) & 0xFFFFFFFC;
}

uint32_t pciConfigReadBAR(uint32_t shuf, uint8_t bar) {
    return pciConfigReadBAR(shuf >> 16, shuf >> 8, shuf & 0xFF, bar);
}

void pciConfigWriteBAR(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar, uint32_t value) {
    pciConfigWriteDW(bus, device, function, 0x10 + 0x4 * bar, value);
}

void pciConfigWriteBAR(uint32_t shuf, uint8_t bar, uint32_t value) {
    pciConfigWriteDW(shuf, 0x10 + 0x4 * bar, value);
}

uint32_t pciIdentifyFunc(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t devClass = pciConfigReadB(bus, device, function, 0xB);
    uint8_t devSubclass = pciConfigReadB(bus, device, function, 0xA);
    uint8_t devProgIF = pciConfigReadB(bus, device, function, 0x9);
    return (uint32_t)((devClass << 16) | (devSubclass << 8) | devProgIF);
}

uint32_t pciIdentifyFunc(uint32_t shuf) {
    return pciIdentifyFunc(shuf >> 16, (shuf >> 8) & 0xFF, shuf & 0xFF);
}

bool pciFuncExists(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t devVendor = pciConfigReadW(bus, device, function, 0);
    return devVendor != 0xFFFF;
}

bool pciFuncExists(uint32_t shuf) {
    return pciFuncExists(shuf >> 16, shuf >> 8, shuf);
}

uint32_t checkPCIFunction(uint8_t bus, uint8_t device, uint8_t func, uint32_t neededID) {
    uint32_t funcID = pciIdentifyFunc(bus, device, func);
    if ((neededID & 0xFF) == 0xFF) {
        neededID &= 0xFFFFFF00;
        funcID &= 0xFFFFFF00;
    }
    uint32_t res;
    if (funcID >> 8 == 0x0604) {
        uint8_t secBus = pciConfigReadB(bus, device, func, 0x19);
        res = checkPCIBus(secBus, neededID);
        return res;
    } else if (funcID == neededID) {
        return (uint32_t)((bus << 16) | (device << 8) | func);
    } else
        return 0;
}

uint32_t checkPCIDevice(uint8_t bus, uint8_t device, uint32_t neededID) {
    if (!pciFuncExists(bus, device, 0)) return 0;

    uint8_t header = pciConfigReadB(bus, device, 0, 0xE);
    uint32_t res;
    if (header & 0x80) {
        for (uint8_t func = 0; func < 8; func ++) {
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

uint32_t checkPCIBus(uint8_t bus, uint32_t neededID) {
    uint32_t res;
    for (uint8_t device = 0; device < 32; device++) {
        res = checkPCIDevice(bus, device, neededID);
        if (res)
            return res;
    }
    return 0;
}

uint32_t pciFindDevice(uint8_t devclass, uint8_t devsubclass, uint8_t progIF) {
    uint32_t neededID = (uint32_t)(devclass << 16 | devsubclass << 8 | progIF);
    uint8_t hostHdrType = pciConfigReadB(0, 0, 0, 0xE);
    uint32_t res;
    if (hostHdrType & 0x80) {
        for (uint8_t f = 0; f < 8; f++) {
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

uint32_t pciFindDevice(uint8_t devclass, uint8_t devsubclass) {
    return pciFindDevice(devclass, devsubclass, 0xFF);
}