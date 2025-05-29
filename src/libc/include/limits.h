//
//  limits.h - Целочисленные пределы
//
//  Описывает максимально возможные целочисленные
//  значения для различных типов данных.
//

#define CHAR_BIT __CHAR_BIT__

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 1
#endif

#define SCHAR_MAX __SHAR_MAX__
#define SCHAR_MIN (-__SCHAR_MAX__ - 1)
#define UCHAR_MAX (2 * __SCHAR_MAX__ + 1)
#define UCHAR_MIN 0

#define SHRT_MAX __SHRT_MAX__
#define SHRT_MIN (-__SHRT_MAX__ - 1)
#define USHRT_MAX (2 * __SHRT_MAX__ + 1)
#define USHRT_MIN 0

#define INT_MAX __INT_MAX__
#define INT_MIN (-__INT_MAX__ - 1)
#define UINT_MAX (2 * __INT_MAX__ + 1)
#define UINT_MIN 0

#define LONG_MAX __LONG_MAX__
#define LONG_MIN (-__LONG_MAX__ - 1)
#define ULONG_MAX (2 * __LONG_MAX__ + 1)
#define ULONG_MIN 0

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199900L \
    || defined __cplusplus && __cplusplus >= 201100L

#define LLONG_MAX __LONG_LONG_MAX__
#define LLONG_MIN (-__LONG_LONG_MAX - 1LL)
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)
#define ULLONG_MIN 0
    
#endif

#if __CHAR_UNSIGNED__
#define CHAR_MIN UCHAR_MIN
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#endif

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 202300L

#define CHAR_WIDTH CHAR_BIT
#define UCHAR_WIDTH CHAR_BIT
#define SCHAR_WIDTH CHAR_BIT

#define SHRT_WIDTH __SHRT_WIDTH__
#define USHRT_WIDTH __SHRT_WIDTH__

#define INT_WIDTH __INT_WIDTH__
#define UINT_WIDTH __INT_WIDTH__

#define LONG_WIDTH __LONG_WIDTH__
#define ULONG_WIDTH __LONG_WIDTH__

#define LLONG_WIDTH __LLONG_WIDTH__
#define ULLONG_WIDTH __LLONG_WIDTH__

#define BITINT_MAXWIDTH __BITINT_MAXWIDTH__

#define BOOL_WIDTH __BOOL_WIDTH__

#endif