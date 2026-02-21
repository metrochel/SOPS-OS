/*
 *  printf-функции
 *
 *  Реализует все printf-функции.
 */

#include "../include/wctype.h"
#include "../include/wchar.h"

#define CHAR char
#define ISDIGIT isdigit
#define PRINTF printf
#define STRTOL strtol
#define SNPRINTF

#include "../.no-compile/stdio_c_xprintf.c"

#undef CHAR
#undef ISDIGIT
#undef PRINTF
#undef STRTOL
#undef SNPRINTF

#define CHAR wchar_t
#define ISDIGIT iswdigit
#define STRTOL wcstol
#define PRINTF wprintf
#define WCHAR

#include "../.no-compile/stdio_c_xprintf.c"