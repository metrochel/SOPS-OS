//
//  setjmp.h - Изменение контекста исполнения
//
//  Позволяет переходить на другие точки исполнения.
//

#ifndef _SETJMP_H
#define _SETJMP_H

#include <stdnoreturn.h>

#ifdef __x86_64__

typedef struct {
    int status;
    unsigned long long eax, ebx, ecx, edx;
    unsigned long long esi, edi;
    unsigned long long esp, ebp;
    unsigned long long eip;
} jmp_buf;

#define GET_EAX(memloc) "movl %%rax, " memloc ";"
#define GET_EBX(memloc) "movl %%rbx, " memloc ";"
#define GET_ECX(memloc) "movl %%rcx, " memloc ";"
#define GET_EDX(memloc) "movl %%rdx, " memloc ";"
#define GET_ESI(memloc) "movl %%rsi, " memloc ";"
#define GET_EDI(memloc) "movl %%rdi, " memloc ";"
#define GET_ESP(memloc) "movl %%rsp, " memloc ";"
#define GET_EBP(memloc) "movl %%rbp, " memloc ";"
#define GET_EIP(memloc) "push __label; pop " memloc "; __label:"

#define SET_EAX(memloc) "movl " memloc ", %%rax;"
#define SET_EBX(memloc) "movl " memloc ", %%rbx;"
#define SET_ECX(memloc) "movl " memloc ", %%rcx;"
#define SET_EDX(memloc) "movl " memloc ", %%rdx;"
#define SET_ESI(memloc) "movl " memloc ", %%rsi;"
#define SET_EDI(memloc) "movl " memloc ", %%rdi;"
#define SET_ESP(memloc) "movl " memloc ", %%rsp;"
#define SET_EBP(memloc) "movl " memloc ", %%rbp;"
#define SET_EIP(memloc) "jmp "  memloc ";"

#else

typedef struct {
    int status;
    unsigned int eax, ebx, ecx, edx;
    unsigned int esi, edi;
    unsigned int esp, ebp;
    unsigned int eip;
} jmp_buf;

#define GET_EAX(memloc) "movl %%eax, " memloc ";"
#define GET_EBX(memloc) "movl %%ebx, " memloc ";"
#define GET_ECX(memloc) "movl %%ecx, " memloc ";"
#define GET_EDX(memloc) "movl %%edx, " memloc ";"
#define GET_ESI(memloc) "movl %%esi, " memloc ";"
#define GET_EDI(memloc) "movl %%edi, " memloc ";"
#define GET_ESP(memloc) "movl %%esp, " memloc ";"
#define GET_EBP(memloc) "movl %%ebp, " memloc ";"
#define GET_EIP(memloc) "push __label; pop " memloc "; __label:"

#define SET_EAX(memloc) "movl " memloc ", %%eax;"
#define SET_EBX(memloc) "movl " memloc ", %%ebx;"
#define SET_ECX(memloc) "movl " memloc ", %%ecx;"
#define SET_EDX(memloc) "movl " memloc ", %%edx;"
#define SET_ESI(memloc) "movl " memloc ", %%esi;"
#define SET_EDI(memloc) "movl " memloc ", %%edi;"
#define SET_ESP(memloc) "movl " memloc ", %%esp;"
#define SET_EBP(memloc) "movl " memloc ", %%ebp;"
#define SET_EIP(memloc) "jmp "  memloc ";"

#endif

// Сохраняет контекст исполнения в `env` и восстанавливает значение `status`.
#define setjmp(env) {                                                               \
    env.status = 0;                                                                 \
    __asm__ volatile (                                                              \
        GET_EAX("%d0")                                                              \
        GET_EBX("%d1")                                                              \
        GET_ECX("%d2")                                                              \
        GET_EDX("%d3")                                                              \
        GET_ESI("%d4")                                                              \
        GET_EDI("%d5")                                                              \
        GET_ESP("%d6")                                                              \
        GET_EBP("%d7")                                                              \
        GET_EIP("%d8")                                                              \
        :                                                                           \
            "=m"(env.eax), "=m"(env.ebx), "=m"(env.ecx), "=m"(env.edx),             \
            "=m"(env.esi), "=m"(env.edi),                                           \
            "=m"(env.esp), "=m"(env.ebp),                                           \
            "=m"(env.eip)                                                           \
        : :                                                                         \
    );                                                                              \
    env.status;                                                                     \
}

// Производит переход на контекст, заданный `env`, при этом
noreturn void longjmp(jmp_buf env, int status);

#endif
