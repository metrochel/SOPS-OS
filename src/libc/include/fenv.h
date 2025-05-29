//
//  fenv.h - Операции над числами с плавающей точкой
//
//  Производит операции над числами с плавающей точкой. Вот же совпадение!
//

#ifndef _FENV_INCL
#define _FENV_INCL 1

#include <etc/decl.h>

BEGIN_DECLS

// `fenv_t` - это тип, определяющий текущее окружение для
// сопроцессора чисел с плавающей точкой.
typedef struct {
    unsigned short int control;
    unsigned short int _reserved1;
    unsigned short int status;
    unsigned short int _reserved2;
    unsigned short int tag;
    unsigned short int _reserved3;
    unsigned int instructionPtr;
    unsigned short int codeSelector;
    unsigned int opcode : 11;
    unsigned int _reserved4 : 5;
    unsigned int dataOffset;
    unsigned short int dataSelector;
    unsigned short int _reserved5;
} fenv_t;

// Ошибка деления на ноль
#define FE_DIVBYZERO    0b1
// Ошибка неточного результата операции
#define FE_INEXACT      0b10
// Ошибка неверного результата
#define FE_INVALID      0b100
// Ошибка переполнения
#define FE_OVERFLOW     0b1000
// Ошибка недополнения
#define FE_UNDERFLOW    0b10000

// Любая ошибка
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

// `fexcept_t` - это тип, отвечающий за показания ошибок от сопроцессора.
typedef unsigned short int fexcept_t;

// Режим округления - вниз
#define FE_DOWNWARD     0b1
// Режим округления - к ближайшему
#define FE_TONEAREST    0b10
// Режим округления - к нулю
#define FE_TOWARDZERO   0b100
// Режим округления - вверх
#define FE_UPWARD       0b1000

// Начальное окружение сопроцессора для чисел с плавающей точкой
#define FE_DFL_ENV ((const fenv_t*)0)

// Очищает флаг исключения `excepts`.
int feclearexcept(int excepts);

// Проверяет, какие исключения вызваны.
int fetestexcept(int excepts);

// Вызывает исключения `excepts`.
int feraiseexcept(int excepts);

// Записывает в `flags` исключения `excepts` в зависимости от того, возникли ли они.
int fegetexceptflag(fexcept_t *flags, int excepts);

// Отмечает исключения `excepts` из `flags` как разрешённые.
int fesetexceptflag(const fexcept_t *flags, int excepts);

// Получает текущий режим округления.
int fegetround(void);

// Изменяет режим округления на `round`.
int fesetround(int round);

// Записывает текущее состояние окружения в `envPtr`.
int fegetenv(fenv_t *envPtr);

// Записывает состояние сопроцессора в `envPtr` с очищенными флагами исключений.
int feholdexcept(fenv_t *envPtr);

// Изменяет состояние окружения сопроцессора на то, что записано в `envPtr`.
int fesetenv(const fenv_t *envPtr);

// Обновляет окружение в соответствии с `envPtr` и вызывает связанные с этим исключения.
int feupdateenv(const fenv_t *envPtr);

END_DECLS

#endif