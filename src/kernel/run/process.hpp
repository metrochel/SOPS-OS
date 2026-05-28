#ifndef _PROC_INCL
#define _PROC_INCL
//
//  Библиотека для управления процессами.
//
//  - Управляет процессами: создаёт и удаляет их, приостанавливает и возобновляет и так далее.
//
#include "../libk/nums.hpp"
#include "../file/file.hpp"
#include "stdio.hpp"

// Резервированный PID для ядра
#define PID_KERNEL          1

#define PROC_MAX_FILES      8

// ### Process
// Отвечает за одну программную единицу, то есть процесс.
// Хранит в себе все важные данные о процессе: его PID,
// количество использованной ОЗУ и так далее.
struct Process {
    word pid;                               // PID процесса
    word clearedScreen : 1;                 // Флаг; если true, то процесс очищал экран
    word changedBounds : 1;                 // Флаг; если true, то процесс менял размер экрана
    word _reserved : 14;                    // <резервировано>
    stdout_file *stdout;                    // Файл стандартного вывода
    stdin_file *stdin;                      // Файл стандартного ввода
    stderr_file *stderr;                    // Файл стандартной ошибки
    File *files[PROC_MAX_FILES];            // Открытые файлы
    Process *parent;                        // Родительский процесс
    dword usedMemory;                       // Использованная процессом память, Б
    ptrint startAddress;                    // Начальный адрес процесса
    qword start_timestamp;                  // Момент запуска процесса
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
word determine_pid(ptrint addr);

/// @brief Определяет PID процесса-владельца блока по адресу `addr` размером `size` Б.
/// @param addr Адрес блока
/// @param size Размер блока
/// @return PID владельца
/// @note Если блок частично принадлежит другому процессу, то возвращается `maxword`.
word getBlockOwnerPID(ptrint addr, ptrint size);

#endif