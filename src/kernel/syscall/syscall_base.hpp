/*
 * syscall_base.hpp - Основы системных вызовов
 *
 * Предоставляет основные макросы для определения системного вызова.
 */

#ifndef _SYSCALL_BASE_INCL
#define _SYSCALL_BASE_INCL

#include "../util/nums.hpp"

// Ошибка неверного доступа
#define RUNTIME_ERROR_INVALID_ACCESS        0xFFFFFFFE
// Ошибка открытия файла
#define RUNTIME_ERROR_FILE_OPEN_FAILURE     0xFFFFFFFD
// Ошибка чтения из файла
#define RUNTIME_ERROR_FILE_READ_FAILURE     0xFFFFFFFC
// Ошибка записи в файл
#define RUNTIME_ERROR_FILE_WRITE_FAILURE    0xFFFFFFFB
// Ошибка открытия чрезмерного количества файлов
#define RUNTIME_ERROR_FILE_MAX_REACHED      0xFFFFFFFA
// Ошибка выделения памяти
#define RUNTIME_ERROR_ALLOC_FAILURE         0xFFFFFFF9

#ifdef __x86_64__
// `syscall_arg_t` - это тип аргумента системного вызова.
typedef qword syscall_arg_t;
// `syscall_ret_t` - это тип значения, возвращаемого системным вызовом.
typedef qword syscall_ret_t;
#else
// `syscall_arg_t` - это тип аргумента системного вызова.
typedef dword syscall_arg_t;
// `syscall_ret_t` - это тип значения, возвращаемого системным вызовом.
typedef dword syscall_ret_t;
#endif

#define _mb_unused  [[maybe_unused]]

// Этот макрос объявляет аргументы для обработчика системного вызова.
#define syscall_args _mb_unused word pid, _mb_unused syscall_arg_t arg1, _mb_unused syscall_arg_t arg2, _mb_unused syscall_arg_t arg3, _mb_unused syscall_arg_t arg4, _mb_unused syscall_arg_t arg5

typedef syscall_ret_t (*syscall_handle_t)(syscall_args);

/*
 * Я подсмотрел этот метод на StackOverflow. Чтобы использовать встроенный счётчик, мы объявим его
 * текущее значение как основное, а потом каждый раз вычитать его из счётчика.
 */
enum { __COUNTER_BASE__ = __COUNTER__ };
#define __counter __COUNTER__ - __COUNTER_BASE__

#endif

/*
 * Здесь используем ту же схему, что и в libc: этот заголовок будет включаться для каждой категории,
 * каждая из которых будет объявлять SYSCALL_BASE как основу для своей категории.
 */

#ifdef SYSCALL_BASE

// Этот макрос объявляет аттрибуты функций от компилятора, применимые к системным вызовам.
#define __syscall_handle__

#define __syscall_number_name(name) syscall_##name
#define __syscall_handle_name(name) handle_##name##_syscall

// Этот макрос объявляет число - номер системного вызова.
#define __syscall_num_decl(name, num, base) \
    const inline dword __syscall_number_name(name) = base | num;

// Этот макрос объявляет обработчик системного вызова в заголовке.
#define __syscall_hdr_hdl_decl(name)    \
    syscall_ret_t __syscall_handle__ __syscall_handle_name(name)(syscall_args);

#define __syscall_ext_hdl_decl(name) \
    syscall_ret_t __syscall_handle__ syscalls::__syscall_handle_name(name)(syscall_args)

// Макрос `declare_syscall` объявляет набор инструментов, который можно считать
// объявлением системного вызова с именем `name`: номер и функцию-обработчик.
#define declare_syscall(name)                               \
        __syscall_num_decl(name, (__counter), SYSCALL_BASE) \
        __syscall_hdr_hdl_decl(name)

#else
// Этот заголовок не должен включаться без объявленного SYSCALL_BASE.
#error для включения syscall_base.hpp должен быть объявлен SYSCALL_BASE
#endif