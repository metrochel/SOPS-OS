#ifndef _PROC_INCL
#define _PROC_INCL
//
//  Библиотека для управления процессами.
//
//  - Управляет процессами: создаёт и удаляет их, приостанавливает и возобновляет и так далее.
//
#include "../util/nums.hpp"
#include "../file/file.hpp"

// Резервированный PID для ядра
#define PID_KERNEL          1

#define PROC_MAX_FILES      8

// ### Process
// Отвечает за одну программную единицу, то есть процесс.
// Хранит в себе все важные данные о процессе: его PID,
// количество использованной ОЗУ и так далее.
struct Process {
    word pid;
    word clearedScreen : 1;
    word changedBounds : 1;
    word _reserved : 14;
    FileHandle *handles[PROC_MAX_FILES];
    dword usedMemory;
    ptrint startAddress;
};

/// @brief Инициализирует библиотеку процессов для работы.
void initProcessesLib();

/// @brief Получает данные о процессе.
/// @param pid PID процесса
/// @return Данные о процессе
Process getProcessData(word pid);

/// @brief Записывает данные о процессе
/// @param pid PID процесса
/// @param data Новые данные
void setProcessData(word pid, Process data);

/// @brief Регистрирует процесс.
/// @return Данные о процессе
Process registerProcess();

/// @brief Завершает процесс и освобождает его данные.
/// @param pid PID освобождаемого процесса
void unregisterProcess(word pid);

/// @brief Определяет по данному адресу PID процесса, к которому он принадлежит.
/// @param addr Адрес внутри пространства данного процесса
/// @return PID процесса
word determinePID(ptrint addr);

/// @brief Определяет PID процесса-владельца блока по адресу `addr` размером `size` Б.
/// @param addr Адрес блока
/// @param size Размер блока
/// @return PID владельца
/// @note Если блок частично принадлежит другому процессу, то возвращается `maxword`.
word getBlockOwnerPID(ptrint addr, ptrint size);

#endif