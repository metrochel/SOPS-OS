/*
 * dbgutil.cpp - Инструменты отладки
 *
 */

#include "dbgutil.hpp"
#include "libk/objs.hpp"
#include "graphics/graphics.hpp"

NAMESPACE_BEGIN(debug)

void print_stack_trace() {
    auto *frame = (stack_frame*)__builtin_return_address(0);

    cout << "Стек:\n";
    cout << "  " << frame->eip;
    frame = frame->ebp;
    while (frame) {
        cout << ",\n  " << frame->eip;
        frame = frame->ebp;
    }
}

NAMESPACE_END()