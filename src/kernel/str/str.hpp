//
//  Библиотека для работы со строками
//
//  - Позволяет работать со строками. А вы что думали?
//
#ifndef _STR_INCL
#define _STR_INCL

#include "../util/nums.hpp"

bool strcmp(char* str1, char* str2);
byte strcmpS(char* str1, char* str2);
byte strcmpS(char* str1, char* str2, bool caseInsensitive);

bool strstartswith(char* str, char* substr);

bool strendswith(char* str, char* substr);

dword strcpy(char* str1, char* str2);

dword strlen(char* str);

byte numasstr(dword num, char* str);

char** strsplit(char* str, char* pattern);

char chartranslit(word c);

#endif