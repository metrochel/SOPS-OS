//
//	init.h - Заголовок инициализации libc
//
//	Подготавливает libc для работы с программой.
//

#ifndef _INIT_LIBC_INCL
#define _INIT_LIBC_INCL 1

// Инициализирует libc.
void init_libc(int argc, char *argv, int envc, char *envp);

#endif