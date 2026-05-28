//
//  Библиотека для работы со строками
//
//  - Позволяет работать со строками. А вы что думали?
//
#ifndef _STR_INCL
#define _STR_INCL

#include "../libk/nums.hpp"

bool strcmp(const char* str1, const char* str2);
byte strcmpS(const char* str1, const char* str2);
byte strcmpS(const char* str1, const char* str2, bool caseInsensitive);

bool strstartswith(const char* str, const char* substr);

bool strendswith(const char* str, const char* substr);

dword strcpy(const char* str1, char* str2);

dword strlen(const char* str);

byte numasstr(dword num, char* str);

char** strsplit(const char* str, const char* pattern);

void strconcat(const char *str1, const char *str2, char *&str);

void strskiplines(char *&str, dword lines);

byte get_utf8_char_sz(const char *ptr);

qword strhextoint(const char *str);
qword strdectoint(const char *str);

#endif