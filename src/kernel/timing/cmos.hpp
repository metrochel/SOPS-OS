//
//  Библиотека для управления CMOS
//
//  - Управляет CMOS-часами ПК.
//
#ifndef TIME_SIG
#include "time.hpp"
#endif
#ifndef IO_SIG
#include "../io/io.hpp"
#endif
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