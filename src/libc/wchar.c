/*
 * wchar.c
 * 
 * 
 */

/*
 * Этой чёрной магии я научился, когда начал читать код glibc.
 * Благодаря ловкости рук и магии препроцессора мы можем не дублировать код
 * для string.c и wchar.c, а просто сделать макросы.
 */

#include "include/wchar.h"
#include "include/wctype.h"
#include "include/stdio.h"

#define CHAR_T                  wchar_t
#define SINGLE_CHAR_T           wchar_t
#define COLL_T                  wcoll_t
#define VOID_T                  wchar_t

#define GET_CHAR_LEN_FUNC       get_utf32_char_len
#define GET_CHAR_FUNC           get_utf32_char
#define WRITE_CHAR_FUNC         write_utf32_char

#define STRCPY_FUNC             wcscpy
#define STRNCPY_FUNC            wcsncpy
#define STRCAT_FUNC             wcscat
#define STRNCAT_FUNC            wcsncat
#define STRDUP_FUNC             wcsdup
#define STRNDUP_FUNC            wcsndup
#define STRCOLL_FUNC            wcscoll
#define STRXFRM_FUNC            wcsxfrm
#define STRLEN_FUNC             wcslen
#define STRCMP_FUNC             wcscmp
#define STRNCMP_FUNC            wcsncmp
#define STRCHR_FUNC             wcschr
#define STRRCHR_FUNC            wcsrchr
#define STRSPN_FUNC             wcsspn
#define STRCSPN_FUNC            wcscspn
#define STRPBRK_FUNC            wcspbrk
#define STRSTR_FUNC             wcsstr
#define STRTOK_FUNC             wcstok
#define MEMCHR_FUNC             wmemchr
#define MEMCMP_FUNC             wmemcmp
#define MEMSET_FUNC             wmemset
#define MEMSET_EXPLICIT_FUNC    wmemset_explicit
#define MEMCPY_FUNC             wmemcpy
#define MEMMOVE_FUNC            wmemmove
#define MEMCCPY_FUNC            wmemccpy

#define ISLOWER                 iswlower
#define ISUPPER                 iswupper
#define TOLOWER                 towlower

#include ".no-compile/string_base.c"

// Нам нужно всё равно прописать пару функций, потому что они отличаются от
// string.c.

wchar_t *__wcstok_internal_state = NULL;

wchar_t* wcstok(wchar_t *str, const wchar_t *delim, wchar_t **str_end) {
    if (!str)
        str = __wcstok_internal_state;

    size_t cspn = wcscspn(str, delim);
    if (cspn == wcslen(str))
        return NULL;

    str[cspn - 1] = 0;

    size_t new_int_size = wcslen(str + cspn);
    __wcstok_internal_state = realloc(__wcstok_internal_state, (new_int_size + 1) * sizeof ((wchar_t)0));
    wcscpy(__wcstok_internal_state, str + cspn);
    __wcstok_internal_state[new_int_size] = 0;
    if (str_end) *str_end = str + cspn;
    return str;
}

#define CHAR_T          wchar_t
#define ISSPACE         iswspace
#define ISDIGIT         iswdigit
#define ISXDIGIT        iswxdigit
#define TOLOWER         towlower
#define TOUPPER         towupper
#define STRSTR          wcsstr
#define STRLEN          wcslen
#define STRPBRK         wcspbrk
#define STRTO           wcsto
#include ".no-compile/strtonum_base.c"

// Ещё надо определить пару функций, которых нет в string.h.

wint_t btowc(int c) {
    if (c == EOF) return WEOF;

    return (wint_t)(c);
}