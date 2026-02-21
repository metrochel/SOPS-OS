//
//  intmanip.h - Управление целыми числами
//
//  Объявляет вспомогательные макросы, позволяющие извлекать
//  части из целых чисел.
//

#ifndef _INTMANIP_H
#define _INTMANIP_H

#define BYTE0(n)    ((n) & 0xFF)
#define BYTE1(n)    (((n) >> 8) & 0xFF)
#define BYTE2(n)    (((n) >> 16) & 0xFF)
#define BYTE3(n)    (((n) >> 24) & 0xFF)

#endif
