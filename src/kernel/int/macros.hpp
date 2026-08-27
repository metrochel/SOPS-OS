/*
 * macros.hpp - Вспомогательные макросы
 *
 * Объявляет вспомогательные макросы для библиотеки прерываний.
 */

#ifndef _INTS_MACROS_INCL
#define _INTS_MACROS_INCL

#define int_attr    __attribute__((interrupt))

#define isr(name)    int_attr void name([[maybe_unused]] int_frame *frame)

#endif //_INTS_MACROS_INCL
