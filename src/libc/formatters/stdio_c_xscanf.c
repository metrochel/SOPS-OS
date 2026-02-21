#include "../include/wchar.h"
#include "../include/wctype.h"

#define CHAR char
#define ISDIGIT isdigit
#define ISXDIGIT isxdigit
#define ISSPACE isspace
#define STRTOL strtol
#define SCANF scanf

#include "../.no-compile/stdio_c_xscanf.c"

#undef CHAR
#undef ISDIGIT
#undef ISXDIGIT
#undef ISSPACE
#undef STRTOL
#undef SCANF

#define CHAR wchar_t
#define ISDIGIT iswdigit
#define ISXDIGIT iswxdigit
#define ISSPACE iswspace
#define STRTOL wcstol
#define SCANF wscanf
#define WCHAR

#include "../.no-compile/stdio_c_xscanf.c"