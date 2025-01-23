//
//  Библиотека для отладки
//
//  - Маленькая фишка, которая поможет работать с отладчиками.
//
#ifndef _DBG_INCL
#define _DBG_INCL

#include "../util/nums.hpp"
#include "../io/com.hpp"

// ### StackFrame
// Структура для трассировки стека.
struct StackFrame {
    StackFrame *ebp;
    dword eip;
} __attribute__((packed));

/// @brief Создаёт точку останова для Bochs-а.
inline void magicBreakpoint() {
    kdebugwait();
    __asm__ ("xchgw %bx, %bx");
}

/// @brief Пробегает по стеку вызовов.
void traceStack();

void initKernelMap(byte drive);

#endif