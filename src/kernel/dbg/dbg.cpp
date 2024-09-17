#include "dbg.hpp"
#include "../graphics/glyphs.hpp"

void traceStack() {
    StackFrame *frame;
    kprint("Стек:\n  ");
    __asm__ ("movl %%ebp, %d0" : "=r"(frame) :);
    kprint("%x", frame->eip);
    frame = frame->ebp;
    while (frame) {
        kprint(",\n  %x", frame->eip);
        frame = frame->ebp;
    }
    kprint("\n");
}