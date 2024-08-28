//
//  Библиотека для работы со строками
//
//  - Позволяет работать со строками. А вы что думали?
//
#ifndef _STR_INCL
#define _STR_INCL

#include "../util/nums.hpp"

byte strcmp(char* str1, char* str2);

bool strstartswith(char* str, char* substr);

bool strendswith(char* str, char* substr);

byte strcpy(char* str1, char* str2);

dword strlen(char* str);

byte numasstr(dword num, char* str);

#endif