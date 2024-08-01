//
//  Библиотека для работы со строками
//
//  - Позволяет работать со строками. А вы что думали?
//
#include <stdint.h>

#define STR_SIG 9

bool strcmp(char* str1, char* str2);

bool strstartswith(char* str, char* substr);

bool strendswith(char* str, char* substr);

uint32_t strlen(char* str);