//
//  signal.h - Обработка сигналов
//
//  Генерирует и обрабатывает сигналы.
//

#ifndef _SIGNAL_INCL
#define _SIGNAL_INCL 1

#include <etc/decl.h>

/* Сигналы */

// Сигнал экстренного завершения программы
#define SIGABRT 1
// Сигнал ошибки в вычислениях
#define SIGFPE 2
// Сигнал ошибки в инструкции
#define SIGILL 3
// Сигнал прерывания
#define SIGINT 4
// Сигнал ошибки в памяти
#define SIGSEGV 5
// Сигнал завершения программы
#define SIGTERM 6

/* Флаги сигналов */

// Флаг активации обработчика сигнала
#define SIG_DFL ((void (*)(int))0)
// Флаг игнорирования сигнала
#define SIG_IGN ((void (*)(int))1)
// Флаг ошибки при установке обработчика сигнала
#define SIG_ERR ((void (*)(int))-1)

BEGIN_DECLS

// `sig_atomic_t` - тип для атомарных операций с сигналами.
typedef int sig_atomic_t;

// Устанавливает функцию `handler` как обработчик сигнала `signal`.
void (*signal(int signal, void (*handler)(int)))(int);

// Генерирует сигнал `sig`.
int raise(int sig);

END_DECLS

#endif