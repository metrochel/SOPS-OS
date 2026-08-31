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

// `size_t` - это число, достаточное, чтобы указать размер чего-либо.
typedef unsigned long size_t;

// Размер указателя
#define ptrsize sizeof(ptrint)

// 0-й байт числа
#define BYTE0(n)    (byte)((n) & 0xFF)
// 1-й байт числа
#define BYTE1(n)    (byte)(((n) >> 8) & 0xFF)
// 2-й байт числа
#define BYTE2(n)    (byte)(((n) >> 16) & 0xFF)
// 3-й байт числа
#define BYTE3(n)    (byte)(((n) >> 24) & 0xFF)
// 4-й байт числа
#define BYTE4(n)    (byte)(((n) >> 32) & 0xFF)
// 5-й байт числа
#define BYTE5(n)    (byte)(((n) >> 40) & 0xFF)
// 6-й байт числа
#define BYTE6(n)    (byte)(((n) >> 48) & 0xFF)
// 7-й байт числа
#define BYTE7(n)    (byte)(((n) >> 56) & 0xFF)

// 0-е слово в числе
#define WORD0(n)    (word)(((n) >> 0) & 0xFFFF)
// 1-е слово в числе
#define WORD1(n)    (word)(((n) >> 16) & 0xFFFF)
// 2-е слово в числе
#define WORD2(n)    (word)(((n) >> 32) & 0xFFFF)
// 3-е слово в числе
#define WORD3(n)    (word)(((n) >> 48) & 0xFFFF)

// Низшее двойное слово в числе
#define LDWORD(n)   (dword)((n) & maxdword)
// Высшее двойное слово в числе
#define HDWORD(n)   (dword)(((n) >> 32) & maxdword)

// Младший байт числа
#define LSB(n)  (byte)((n) & 0xFF)
// Старший байт числа
#define MSB(n)  (byte)(((n) >> ((sizeof(n) - 1) * 8)) & 0xFF)

// Младший бит числа
#define lsb(n) (byte)((n) & 0x01)
// Старший бит числа
#define msb(n) (byte)(((n) >> ((sizeof(n) * 8 - 1))) & 0x01)

#define test_bit(n, bit) ((n) & (((decltype(n))(1)) << (bit)))

#endif