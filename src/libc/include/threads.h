//
//  threads.h - Управление потоками
//
//  Позволяет создать многопоточную среду и контроллировать её.
//

#if !defined(_THREADS_INCL) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L
#define _THREADS_INCL 1

#include <etc/decl.h>

BEGIN_DECLS

/* Состояния потоков */

// Успех
#define thrd_success 0
// Время ожидания истекло
#define thrd_timedout 1
// Занят
#define thrd_busy 2
// Недостаточно памяти
#define thrd_nomem 3
// Ошибка
#define thrd_error 4

/* Типы мьютексов */

// Обычный мьютекс
#define mtx_plain 0
// Рекурсивный мьютекс
#define mtx_recursive 1
// Ограниченный по времени мьютекс
#define mtx_timed 2

// Наибольшее количество раз, которые вызывается деструктор поточного хранилища
#define TSS_DTOR_ITERATIONS 4

// `thrd_t` - это структура, позволяющая однозначно описать один поток.
typedef struct {
    int id;
    int state;
} thrd_t;

// `mtx_t` - это структура, определяющая один мьютекс.
typedef struct {
    int id;
    int type;
    void *value;
} mtx_t;

// `cnd_t` - это структура, определяющая одну условную переменную.
typedef struct {
    
} cnd_t;

// `tss_t` - это структура, описывающая хранилище для одного потока (поточное хранилище).
typedef struct {
    
} tss_t;

// `tss_dtor_t` - это тип указателя на функцию-деструктор поточного хранилища.
typedef void (*tss_dtor_t)(void*);

// Создаёт новый поток, исполняющий функцию `start(args)`. При успехе записывает его данные в `thrd`.
int thrd_create(thrd_t *thrd, thrd_start_t start, void *args);

// Проверяет, относятся ли `t1` и `t2` к одному и тому же потоку.
int thrd_equal(thrd_t t1, thrd_t t2);

// Возвращает данные текущего потока.
thrd_t thrd_current();

// Приостанавливает текущий поток, пока не пройдёт время, указанное в `duration`.
// Если поток продолжится раньше, то оставшееся время будет записано в `remaining`.
int thrd_sleep(const struct timespec* duration, struct timespec *remaining);

// Отдаёт оставшееся для данного потока время обратно системе.
void thrd_yield();

// Завершает текущий поток с кодом `exit_code`.
void _Noreturn thrd_exit(int exit_code);

// Отсоединяет поток `thread`.
int thrd_detach(thrd_t thread);

// Блокирует исполнение текущего потока, пока поток `thread` не завершит исполнение.
// Если `exit` ненулевой, то код выхода `thread` будет записан в `exit`.
int thrd_join(thrd_t thread, int *exit);

// Инициализирует мьютекс `mtx` с типом `type`.
int mtx_init(mtx_t *mtx, int type);

// Блокирует исполнение текущего потока, пока мьютекс `mtx` не освободится.
int mtx_lock(mtx_t *mtx);

// Блокирует исполнение текущего потока, пока не освободится мьютекс `mtx` или
// пока не пройдёт время, указанное в `timeout`.
int mtx_timedlock(mtx_t* restrict mtx, const struct timespec* timeout);

// Пробует занять мьютекс `mtx`.
int mtx_trylock(mtx_t *mtx);

// Освобождает мьютекс `mtx`.
int mtx_unlock(mtx_t *mtx);

// Уничтожает мьютекс `mtx`.
void mtx_destroy(mtx_t *mtx);

// Вызывает функцию `func` ровно один раз.
void call_once(once_flag *flag, void (*func)(void));

// Инициализирует новую условную переменную `cnd`.
int cnd_init(cnd_t *cnd);

// Разблокирует исполнение одного потока, ожидающий условную переменную `cnd`.
int cnd_signal(cnd_t *cnd);

// Разблокирует исполнение всех потоков, ожидающих условную переменную `cnd`.
int cnd_broadcast(cnd_t *cnd);

// Разблокирует мьютекс `mtx`, затем ждёт условную переменную `cnd`.
int cnd_wait(cnd_t *cnd, mtx_t *mtx);

// Разблокирует мьютекс `mtx`, затем ждёт условную переменную `cnd` или время, указанное в `time_point`.
int cnd_timedwait(cnd_t* restrict cnd, mtx_t* restrict mtx, const struct timespec* restrict time_point);

// Уничтожает условную переменную `cnd`.
void cnd_destroy(cnd_t *cnd);

#if __STDC_VERSION__ <= 202300L
#define thread_local _Thread_local
#endif

// Создаёт поточное хранилище и записывает его данные в `tss`.
int tss_create(tss_t* tss, tss_dtor_t destructor);

// Считывает из поточного хранилища `tss`.
void *tss_get(tss_t tss);

// Записывает буфер `val` в поточное хранилище `tss`.
int tss_set(tss_t tss, void *val);

// Удаляет поточное хранилище `tss`.
void tss_delete(tss_t tss);

END_DECLS

#endif