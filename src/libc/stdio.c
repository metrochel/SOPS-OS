#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/string.h"
#include "etc/syscalls.h"

FILE __stdout = {};
FILE __stdin  = {};
FILE __stderr = {};

FILE files[FOPEN_MAX];
unsigned char files_taken[FOPEN_MAX];

#define FILE_DESC_STDOUT    0
#define FILE_DESC_STDIN     1
#define FILE_DESC_STDERR    2
#define FILE_DESC_INVALID   255

#define __is_buffered(stream) ((((stream)->buffer_mode == _IOLBF) || ((stream)->buffer_mode == _IOFBF)) && (stream)->buffer)
#define __invalid_stream(stream) ((stream)->descriptor == FILE_DESC_INVALID)

#define __increment_pos(arg_pos) \
    if ((arg_pos).pos == (arg_pos).end) (arg_pos).end++; (arg_pos).pos++;

// === Открытие/закрытие файла ===

int generate_fopen_mode(const char *mode) {
    // Режим открытия файла описан в src/kernel/file/filebase.hpp.
    struct {
        int read_allow : 1;
        int write_allow : 1;
        int append : 1;
        int binary : 1;
        int exclusive : 1;
    } open_mode;
    while (*mode) {
        char c = *mode++;
        switch (c) {
            case 'r': open_mode.read_allow = 1; break;
            case 'w': open_mode.write_allow = 1; break;
            case 'a': open_mode.append = 1; open_mode.write_allow = 1; break;
            default: break;
        }

        if (c == 'b')
            open_mode.binary = 1;

        if (c == '+') {
            open_mode.read_allow = 1;
            open_mode.write_allow = 1;
        }

        if (c == 'x') {
            open_mode.exclusive = 1;
        }
    }

    return *(int*)&open_mode;
}

int take_file_spot() {
    for (int handle = 0; handle < FOPEN_MAX; handle ++) {
        if (files_taken[handle] == 0) {
            files_taken[handle] = 1;
            return handle;
        }
    }

    return -1;
}

void free_file_spot(int spot) {
    if (spot < 0 || spot >= FOPEN_MAX) return;
    files_taken[spot] = 0;
}

FILE* fopen(const char *fname, const char *strmode) {
    int spot = take_file_spot();
    if (spot == -1)
        return NULL;

    int mode = generate_fopen_mode(strmode);

    int syscall_result = syscall2(syscall_open_file, (int)fname, mode);
    if (syscall_result < 0) {
        free_file_spot(spot);
        return NULL;
    }

    FILE *fileptr = files + spot;

    fileptr->descriptor = syscall_result;
    fileptr->error = 0;
    fileptr->eof = 0;
    fileptr->buffer = NULL;
    fileptr->pos = START_FPOS;
    fileptr->ungetc_buf = malloc(UNGETC_MAX_BUF);
    fileptr->ungetc_idx = 0;

    return fileptr;
}

FILE* freopen(const char *fname, const char *strmode, FILE *handle) {
    int mode = generate_fopen_mode(strmode);

    int syscall_result = syscall1(syscall_close_file, handle->descriptor);
    if (syscall_result != 0)
        return NULL;

    syscall_result = syscall2(syscall_open_file, SYSCALL_ARG(fname), mode);
    if (syscall_result == -1)
        return NULL;

    handle->descriptor = syscall_result;
    handle->error = 0;
    handle->eof = 0;
    handle->buffer = NULL;
    handle->pos = START_FPOS;

    return handle;
}

int fclose(FILE *file) {
    int syscall_result = syscall1(syscall_close_file, file->descriptor);
    if (syscall_result != 0)
        return EOF;

    int spot = files - file;
    free_file_spot(spot);
    return 0;
}

// === Чтение и запись ===

size_t fread(void *ptr, size_t sz, size_t n, FILE *stream) {
    if (sz == 0 || n == 0) {
        return 0;
    }

    size_t read_size = sz * n;
    fpos_t read_start = stream->pos;
    int desc = stream->descriptor;

    size_t read_objects;
    if (sz == 1) {
        char *buf = (char*)ptr;
        while (stream->ungetc_idx && read_size > 0) {
            *buf++ = stream->ungetc_buf[--stream->ungetc_idx];
            read_size--;
        }
        ptr = buf;
        if (read_size) {
            ptr = (void*)buf;
            size_t syscall_result = syscall4(syscall_read, (int)ptr, read_size, read_start.pos, desc);
            if (syscall_result == (size_t)-1) {
                read_objects = 0;
            } else {
                read_objects = syscall_result;
                read_start.pos += syscall_result;
            }
        }
    } else {
        char *buf = ptr;
        for (size_t i = 0; (!feof(stream) && !ferror(stream)) && (i < n); i++) {
            int full_read = 1;
            for (size_t i = 0; i < sz; i++) {
                int c = fgetc(stream);
                if (c == EOF) {
                    full_read = 0;
                    break;
                }
                *buf++ = c;
            }
            if (full_read)
                read_objects++;
        }
    }

    stream->pos = read_start;
    return read_objects;
}

size_t fwrite(const void *ptr, size_t sz, size_t n, FILE *stream) {
    if (sz == 0 || n == 0) {
        return 0;
    }

    int desc = stream->descriptor;
    fpos_t write_start = stream->pos;

    size_t written_objects = 0;
    if (sz == 1) {
        size_t syscall_result = syscall4(syscall_write, (syscall_arg_t)ptr, sz * n, write_start.pos, desc);
        write_start.pos += syscall_result;
        written_objects = syscall_result;
    } else {
        for (size_t i = 0; i < n; i++) {
            size_t syscall_result = syscall4(syscall_write, (syscall_arg_t)ptr, sz, write_start.pos, desc);
            write_start.pos += syscall_result;
            if (syscall_result != sz)
                break;
            written_objects++;
        }
    }

    stream->pos = write_start;
    return written_objects;
}

int fgetc(FILE *stream) {
    if (!stream) return EOF;

    if (stream->eof) {
        stream->error = 1;
        return EOF;
    }

    if (stream->ungetc_idx) {
        return stream->ungetc_buf[--stream->ungetc_idx];
    }

    if (__is_buffered(stream)) {
        if (stream->buffer_idx == stream->buffer_active_size) {
            int syscall_result = syscall4(syscall_read,
                SYSCALL_ARG(stream->buffer),
                stream->buffer_size,
                stream->pos.pos,
                stream->descriptor);
            if (syscall_result <= 0) {
                stream->error = 1;
                return EOF;
            }
            stream->buffer_idx = 0;
            stream->buffer_active_size = syscall_result;
            if (stream->buffer_active_size < stream->buffer_size)
                stream->is_final_buf = 1;
        }

        int c = stream->buffer[stream->buffer_idx++];
        __increment_pos(stream->pos)
        if (stream->buffer_idx == stream->buffer_active_size && stream->is_final_buf)
            stream->eof = 1;

        return c;
    }

    int desc = stream->descriptor;
    fpos_t read_start = stream->pos;

    int syscall_result = syscall2(syscall_read_char, read_start.pos, desc);
    if (syscall_result == -1) {
        stream->eof = 1;
        return EOF;
    }
    if (syscall_result == -2) {
        stream->error = 1;
        return EOF;
    }

    __increment_pos(read_start)
    stream->pos = read_start;
    return syscall_result;
}

int getc(FILE *stream) {
    return fgetc(stream);
}

int getchar() {
    return fgetc(stdin);
}

int fputc(int c, FILE *stream) {
    if (!stream) return EOF;

    if (__is_buffered(stream)) {
        if (stream->buffer_idx == stream->buffer_size) {
            int flush_result = fflush(stream);
            if (flush_result) {
                return EOF;
            }
            stream->buffer_idx = 0;
        }

        stream->buffer[stream->buffer_idx++] = c;
        __increment_pos(stream->pos)

        return 0;
    }

    int desc = stream->descriptor;
    fpos_t write_start = stream->pos;

    int syscall_result = syscall3(syscall_write_char, c, write_start.pos, desc);
    if (syscall_result == -1) {
        stream->error = 1;
        return EOF;
    }

    __increment_pos(write_start)

    stream->pos = write_start;
    return syscall_result;
}

int putc(int c, FILE *stream) {
    return fputc(c, stream);
}

int putchar(int c) {
    return fputc(c, stdout);
}

int ungetc(int c, FILE *stream) {
    if (stream->ungetc_idx < UNGETC_MAX_BUF) {
        stream->ungetc_buf[stream->ungetc_idx++] = c;
        return c;
    } else
        return EOF;
}

char *fgets(char *s, int n, FILE *stream) {
    char *_s = s;
    int c = fgetc(stream);
    if (feof(stream)) {
        return NULL;
    }
    *s++ = c;
    for (int i = 1; i < n - 1; i++) {
        c = fgetc(stream);
        if (c == '\n' || c == EOF)
            break;
        *s++ = c;
    }
    *s = 0;
    return _s;
}

int fputs(const char *s, FILE *stream) {
    if (!s) return EOF;
    if (!stream) return EOF;

    if (stream->error)
        return EOF;

    size_t len = strlen(s);
    int result = fwrite(s, 1, len, stream);
    return result;
}

int puts(const char *str) {
    return fputs(str, stdout);
}

[[gnu::always_inline]] int __flush_stream(FILE *stream) {
    if (__invalid_stream(stream)) return EOF;

    fpos_t *pos = &(stream->pos);

    size_t bytes_to_write = pos->pos - pos->file_pos;
    size_t write_start = pos->file_pos;

    size_t syscall_result = syscall4(syscall_write,
        SYSCALL_ARG(stream->buffer),
        bytes_to_write,
        write_start,
        stream->descriptor);

    if (syscall_result <= 0 || syscall_result < bytes_to_write) {
        stream->error = 1;
        return EOF;
    }

    return 0;
}

int fflush(FILE *stream) {
    if (!stream) {
        for (int i = 0; i < FOPEN_MAX; i++) {
            __flush_stream(&files[i]);
        }
        return 0;
    }
    else
        return __flush_stream(stream);
}

int fgetpos(FILE *stream, fpos_t *pos) {
    if (!pos || !stream) return EOF;
    *pos = stream->pos;
    return 0;
}

int fsetpos(FILE *stream, const fpos_t *pos) {
    if (!pos || !stream) return EOF;
    stream->pos = *pos;
    return 0;
}

long ftell(FILE *stream) {
    if (!stream) return EOF;
    return stream->pos.pos;
}

int fseek(FILE *stream, long offset, int whence) {
    if (!stream) return EOF;
    fpos_t new_pos = stream->pos;
    switch (whence) {
        case SEEK_SET:
            new_pos.pos = offset;
            break;
        case SEEK_CUR:
            if (offset == 0 || stream->binary) new_pos.pos += offset;
            else return EOF;
            break;
        case SEEK_END:
            if (offset == 0 || stream->binary) new_pos.pos = new_pos.end + offset;
            else return EOF;
            break;
        default: return EOF;
    }
    stream->pos = new_pos;
    return 0;
}

int rewind(FILE *stream) {
    return fseek(stream, 0, SEEK_SET);
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
    if (!stream) return EOF;
    if (!buf) {
        stream->buffer = realloc(stream->buffer, size);
    } else {
        stream->buffer = buf;
        stream->buffer_mode = mode;
        stream->buffer_size = size;
    }
    return 0;
}

void setbuf(FILE *stream, char *buf) {
    if (!stream) return;
    if (!buf)
        setvbuf(stream, NULL, _IONBF, 0);
    else
        setvbuf(stream, buf, _IOFBF, BUFSIZ);
}

int feof(FILE *stream) {
    return stream->eof;
}

int ferror(FILE *stream) {
    return stream->error;
}

void perror(const char *s) {
    fputs(s, stderr);
}

void clearerr(FILE *stream) {
    if (!stream) return;
    stream->eof = 0;
    stream->error = 0;
}

int remove(const char *filename) {
    int result = syscall1(syscall_remove_file, SYSCALL_ARG(filename));
    return result;
}

int rename(const char *oldname, const char *newname) {
    int result = syscall2(syscall_move_file, SYSCALL_ARG(oldname), SYSCALL_ARG(newname));
    return result;
}

int __allocate_tmpfile() {
    // TODO
    return -1;
}

char* tmpnam(char *buf) {
    // Формат названия временного файла см. в файле stdio.h.

    const char tmpnam_format[] = "$TMPDIR/_tmp_XXXXX";

    int syscall_result = __allocate_tmpfile();
    if (syscall_result == -1)
        return NULL;

    strcpy(buf, tmpnam_format);
    char *number_ptr = (char*)tmpnam_format + sizeof tmpnam_format - __TMPNAM_NUMBERS_LEN - 1;

    int number = syscall_result;
    for (int i = __TMPNAM_NUMBERS_LEN - 1; i >= 0; i--) {
        int digit = (number & 0xF) + '0';
        if (digit > '9')
            digit = (number & 0xF) - 10 + 'A';
        number_ptr[i] = digit;
    }

    return buf;
}

FILE* tmpfile() {
    char tmpnam_buf[L_tmpnam];
    char *result = tmpnam(tmpnam_buf);
    if (result == NULL)
        return NULL;

    return fopen(tmpnam_buf, "wb+");
}


