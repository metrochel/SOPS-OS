#include "include/setjmp.h"

noreturn void longjmp(jmp_buf env, int status) {
    env.status = status;

    __asm__ volatile (
        SET_EAX("%d0")
        SET_EBX("%d1")
        SET_ECX("%d2")
        SET_EDX("%d3")
        SET_ESI("%d4")
        SET_EDI("%d5")
        SET_ESP("%d6")
        SET_EBP("%d7")
        SET_EIP("%d8")
        : :
            "m"(env.eax), "m"(env.ebx), "m"(env.ecx), "m"(env.edx),
            "m"(env.esi), "m"(env.edi),
            "m"(env.esp), "m"(env.ebp),
            "m"(env.eip)
        :
    );

    setjmp(env);
}