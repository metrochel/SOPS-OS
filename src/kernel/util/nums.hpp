//
//  Определения типов чисел
//
//  - Пара макросов, при помощи которых удобнее обращаться к числам.
//

#ifndef _NUMS_INCL
#define _NUMS_INCL

#define byte unsigned char
#define word unsigned short
#define dword unsigned int
#define qword unsigned long long

#define maxbyte 0xFF
#define maxword 0xFFFF
#define maxdword 0xFFFFFFFF
#define maxqword 0xFFFFFFFFFFFFFFFF

#define null 0

#ifdef __x86_64__
typedef unsigned long long ptrint;
#else
typedef unsigned int ptrint;
#endif

#define ptrsize sizeof(ptrint)

#endif