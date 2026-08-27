/*
 * dbgutil.cpp - Инструменты отладки
 *
 */

#include "dbgutil.hpp"
#include "libk/objs.hpp"
#include "graphics/graphics.hpp"

NAMESPACE_BEGIN(debug)

void print_stack_trace() {
    stack_frame *frame;
    __asm__ volatile (
        "mov %d0, ebp"
        : "=m"(frame)
        :
        :
    );

    cout << "Стек:\n";
    cout << "  " << frame->eip;
    frame = frame->ebp;
    while (frame) {
        cout << ",\n  " << frame->eip;
        frame = frame->ebp;
    }
}

NAMESPACE_END()