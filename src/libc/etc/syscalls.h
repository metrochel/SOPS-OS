//
//	syscalls.h - Заголовок управления системными вызовами
//
//	Позволяет осуществлять системные вызовы путём
//	функций, а не бесконечных __asm__.
//

#include "syscall_macros.h"

// Осуществляет системный вызов без аргументов под номером `code`.
inline static long syscall0(int code) {
	int result[2];
	__asm__ (
		"movl %d2, %%eax;"
		"int $0xC0;"
		"movl %%eax, %d0;"
        "movl %%edx, %d1"
		: "=m"(result[0]), "=m"(result[1])
		: "m"(code)
		: );
	return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументом `arg1`.
inline static long syscall1(int code, int arg1) {
    int result[2];
    __asm__ (
            "movl %d2, %%eax;"
            "movl %d3, %%esi;"
            "int $0xC0;"
            "movl %%eax, %d0;"
            "movl %%edx, %d1"
            : "=m"(result[0]), "=m"(result[1])
            : "m"(code), "m"(arg1)
            : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1` и `arg2`.
inline static long syscall2(int code, int arg1, int arg2) {
    int result[2];
    __asm__ (
            "movl %d2, %%eax;"
            "movl %d3, %%esi;"
            "movl %d4, %%edi;"
            "int $0xC0;"
            "movl %%eax, %d0;"
            "movl %%edx, %d1"
            : "=m"(result[0]), "=m"(result[1])
            : "m"(code), "m"(arg1), "m"(arg2)
            : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2` и `arg3`.
inline static long syscall3(int code, int arg1, int arg2, int arg3) {
    int result[2];
    __asm__ (
            "movl %d2, %%eax;"
            "movl %d3, %%esi;"
            "movl %d4, %%edi;"
            "movl %d5, %%ecx;"
            "int $0xC0;"
            "movl %%eax, %d0;"
            "movl %%edx, %d1"
            : "=m"(result[0]), "=m"(result[1])
            : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3)
            : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3` и `arg4`.
inline static long syscall4(int code, int arg1, int arg2, int arg3, int arg4) {
    int result[2];
    __asm__ (
            "movl %d2, %%eax;"
            "movl %d3, %%esi;"
            "movl %d4, %%edi;"
            "movl %d5, %%ecx;"
            "movl %d6, %%edx;"
            "int $0xC0;"
            "movl %%eax, %d0;"
            "movl %%edx, %d1"
            : "=m"(result[0]), "=m"(result[1])
            : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4)
            : );
    return *(long*)result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3`, `arg4` и `arg5`.
inline static long syscall5(int code, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int result[2];
    __asm__ (
            "movl %d2, %%eax;"
            "movl %d3, %%esi;"
            "movl %d4, %%edi;"
            "movl %d5, %%ecx;"
            "movl %d6, %%edx;"
            "movl %d7, %%ebx;"
            "int $0xC0;"
            "movl %%eax, %d0;"
            "movl %%edx, %d1"
            : "=m"(result[0]), "=m"(result[1])
            : "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4), "m"(arg5)
            : );
    return *(long*)result;
}