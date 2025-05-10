//
//  stdio.h - Заголовок для ввода/вывода
//
//	Предоставляет функции для взаимодействия с пользователем:
//	консольный ввод/вывод, программы и т.д.
//

// Тут тоже будет микро-спагетти, по большому счёту из-за одной структуры - FILE.

// Если вспомогательный макрос __need_FILE не определён, то заголовок включает программа,
// а значит, ей нужно всё из него.
#if !defined(__need_FILE) && !defined(_STDIO_INCL)
#define _STDIO_INCL 1
#define __need_FILE
#include <etc/decl.h>


#if defined(__need_FILE)
#include <etc/FILE.h>
#undef __need_FILE
#endif

// Из stddef.h нам потребуется size_t.
#define __need_size_t
#include <stddef.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

BEGIN_DECLS

// Резервированный поток, отвечающий за стандартный вывод ошибки.
extern FILE *stderr;

// Открывает файл по имени `filename` в режиме `mode`.
FILE* fopen(const char* filename, const char* mode);

// Закрывает файл в потоке `stream`.
int fclose(FILE* stream);

// Считывает `nmemb` объектов размером `size` Б из потока `stream` и записывает их в указатель `ptr`.
size_t fread(void *ptr, size_t size, size_t nmemb, FILE* stream);

// Записывает `nmemb` объектов размером `size` Б из буфера `ptr` в поток `stream`.
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

// Изменяет положение указателя в файле.
int fseek(FILE* stream, long offset, int whence);

// Получает положение указателя на текущий символ в файле.
long ftell(FILE *stream);

// Устанавливает буфер `buf` для файла `stream`.
void setbuf(FILE *stream, char* buf);

// Форматирует строку `format` по списку аргументов `arg` и выводит её в поток `stream`.
int vfprintf(FILE* stream, const char* format, va_list arg);

// Записывает все данные из потока вывода на устройство.
int fflush(FILE *stream);

// Производит форматированный вывод в поток `stream`.
int fprintf(FILE *stream, const char *format, ...);

// Форматирует строку `format` и копирует её в строку `s`.
int sprintf(char *s, const char *format, ...);

END_DECLS

#endif