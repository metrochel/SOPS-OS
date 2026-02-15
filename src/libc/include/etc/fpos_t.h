//
//  fpos_t.h - Тип fpos_t
//
//  Определяет fpos_t - тип, указывающий на положение в потоке.
//

#ifndef _FPOS_T
#define _FPOS_T

#define START_FPOS ((fpos_t){0})

typedef struct {
    unsigned long pos;
    unsigned long end;
    unsigned long file_pos;
} fpos_t;

#endif