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

inline ptrint getReturnAddress(byte depth) {
    StackFrame *frame;
    __asm__ ("movl %%ebp, %d0;" : "=r"(frame) : : );
    for (byte i = 0; i < depth - 1; i++)
        frame = frame->ebp;
    return frame->eip;
}

inline ptrint getReturnAddress() {
    StackFrame *frame;
    __asm__ ("movl %%ebp, %d0;" : "=r"(frame) : : );
    return frame->eip;
}

/// @brief Пробегает по стеку вызовов.
void traceStack();

char* findFunctionName(ptrint addr);

void initKernelMap(byte drive);

#endif