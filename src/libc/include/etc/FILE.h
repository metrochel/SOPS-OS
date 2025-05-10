//
//	FILE.h - вспомогательный файл, определяющий структуру FILE.
//
//	Просто определяет структуру FILE и ничего больше.
//

#ifndef _FILE_INCL
#define _FILE_INCL
typedef struct {
	unsigned int descriptor;
	unsigned int pos;			// FIXME: Заменить на fpos_t
	unsigned char error;
	unsigned char eof;
	char *buffer;
} FILE;
#endif