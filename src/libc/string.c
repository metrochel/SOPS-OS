/*
 * string.c
 *
 */

/* Этой чёрной магии я научился, когда начал читать код glibc.
 * Благодаря ловкости рук и магии препроцессора мы можем не дублировать код
 * для string.c и wchar.c, а просто сделать макросы.
 */

#include "include/string.h"
#include "include/errno.h"
#include "include/ctype.h"
#include "etc/errmsgs.h"

#define CHAR_T                  char
#define SINGLE_CHAR_T           int
#define COLL_T                  coll_t
#define VOID_T                  void

#define GET_CHAR_LEN_FUNC       get_utf8_char_len
#define GET_CHAR_FUNC           get_utf8_char
#define WRITE_CHAR_FUNC         write_utf8_char

#define STRCPY_FUNC             strcpy
#define STRNCPY_FUNC            strncpy
#define STRCAT_FUNC             strcat
#define STRNCAT_FUNC            strncat
#define STRDUP_FUNC             strdup
#define STRNDUP_FUNC            strndup
#define STRCOLL_FUNC            strcoll
#define STRXFRM_FUNC            strxfrm
#define STRLEN_FUNC             strlen
#define STRCMP_FUNC             strcmp
#define STRNCMP_FUNC            strncmp
#define STRCHR_FUNC             strchr
#define STRRCHR_FUNC            strrchr
#define STRSPN_FUNC             strspn
#define STRCSPN_FUNC            strcspn
#define STRPBRK_FUNC            strpbrk
#define STRSTR_FUNC             strstr
#define STRTOK_FUNC             strtok
#define MEMCHR_FUNC             memchr
#define MEMCMP_FUNC             memcmp
#define MEMSET_FUNC             memset
#define MEMSET_EXPLICIT_FUNC    memset_explicit
#define MEMCPY_FUNC             memcpy
#define MEMMOVE_FUNC            memmove
#define MEMCCPY_FUNC            memccpy
#define STRERROR_FUNC           strerror

#define ISLOWER                 islower
#define ISUPPER                 isupper
#define TOLOWER                 tolower

#include ".no-compile/string_base.c"

// Нам нужно всё равно прописать пару функций, потому что они отличаются от
// wchar.c.

char *__strtok_internal_state = NULL;

char* STRTOK_FUNC(char *str, const char *delim) {
    if (!str)
        str = __strtok_internal_state;

    size_t cspn = strcspn(str, delim);
    if (cspn == strlen(str))
        return NULL;

    str[cspn - 1] = 0;

    size_t new_int_size = strlen(str + cspn);
    __strtok_internal_state = realloc(__strtok_internal_state, new_int_size + 1);
    strcpy(__strtok_internal_state, str + cspn);
    __strtok_internal_state[new_int_size] = 0;
    return str;
}

#define errcase(err) case err: return (char*)STR_##err;

char* STRERROR_FUNC(int errnum) {
    switch (errnum) {
        errcase(EDOM)
        errcase(ERANGE)
        errcase(EILSEQ)
    }
    return NULL;
}