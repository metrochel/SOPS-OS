//
//	FILE.h - вспомогательный файл, определяющий структуру FILE.
//
//	Просто определяет структуру FILE и ничего больше.
//

#ifndef _FILE_INCL
#define _FILE_INCL

#include <etc/fpos_t.h>

#define __need_size_t
#include <stddef.h>

#define UNGETC_MAX_BUF 4096

typedef struct {
	unsigned char descriptor;
    unsigned short error         : 1;
    unsigned short eof           : 1;
    unsigned short binary        : 1;
    unsigned short is_final_buf  : 1;
    unsigned short _reserved     : 12;
	fpos_t pos;
	char *buffer;
    size_t buffer_size;
    size_t buffer_active_size;
    size_t buffer_idx;
    unsigned char buffer_mode;
    char *ungetc_buf;
    unsigned int ungetc_idx;
} FILE;

#endif