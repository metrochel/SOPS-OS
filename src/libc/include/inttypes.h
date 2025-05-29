//
//  inttypes.h - Макросы для целых чисел
//
//  Предоставляет макросы для форматированного вывода чисел.
//

#if !defined _INTTYPES_INCL && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L
#define _INTTYPES_INCL 1

#include <stdint.h>
#include <etc/decl.h>

// Макросы для printf

#define PRId8 "d"
#define PRIdLEAST8 "d"
#define PRIdFAST8 "d"
#define PRId16 "d"
#define PRIdLEAST16 "d"
#define PRIdFAST16 "d"
#define PRId32 "d"
#define PRIdLEAST32 "d"
#define PRIdFAST32 "d"
#define PRId64 "d"
#define PRIdLEAST64 "d"
#define PRIdFAST64 "d"
#define PRIdMAX "d"
#define PRIdPTR "d"

#define PRIi8 "i"
#define PRIiLEAST8 "i"
#define PRIiFAST8 "i"
#define PRIi16 "i"
#define PRIiLEAST16 "i"
#define PRIiFAST16 "i"
#define PRIi32 "i"
#define PRIiLEAST32 "i"
#define PRIiFAST32 "i"
#define PRIi64 "i"
#define PRIiLEAST64 "i"
#define PRIiFAST64 "i"
#define PRIiMAX "i"
#define PRIiPTR "i"

#define PRIu8 "u"
#define PRIuLEAST8 "u"
#define PRIuFAST8 "u"
#define PRIu16 "u"
#define PRIuLEAST16 "u"
#define PRIuFAST16 "u"
#define PRIu32 "u"
#define PRIuLEAST32 "u"
#define PRIuFAST32 "u"
#define PRIu64 "u"
#define PRIuLEAST64 "u"
#define PRIuFAST64 "u"
#define PRIuMAX "u"
#define PRIuPTR "u"

#define PRIo8 "o"
#define PRIoLEAST8 "o"
#define PRIoFAST8 "o"
#define PRIo16 "o"
#define PRIoLEAST16 "o"
#define PRIoFAST16 "o"
#define PRIo32 "o"
#define PRIoLEAST32 "o"
#define PRIoFAST32 "o"
#define PRIo64 "o"
#define PRIoLEAST64 "o"
#define PRIoFAST64 "o"
#define PRIoMAX "o"
#define PRIoPTR "o"

#define PRIx8 "x"
#define PRIxLEAST8 "x"
#define PRIxFAST8 "x"
#define PRIx16 "x"
#define PRIxLEAST16 "x"
#define PRIxFAST16 "x"
#define PRIx32 "x"
#define PRIxLEAST32 "x"
#define PRIxFAST32 "x"
#define PRIx64 "x"
#define PRIxLEAST64 "x"
#define PRIxFAST64 "x"
#define PRIxMAX "x"
#define PRIxPTR "x"

#define PRIX8 "X"
#define PRIXLEAST8 "X"
#define PRIXFAST8 "X"
#define PRIX16 "X"
#define PRIXLEAST16 "X"
#define PRIXFAST16 "X"
#define PRIX32 "X"
#define PRIXLEAST32 "X"
#define PRIXFAST32 "X"
#define PRIX64 "X"
#define PRIXLEAST64 "X"
#define PRIXFAST64 "X"
#define PRIXMAX "X"
#define PRIXPTR "X"

// Макросы для scanf

#define SCNd8 "d"
#define SCNdLEAST8 "d"
#define SCNdFAST8 "d"
#define SCNd16 "d"
#define SCNdLEAST16 "d"
#define SCNdFAST16 "d"
#define SCNd32 "d"
#define SCNdLEAST32 "d"
#define SCNdFAST32 "d"
#define SCNd64 "d"
#define SCNdLEAST64 "d"
#define SCNdFAST64 "d"
#define SCNdMAX "d"
#define SCNdPTR "d"

#define SCNi8 "i"
#define SCNiLEAST8 "i"
#define SCNiFAST8 "i"
#define SCNi16 "i"
#define SCNiLEAST16 "i"
#define SCNiFAST16 "i"
#define SCNi32 "i"
#define SCNiLEAST32 "i"
#define SCNiFAST32 "i"
#define SCNi64 "i"
#define SCNiLEAST64 "i"
#define SCNiFAST64 "i"
#define SCNiMAX "i"
#define SCNiPTR "i"

#define SCNu8 "u"
#define SCNuLEAST8 "u"
#define SCNuFAST8 "u"
#define SCNu16 "u"
#define SCNuLEAST16 "u"
#define SCNuFAST16 "u"
#define SCNu32 "u"
#define SCNuLEAST32 "u"
#define SCNuFAST32 "u"
#define SCNu64 "u"
#define SCNuLEAST64 "u"
#define SCNuFAST64 "u"
#define SCNuMAX "u"
#define SCNuPTR "u"

#define SCNo8 "o"
#define SCNoLEAST8 "o"
#define SCNoFAST8 "o"
#define SCNo16 "o"
#define SCNoLEAST16 "o"
#define SCNoFAST16 "o"
#define SCNo32 "o"
#define SCNoLEAST32 "o"
#define SCNoFAST32 "o"
#define SCNo64 "o"
#define SCNoLEAST64 "o"
#define SCNoFAST64 "o"
#define SCNoMAX "o"
#define SCNoPTR "o"

#define SCNx8 "x"
#define SCNxLEAST8 "x"
#define SCNxFAST8 "x"
#define SCNx16 "x"
#define SCNxLEAST16 "x"
#define SCNxFAST16 "x"
#define SCNx32 "x"
#define SCNxLEAST32 "x"
#define SCNxFAST32 "x"
#define SCNx64 "x"
#define SCNxLEAST64 "x"
#define SCNxFAST64 "x"
#define SCNxMAX "x"
#define SCNxPTR "x"

#define SCNX8 "X"
#define SCNXLEAST8 "X"
#define SCNXFAST8 "X"
#define SCNX16 "X"
#define SCNXLEAST16 "X"
#define SCNXFAST16 "X"
#define SCNX32 "X"
#define SCNXLEAST32 "X"
#define SCNXFAST32 "X"
#define SCNX64 "X"
#define SCNXLEAST64 "X"
#define SCNXFAST64 "X"
#define SCNXMAX "X"
#define SCNXPTR "X"

BEGIN_DECLS

// `idiv_t` - это тип, который возвращается функцией `imaxdiv`.
typedef struct {
    int quotient;       // Частное
    int remainder;      // Остаток
} idiv_t;

// Операция модуля (y = |x|).
intmax_t imaxabs(intmax_t x);

// Вычисляет результат деления `a` на `b`.
idiv_t imaxdiv(intmax_d a, intmax_d b);

// Переводит строку `str` в целое число по основанию `base`.
// После окончания записывает конец строки в `strEnd`, если `strEnd` ненулевой.
intmax_t strtoimax(const char *str, char **strEnd, int base);

// Переводит строку `str` в целое число по основанию `base`.
// После окончания записывает конец строки в `strEnd`, если `strEnd` ненулевой.
uintmax_t strtoumax(const char *str, char **strEnd, int base);

// Переводит строку `str` в целое число по основанию `base`.
// После окончания записывает конец строки в `strEnd`, если `strEnd` ненулевой.
intmax_t wcstoimax(const wchar_t *str, char **strEnd, int base);

// Переводит строку `str` в целое число по основанию `base`.
// После окончания записывает конец строки в `strEnd`, если `strEnd` ненулевой.
uintmax_t wcstoumax(const wchar_t *str, char **strEnd, int base);

END_DECLS

#endif