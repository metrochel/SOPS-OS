//
//  Библиотека для управления CMOS
//
//  - Управляет CMOS-часами ПК.
//
#ifndef _CMOS_INCL
#define _CMOS_INCL

#include "time.hpp"
#include "../io/io.hpp"
#include <stdint.h>

#define CMOS_REGISTER_SELECT 0x70
#define CMOS_REGISTER        0x71

inline uint8_t readCMOSReg(uint8_t regNo) {
    outb(CMOS_REGISTER_SELECT, regNo);
    return inb(CMOS_REGISTER);
};

inline void writeCMOSReg(uint8_t regNo, uint8_t newValue) {
    outb(CMOS_REGISTER_SELECT, regNo);
    outb(CMOS_REGISTER, newValue);
}

#endif