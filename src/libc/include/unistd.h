//
//	unistd.h - Доступ к API POSIX
//
//	Позволяет программе использовать API POSIX-совместимых систем.
//	(СОпС, похоже, будет входить в их число.)
//

#if !defined(_UNISTD_INCL)
#define _UNISTD_INCL 1
#include <etc/decl.h>

// Нам ещё понадобится `pid_t`.
#define __need_pid_t
#include <sys/types.h>

BEGIN_DECLS

// Нам потребуется размер слова для определения `intptr_t`.
#include <etc/word.h>
#if WORDSZ == 64
typedef signed long intptr_t;
#else
typedef signed int  intptr_t;
#endif

// Производит форк процесса и возвращает PID дочернего процесса.
pid_t fork(void);

// Запускает программу по пути `path` с аргументами `argv`.
int execv(const char *path, char* const argv[]);

// Запускает программу по пути `path` с аргументами `argv` и переменными окружения `envp`.
int execve(const char *path, char *const *argv, char *const *envp);

// Запускает программу с названием `path`, если она есть в PATH, с аргументами `argv` и окружением `envp`.
int execvp(const char *path, char *const *argv);

// Извлекает значение PID для данного процесса.
pid_t getpid(void);

END_DECLS

#endif