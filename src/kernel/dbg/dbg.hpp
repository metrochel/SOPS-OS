//
//  Библиотека для отладки
//
//  - Маленькая фишка, которая поможет работать с отладчиками.
//
#ifndef _DBG_INCL
#define _DBG_INCL

/// @brief Создаёт точку останова для Bochsа.
inline void magicBreakpoint() {
    __asm__ ("xchgw %bx, %bx");
}

#endif