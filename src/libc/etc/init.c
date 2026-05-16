#include "../include/etc/init.h"
#include "../include/stdlib.h"
#include "../include/stdio.h"

#define init_std_stream(stream, desc)                   \
    (stream)->descriptor = desc;                        \
    (stream)->error = 0;                                \
    (stream)->eof = 0;                                  \
    (stream)->binary = 0;                               \
    (stream)->is_final_buf = 0;                         \
    (stream)->pos = (fpos_t){};                         \
    (stream)->buffer = NULL;                            \
    (stream)->buffer_size = 0;                          \
    (stream)->buffer_active_size = 0;                   \
    (stream)->buffer_idx = 0;                           \
    (stream)->buffer_mode = _IONBF;                     \
    (stream)->ungetc_buf = malloc(UNGETC_MAX_BUF);      \
    (stream)->ungetc_idx = 0;

void init_std_streams() {
    init_std_stream(stdout, 0)
    init_std_stream(stdin, 1)
    init_std_stream(stderr, 2)
}

void init_libc(int argc, char *argv, int envc, char *envp) {
	init_std_streams();
}