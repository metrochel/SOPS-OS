#include "include/signal.h"
#include "include/stddef.h"
#include "include/stdlib.h"
#include "include/stdio.h"
#include "include/erc/errorcodes.h"

typedef void (*signal_handle_t)(int);

typedef struct {
    int sig_id;
    signal_handle_t sig_hdl;
} signal_desc_t;

signal_desc_t *signal_descs = NULL;
size_t defined_signals = 0;

void default_sigabrt_handle(int signal) {
    if (signal != SIGABRT) return;

    perror("Программа экстренно завершается!\n");
}

void default_sigfpe_handle(int signal) {
    if (signal != SIGFPE) return;

    perror("Произошла вычислительная ошибка\n");
    exit(EXIT_ERROR);
}

void default_sigill_handle(int signal) {
    if (signal != SIGILL) return;

    perror("Исполнена невозможная инструкция\n");
    exit(EXIT_ERROR);
}

void default_sigint_handle(int signal) {
    if (signal != SIGINT) return;

    perror("Исполнение прервано\n");
    exit(EXIT_ERROR);
}

void default_sigterm_handle(int signal) {
    if (signal != SIGTERM) return;

    perror("Исполнение экстренно остановлено\n");
    abort();
}

void default_sigsegv_handle(int signal) {
    if (signal != SIGSEGV) return;

    perror("Ошибка сегментации\n");
    exit(EXIT_ERROR);
}

const signal_desc_t default_signal_handlers[] = {
        {SIGABRT, default_sigabrt_handle},
        {SIGFPE,  default_sigfpe_handle},
        {SIGILL,  default_sigill_handle},
        {SIGINT,  default_sigint_handle},
        {SIGSEGV, default_sigsegv_handle},
        {SIGTERM, default_sigterm_handle}
};

inline const size_t default_sig_handles_count = sizeof default_signal_handlers / sizeof (signal_handle_t)NULL;

inline signal_handle_t get_signal_handle(int signal, signal_handle_t hdl_value) {
    size_t i;
    switch (hdl_value) {
        case SIG_DFL:
            for (i = 0; i < default_sig_handles_count; i++) {
                if (default_signal_handlers[i].sig_id == signal)
                    return default_signal_handlers[i].sig_hdl;
            }
            return SIG_ERR;
        case SIG_ERR:
            return SIG_ERR;
        case SIG_IGN:
            return SIG_IGN;
        default:
            return hdl_value;
    }
}

void (*signal(int signal, void (*handler)(int)))(int) {
    if (handler == SIG_ERR) return SIG_ERR;

    signal_desc_t sig_desc = {signal, get_signal_handle(signal, handler)};

    if (defined_signals && signal_descs) {
        for (size_t i = 0; i < defined_signals; i++) {
            if (signal_descs[i].sig_id == signal) {
                signal_handle_t prev_hdl = signal_descs[i].sig_hdl;
                signal_descs[i] = sig_desc;
                return prev_hdl;
            }
        }
    }

    signal_desc_t *new_sig_descs = realloc(signal_descs, ++defined_signals);
    if (!new_sig_descs)
        return SIG_ERR;

    signal_descs = new_sig_descs;
    signal_descs[defined_signals - 1] = sig_desc;
    return NULL;
}

int raise(int signal) {
    signal_handle_t handle;

    for (size_t i = 0; i < defined_signals; i++) {
        if (signal_descs[i].sig_id == signal) {
            handle = signal_descs[i].sig_hdl;
            break;
        }
    }

    if (!handle || handle == SIG_IGN)
        return -1;

    handle(signal);

    return 0;
}

