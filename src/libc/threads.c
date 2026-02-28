/*
 * threads.c
 *
 *
 */

#include "include/threads.h"
#include "include/time.h"
#include "include/stdlib.h"
#include "etc/syscalls.h"

#define INVALID_TIMESPEC ((struct timespec){-1, -1})

tss_t *tss_keys = NULL;
size_t tss_keys_amt = 0;

inline struct timespec compute_diff(const struct timespec *t1, const struct timespec *t2) {
    struct timespec dt;
    long dt_s = t2->tv_sec - t1->tv_nsec;
    long dt_ns = t2->tv_nsec - t1->tv_nsec;

    while (dt_ns < 0) {
        dt_ns += 1000000000;
        dt_s --;
    }

    if (dt_s < 0) {
        return INVALID_TIMESPEC;
    }

    dt.tv_sec = dt_s;
    dt.tv_nsec = dt_ns;
    return dt;
}

inline int is_timespec_invalid(const struct timespec *spec) {
    const struct timespec inv = INVALID_TIMESPEC;
    return spec->tv_sec == inv.tv_sec && spec->tv_nsec == inv.tv_nsec;
}

int thrd_create(thrd_t *thrd, thrd_start_t start, void *args) {
    if (!thrd) return thrd_error;

    int thrd_desc = syscall2(syscall_start_thread, (syscall_arg_t)start, (syscall_arg_t)args);
    if (!thrd_desc)
        return thrd_error;

    thrd->id = thrd_desc;
    thrd->state = thrd_active;
    return thrd_success;
}

int thrd_equal(thrd_t lt, thrd_t rt) {
    return (lt.id == rt.id) && (lt.state == rt.state);
}

thrd_t thrd_current() {
    int thrd_desc = syscall0(syscall_get_current_thread);

    thrd_t thrd;
    thrd.id = thrd_desc;
    thrd.state = thrd_active;

    return thrd;
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining) {
    unsigned long time_s = duration->tv_sec;
    unsigned long time_ns = duration->tv_nsec;

    struct timespec start;
    timespec_get(&start, 0);

    int syscall_result = syscall3(syscall_timed_block_thread, 0, time_s, time_ns);

    if (!syscall_result)
        return thrd_error;

    if (remaining) {
        struct timespec end;
        timespec_get(&end, 0);

        *remaining = compute_diff(&start, &end);
    }

    return thrd_success;
}

void thrd_yield() {
    syscall0(syscall_yield);
}

void noreturn thrd_exit(int result) {
    if (tss_keys) {
        for (size_t _ = 0; _ < TSS_DTOR_ITERATIONS; _++) {
            int values_remaining = 0;
            for (size_t i = 0; i < tss_keys_amt; i++) {
                tss_t tss = tss_keys[i];
                void *value = tss_get(tss);
                tss_set(tss, NULL);
                tss.dtor(value);
                if (tss_get(tss))
                    values_remaining = 1;
            }

            if (!values_remaining) break;
        }
    }

    syscall1(syscall_exit_thread, result);
}

int thrd_detach(thrd_t thrd) {
    return syscall1(syscall_detach_thread, thrd.id);
}

int thrd_join(thrd_t thrd, int *result) {
    int joined_thrd_exit = syscall1(syscall_join_thread, thrd.id);

    if (joined_thrd_exit == -0xFFFFFFFF)
        return thrd_error;

    if (result)
        *result = joined_thrd_exit;

    return thrd_success;
}

int mtx_init(mtx_t *mutex, int type) {
    if (!mutex) return thrd_error;
    if (type & ~(mtx_recursive | mtx_timed))
        return thrd_error;

    int syscall_result = syscall1(syscall_create_mutex, type);

    if (!syscall_result) return thrd_error;

    mutex->id = syscall_result;
    mutex->type = type;

    return thrd_success;
}

int mtx_lock(mtx_t *mtx) {
    if (!mtx) return thrd_error;

    int mtx_locked = syscall1(syscall_get_mutex_locked, mtx->id);
    if (mtx_locked) {
        syscall1(syscall_block_thread, mtx->id);
        syscall1(syscall_lock_mutex, mtx->id);
        return thrd_error;
    } else {
        int result = syscall1(syscall_lock_mutex, mtx->id);
        return result ? thrd_success : thrd_error;
    }
}

int mtx_timedlock(mtx_t *mtx, const struct timespec *time) {
    if (!time || !mtx) return thrd_error;

    struct timespec the_time;
    timespec_get(&the_time, TIME_UTC);
    struct timespec dt = compute_diff(&the_time, time);
    if (is_timespec_invalid(&dt)) return thrd_error;

    int mtx_locked = syscall1(syscall_get_mutex_locked, mtx->id);
    if (mtx_locked) {
        syscall3(syscall_block_thread, mtx->id, dt.tv_sec, dt.tv_nsec);
        syscall1(syscall_lock_mutex, mtx->id);
        return thrd_busy;
    } else {
        int result = syscall1(syscall_lock_mutex, mtx->id);
        return result ? thrd_success : thrd_error;
    }
}

int mtx_trylock(mtx_t *mtx) {
    if (!mtx) return thrd_error;

    int mtx_locked = syscall1(syscall_get_mutex_locked, mtx->id);
    if (mtx_locked)
        return thrd_busy;
    else {
        int result = syscall1(syscall_lock_mutex, mtx->id);
        return result ? thrd_success : thrd_error;
    }
}

int mtx_unlock(mtx_t *mtx) {
    if (!mtx) return thrd_error;

    int result = syscall1(syscall_unlock_mutex, mtx->id);
    return result ? thrd_success : thrd_error;
}

void mtx_destroy(mtx_t *mtx) {
    if (!mtx) return;

    syscall1(syscall_destroy_mutex, mtx->id);
}

void call_once(once_flag *flag, void (*func)(void)) {
    if (!flag || !func) return;

    if (*flag) return;

    *flag = 1;
    func();
}

int cnd_init(cnd_t *cnd) {
    if (!cnd) return thrd_error;

    int syscall_result = syscall0(syscall_create_cond_var);
    if (!syscall_result)
        return thrd_error;

    cnd->id = syscall_result;

    return thrd_success;
}

int cnd_signal(cnd_t *cnd) {
    if (!cnd) return thrd_error;

    int syscall_result = syscall1(syscall_signal_cond_var, cnd->id);

    return syscall_result ? thrd_success : thrd_error;
}

int cnd_broadcast(cnd_t *cnd) {
    if (!cnd) return thrd_error;

    int syscall_result = syscall1(syscall_broadcast_cond_var, cnd->id);

    return syscall_result ? thrd_success : thrd_error;
}

int cnd_wait(cnd_t *cnd, mtx_t *mtx) {
    if (!mtx || !cnd) return thrd_error;

    int result = mtx_unlock(mtx);
    if (result == thrd_error) return thrd_error;

    int syscall_result = syscall1(syscall_block_thread, cnd->id);
    if (!syscall_result)
        return thrd_error;

    result = mtx_lock(mtx);
    if (result == thrd_error) return thrd_error;

    return thrd_success;
}

int cnd_timedwait(cnd_t *cnd, mtx_t *mtx, const struct timespec *endpoint) {
    struct timespec the_time;
    timespec_get(&the_time, TIME_UTC);

    struct timespec dt = compute_diff(&the_time, endpoint);

    int result = mtx_unlock(mtx);
    if (result == thrd_error) return thrd_error;

    int syscall_result = syscall3(syscall_timed_block_thread, cnd->id, dt.tv_sec, dt.tv_nsec);
    if (!syscall_result)
        return thrd_error;

    result = mtx_lock(mtx);
    if (result == thrd_error) return thrd_error;

    return thrd_success;
}

void cnd_destroy(cnd_t *cnd) {
    syscall1(syscall_destroy_cond_var, cnd->id);
}

int tss_create(tss_t *tss, tss_dtor_t destructor) {
    if (!tss || !destructor) return thrd_error;

    tss_t *new_tss_keys = realloc(tss_keys, (++tss_keys_amt) * sizeof(*tss));
    if (!new_tss_keys) {
        tss_keys_amt--;
        return thrd_error;
    }
    tss_keys[tss_keys_amt - 1] = *tss;
    tss_keys[tss_keys_amt - 1].dtor = destructor;

    int syscall_result = syscall1(syscall_create_tss, (syscall_arg_t)tss->key);
    if (!syscall_result) return thrd_error;

    return thrd_success;
}

void* tss_get(tss_t tss_key) {
    syscall_arg_t syscall_result = syscall1(syscall_get_tss, (syscall_arg_t)tss_key.key);

    return (void*)syscall_result;
}

int tss_set(tss_t tss_key, void *value) {
    int syscall_result = syscall2(syscall_set_tss, (syscall_arg_t)tss_key.key, (syscall_arg_t)value);

    return syscall_result ? thrd_success : thrd_error;
}

void tss_delete(tss_t tss_key) {
    syscall1(syscall_destroy_tss, (syscall_arg_t)tss_key.key);
}