//
//	syscalls.h - Заголовок управления системными вызовами
//
//	Позволяет осуществлять системные вызовы путём
//	функций, а не бесконечных __asm__.
//

#include "syscall_macros.h"

#ifdef __x86_64__                   // Если система 64-битная...

// Порядок передачи аргументов системным вызовам:
// RSI, RDI, RCX, RDX, RBX

// Тип аргумента системного вызова. (64-бит)
typedef unsigned long long syscall_arg_t;

#define SET_CODE(arg)  "movl " #arg ", %%rax;"
#define SET_ARG1(arg)  "movl " #arg ", %%rsi;"
#define SET_ARG2(arg)  "movl " #arg ", %%rdi;"
#define SET_ARG3(arg)  "movl " #arg ", %%rcx;"
#define SET_ARG4(arg)  "movl " #arg ", %%rdx;"
#define SET_ARG5(arg)  "movl " #arg ", %%rbx;"

// Возврат значения производится в регистре RAX. RDX зарезервирован под дальнейшее расширение.
#define GET_RESULT(arg1, arg2) "movl %%rax, " #arg1 "; movl %%rdx, " #arg2

#else

// Тип аргумента системного вызова. (32-бит)
typedef unsigned int syscall_arg_t;

// Порядок передачи аргументов системным вызовам:
// ESI, EDI, ECX, EDX, EBX

#define SET_CODE(arg)  "movl " arg ", %%eax;"
#define SET_ARG1(arg)  "movl " arg ", %%esi;"
#define SET_ARG2(arg)  "movl " arg ", %%edi;"
#define SET_ARG3(arg)  "movl " arg ", %%ecx;"
#define SET_ARG4(arg)  "movl " arg ", %%edx;"
#define SET_ARG5(arg)  "movl " arg ", %%ebx;"

// Возврат значения производится в регистрах EAX:EDX.
#define GET_RESULT(arg1, arg2) "movl %%eax, " arg1 "; movl %%edx, " arg2

#endif

#define SYSCALL_INT "int $0xC0;"

#define SYSCALL_ARG(arg)    (syscall_arg_t)(arg)

// Осуществляет системный вызов без аргументов под номером `code`.
inline static long syscall0(syscall_arg_t code) {
	int result[2];
	__asm__ (
		SET_CODE("%d2")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
		: "=m"(result[0]), "=m"(result[1])
		: "m"(code)
		: );
	return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументом `arg1`.
inline static long syscall1(int code, int arg1) {
    int result[2];
    __asm__ (
        SET_CODE("%d2")
        SET_ARG1("%d3")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
        : "=m"(result[0]), "=m"(result[1])
        : "m"(code), "m"(arg1)
        : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1` и `arg2`.
inline static long syscall2(int code, int arg1, int arg2) {
    int result[2];
    __asm__ (
        SET_CODE("%d2")
        SET_ARG1("%d3")
        SET_ARG2("%d4")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
        : "=m"(result[0]), "=m"(result[1])
        : "m"(code), "m"(arg1), "m"(arg2)
        : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2` и `arg3`.
inline static long syscall3(int code, int arg1, int arg2, int arg3) {
    int result[2];
    __asm__ (
        SET_CODE("%d2")
        SET_ARG1("%d3")
        SET_ARG2("%d4")
        SET_ARG3("%d5")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
        : "=m"(result[0]), "=m"(result[1])
        : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3)
        : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3` и `arg4`.
inline static long syscall4(int code, int arg1, int arg2, int arg3, int arg4) {
    int result[2];
    __asm__ (
        SET_CODE("%d2")
        SET_ARG1("%d3")
        SET_ARG2("%d4")
        SET_ARG3("%d5")
        SET_ARG4("%d6")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
        : "=m"(result[0]), "=m"(result[1])
        : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4)
        : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3`, `arg4` и `arg5`.
inline static long syscall5(int code, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int result[2];
    __asm__ (
        SET_CODE("%d2")
        SET_ARG1("%d3")
        SET_ARG2("%d4")
        SET_ARG3("%d5")
        SET_ARG4("%d6")
        SET_ARG5("%d7")
        SYSCALL_INT
        GET_RESULT("%d0", "%d1")
        : "=m"(result[0]), "=m"(result[1])
        : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4), "m"(arg5)
        : );
    return *(long*)result;
}