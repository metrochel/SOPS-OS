//
//	syscalls.h - Заголовок управления системными вызовами
//
//	Позволяет осуществлять системные вызовы путём
//	функций, а не бесконечных __asm__.
//

// Осуществляет системный вызов без аргументов под номером `code`.
inline static int syscall0(int code) {
	int result;
	__asm__ (
		"movl %d1, %%eax;"
		"int $0xC0;"
		"movl %%eax, %d0"
		: "=m"(result)
		: "m"(code)
		: );
	return result;
}

// Осуществляет системный вызов под номером `code` с аргументом `arg1`.
inline static int syscall1(int code, int arg1) {
	int result;
	__asm__ (
		"movl %d1, %%eax;"
		"movl %d2, %%esi;"
		"int $0xC0;"
		"movl %%eax, %d0"
		: "=m"(result)
		: "m"(code), "m"(arg1)
		: );
	return result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1` и `arg2`.
inline static int syscall2(int code, int arg1, int arg2) {
	int result;
	__asm__ (
		"movl %d1, %%eax;"
		"movl %d2, %%esi;"
		"movl %d3, %%edi;"
		"int $0xC0;"
		"movl %%eax, %d0"
		: "=m"(result)
		: "m"(code), "m"(arg1), "m"(arg2)
		: );
	return result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2` и `arg3`.
inline static int syscall3(int code, int arg1, int arg2, int arg3) {
	int result;
	__asm__ (
		"movl %d1, %%eax;"
		"movl %d2, %%esi;"
		"movl %d3, %%edi;"
		"movl %d4, %%ecx;"
		"int $0xC0;"
		"movl %%eax, %d0"
		: "=m"(result)
		: "m"(code), "m"(arg1), "m"(arg2), "m"(arg3)
		: );
	return result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3` и `arg4`.
inline static int syscall4(int code, int arg1, int arg2, int arg3, int arg4) {
	int result;
	__asm__ (
		"movl %d1, %%eax;"
		"movl %d2, %%esi;"
		"movl %d3, %%edi;"
		"movl %d4, %%ecx;"
		"movl %d5, %%edx;"
		"int $0xC0;"
		"movl %%eax, %d0"
		: "=m"(result)
		: "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4)
		: );
	return result;
}

// Осуществляет системный вызов под номером `code` с аргументами `arg1`, `arg2`, `arg3`, `arg4` и `arg5`.
inline static int syscall5(int code, int arg1, int arg2, int arg3, int arg4, int arg5) {
	int result;
	__asm__ (
		"push %%ebx;"
		"movl %d1, %%eax;"
		"movl %d2, %%esi;"
		"movl %d3, %%edi;"
		"movl %d4, %%ecx;"
		"movl %d5, %%edx;"
		"movl %d6, %%ebx;"
		"int $0xC0;"
		"movl %%eax, %d0;"
		"pop %%ebx"
		: "=m"(result)
		: "m"(code), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4), "m"(arg5)
		: );
	return result;
}