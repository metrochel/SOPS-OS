/*
 * dbgutil.hpp - Инструменты отладки
 *
 * Предоставляет несколько инструментов отладки.
 */

#ifndef SOPS_DBGUTIL_HPP
#define SOPS_DBGUTIL_HPP

#include "libk/objs.hpp"

namespace debug {
    struct stack_frame {
        stack_frame *ebp;
        byte *eip;
    };

    inline void magic_breakpoint() {
        __asm__ volatile ("xchg bx, bx");
    }

    inline void hang() {
        __asm__ volatile ("cli; 1: hlt; jmp 1");
    }

    always_inline void* get_return_address() {
        void* addr;
        __asm__ volatile (
            "mov %d0, dword [ebp + 4]"
            : "=a"(addr)
            :
            :
            );
        return addr;
    }

    void print_stack_trace();
}

#endif //SOPS_DBGUTIL_HPP
