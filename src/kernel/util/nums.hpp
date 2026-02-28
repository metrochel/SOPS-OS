//
//  Определения типов чисел
//
//  - Пара макросов, при помощи которых удобнее обращаться к числам.
//

#ifndef _NUMS_INCL
#define _NUMS_INCL

// `byte` - это число размером 1 байт.
typedef unsigned char byte;
// `word` - это число размером 2 байта.
typedef unsigned short word;
// `dword` - это число размером 4 байта.
typedef unsigned int dword;
// `qword` - это число размером 8 байтов.
typedef unsigned long long qword;

// Наибольшее возможное значение `byte`
#define maxbyte     0xFF
// Наибольшее возможное значение `word`
#define maxword     0xFFFF
// Наибольшее возможное значение `dword`
#define maxdword    0xFFFFFFFF
// Наибольшее возможное значение `qword`
#define maxqword    0xFFFFFFFFFFFFFFFF

#define null 0

#ifdef __x86_64__
// `ptrint` - это число, имеющее размер одного указателя.
typedef qword ptrint;
#else
// `ptrint` - это число, имеющее размер одного указателя.
typedef dword ptrint;
#endif

// Размер указателя
#define ptrsize sizeof(ptrint)

#endif