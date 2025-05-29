//
//  stdio.h - Заголовок для ввода/вывода
//
//	Предоставляет функции для взаимодействия с пользователем:
//	консольный ввод/вывод, программы и т.д.
//

// Тут тоже будет микро-спагетти, по большому счёту из-за одной структуры - FILE.
#ifndef _STDIO_INCL

// Если вспомогательный макрос __need_FILE не определён, то заголовок включает программа,
// а значит, ей нужно всё из него.
#if !defined(__need_FILE) && !defined(__need_fpos_t)
#define _STDIO_INCL 1
#define __need_FILE
#define __need_fpos_t
#endif

#include <etc/decl.h>

#if defined(__need_FILE)
#include <etc/FILE.h>
#undef __need_FILE
#endif

#if defined(__need_fpos_t)
#include <etc/fpos_t.h>
#undef __need_fpos_t
#endif

#include <stdarg.h>

#ifdef _STDIO_INCL

// Из stddef.h нам потребуется size_t.
#define __need_size_t
#include <stddef.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Конец файла
#define EOF -1

// Максимальное число открытых одновременно файлов
#define FOPEN_MAX 32
// Максимальная длина имени файла
#define FILENAME_MAX 1024

// Размер буфера, используемый в функции `setbuf`
#define BUFSIZ 4096
// Аргумент функции `setbuf`, означающий полную буферизацию потока
#define _IOFBF 0
// Аргумент функции `setbuf`, означающий построчную буферизацию потока
#define _IOLBF 1
// Аргумент функции `setbuf`, означающий отсутствие буферизации потока
#define _IONBF 2

// Пусть наш временный файл будет иметь формат "$TMPDIR/_tmp_XXXXX", где X - это шестнадцатеричная цифра.
// Тогла L_tmpnam будет равен 19 (не забываем 0x00 на конце), а TMP_MAX будет равен 16^5 = 1 048 576.

// Максимально возможное число имён для временного файла
#define TMP_MAX 1048576
// Размер буфера для имени временного файла
#define L_tmpnam 19

BEGIN_DECLS

// Резервированный поток, отвечающий за стандартный вывод.
extern FILE *stdout;
// Резервированный поток, отвечающий за стандартный ввод.
extern FILE *stdin;
// Резервированный поток, отвечающий за стандартный вывод ошибки.
extern FILE *stderr;

// Открывает файл по имени `filename` в режиме `mode`.
FILE* fopen(const char* filename, const char* mode);

// Закрывает поток `stream` и открывает его снова.
FILE *freopen(const char *filename, const char *mode, FILE *stream);

// Закрывает файл в потоке `stream`.
int fclose(FILE* stream);

// Записывает все данные из потока вывода на устройство.
int fflush(FILE *stream);

// Устанавливает буфер `buf` для файла `stream`.
void setbuf(FILE *stream, char* buf);

// Устанавливает буфер `buf` размером `size` в режиме `mode` для потока `stream`.
void setvbuf(FILE *stream, char *buf, int mode, size_t size);

// Считывает `nmemb` объектов размером `size` Б из потока `stream` и записывает их в указатель `ptr`.
size_t fread(void *ptr, size_t size, size_t nmemb, FILE* stream);

// Записывает `nmemb` объектов размером `size` Б из буфера `ptr` в поток `stream`.
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

// Извлекает один символ из потока `stream`.
int fgetc(FILE* stream);

// Извлекает один символ из потока `stream`.
int getc(FILE* stream);

// Извлекает строку длиной `n` из потока `stream` и записывает её в `s`.
char *fgets(char *s, int n, FILE *stream);

// Записывает символ `c` в поток `stream`.
int fputc(int c, FILE *stream);

// Записывает строку `str` в поток `stream`.
int fputs(const char *str, FILE *stream);

// Берёт один символ из `stdin`.
int getchar();

// Функция была удалена в C++14, соответственно, до него она должна быть.
#if !defined(__cplusplus) || __cplusplus <= 201400L

// Считывает строку из `stdin` и записывает в `str`.
char *gets(char *str);

#endif

// Записывает символ `c` в `stdout`.
int putchar(int c);

// Записывает строку `str` в `stdout`.
int puts(const char *str);

// Кладёт символ `c` обратно в `stream`.
int ungetc(int c, FILE *stream);

// Изменяет положение указателя в файле.
int fseek(FILE* stream, long offset, int whence);

// Считывает строку из `stdin` и форматирует её в соответствии с `format`.
int scanf(const char *format, ...);

// Считывает строку из потока `stream` и форматирует её в соответствии с `format`.
int fscanf(FILE *stream, const char *format, ...);

// Считывает строку из буфера `buf` и форматирует её в соответствии с `format`.
int sscanf(const char *buf, const char *format, ...);

// Следующие функции определены только после C++11.
#if defined(__cplusplus) && __cplusplus >= 201100L || defined(__STDC_VERSION__)

// Считывает строку из `stdin` и форматирует её в соответствии с `format`.
// Вместо перечисления аргументов использует `args`.
int vscanf(const char *format, va_list args);

// Считывает строку из потока `stream` и форматирует её в соответствии с `format`.
// Вместо перечисления аргументов использует `args`.
int vfscanf(FILE *stream, const char *format, va_list args);

// Считывает строку из буфера `buf` и форматирует её в соответствии с `format`.
// Вместо перечисления аргументов использует `args`.
int vsscanf(const char *buf, const char *format, va_list args);

#endif

// Форматирует строку в соответствии с `format` и выводит её в `stdout`.
int printf(const char *format, ...);

// Форматирует строку в соответствии с `format` и выводит её в поток `stream`.
int fprintf(FILE *stream, const char *format, ...);

// Форматирует строку в соответствии с `format` и выводит её в буфер `str`.
int sprintf(char *str, const char *format, ...);

#if defined(__cplusplus) && __cplusplus >= 201100L || defined(__STDC_VERSION__)

// Форматирует строку в соответствии с `format` и выводит её в буфер `str`.
// Длина форматированной строки не превышает `maxlen`.
int snprintf(char *s, size_t maxlen, const char *format, ...);

#endif

// Получает положение указателя на текущий символ в файле.
long int ftell(FILE *stream);

// Получает положение потока `stream` и записывает его в `fpos`.
int fgetpos(FILE *stream, fpos_t *fpos);

// Проверяет, возникла ли ошибка в потоке `stream`.
int ferror(FILE *stream);

// Выводит строку `str` в `stderr`.
void perror(const char *str);

// Проверяет, достиг ли поток `stream` своего конца (то есть проверяет на EOF).
int feof(FILE *stream);

// Удаляет файл `filename`.
int remove(const char *filename);

// Переименовывает файл `filename` в файл `newfilename`.
int rename(const char *filename, const char *newfilename);

// Открывает временный файл, автоматически закрывающийся по завершении программы.
FILE* tmpfile();

// Генерирует имя для временного файла и записывает его в буфер `str`.
char *tmpnam(char *str);

END_DECLS

#endif

#endif