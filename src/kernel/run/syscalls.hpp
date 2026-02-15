#ifndef _SYSCALLS_INCL
#define _SYSCALLS_INCL

//
//  Библиотека управления системными вызовами
//
//  - Распределяет системные вызовы и обрабатывает некоторые из них.
//
#include "../util/nums.hpp"

#define RUNTIME_ERROR_INVALID_ACCESS        0xFFFFFFFE
#define RUNTIME_ERROR_FILE_OPEN_FAILURE     0xFFFFFFFD
#define RUNTIME_ERROR_FILE_READ_FAILURE     0xFFFFFFFC
#define RUNTIME_ERROR_FILE_WRITE_FAILURE    0xFFFFFFFB
#define RUNTIME_ERROR_FILE_MAX_REACHED      0xFFFFFFFA
#define RUNTIME_ERROR_ALLOC_FAILURE         0xFFFFFFF9

#define SYSCALL_GRAPHICS        0x1000
#define SYSCALL_HID_INPUT       0x2000
#define SYSCALL_FILES           0x3000
#define SYSCALL_EXEC_CTRL       0x4000

// ### Syscall
// Центральная часть системы. Отвечает за системные вызовы, доступные программе.
// Программа осуществляет системный вызов путём команды `INT 0xC0` и помещает в 
// регистр EAX номер системного вызова, соответствующий этому перечню. Так программа
// может как-либо взаимодействовать с системой.
enum Syscall {
    // Выход из программы
    Exit = 0,

    /* Графика */

    /// @brief Размещение символа на экране
    /// @param ESI Символ
    /// @param EDI Абсцисса символа (текстовые координаты)
    /// @param ECX Ордината символа (текстовые координаты)
    /// @param EDX Цвет символа
    /// @param EBX Цвет заднего фона
    PutCharacter = SYSCALL_GRAPHICS | 0,
    /// @brief Очистка экрана
    ClearScreen = SYSCALL_GRAPHICS | 1,
    /// @brief Вывод строки на экран
    /// @param ESI Адрес строки
    Print = SYSCALL_GRAPHICS | 2,
    /// @brief Позиционный вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    PositionedPrint = SYSCALL_GRAPHICS | 3,
    /// @brief Цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Цвет текста
    /// @param ECX Цвет заднего фона
    ColouredPrint = SYSCALL_GRAPHICS | 4,
    /// @brief Позиционный цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    /// @param EDX Цвет текста
    /// @param EBX Цвет заднего фона
    PositionedColouredPrint = SYSCALL_GRAPHICS | 5,
    /// @brief Установка границ экрана
    /// @param ESI Абсцисса левого верхнего угла (текстовые координаты)
    /// @param EDI Ордината левого верхнего угла (текстовые координаты)
    /// @param ECX Абсцисса правого нижнего угла (текстовые координаты)
    /// @param EDX Ордината правого нижнего угла (текстовые координаты)
    /// @note Эта функция влияет только на вызовы `Print` и `ColouredPrint`.
    /// @note Вывод вне границ экрана при помощи `PositionedPrint` или `PutCharacter` по-прежнему допускается,
    /// @note но для первого автоматический перенос сломается. Лучше использовать `PutCharacter`.
    SetScreenBounds = SYSCALL_GRAPHICS | 6,
    /// @brief Получение размеров экрана
    /// @returns - Нижнее слово - ширина экрана в символах
    /// @returns - Верхнее слово - высота экрана в символах
    GetScreenBounds = SYSCALL_GRAPHICS | 7,
    /// @brief Включение курсора
    EnableCursor = SYSCALL_GRAPHICS | 8,
    /// @brief Выключение курсора
    DisableCursor = SYSCALL_GRAPHICS | 9,

    // Ввод с клавиатуры

    /// @brief Чтение одной клавиши
    /// @return - Верхний байт - статус нажатия (клавиши-модификаторы: Ctrl, Alt и т.д.)
    /// @return - Нижний байт - код нажатой клавиши
    ReadKey = SYSCALL_HID_INPUT | 0,
    /// @brief Чтение строки с клавиатуры
    /// @param ESI Адрес строки-буфера для данных
    ReadStr = SYSCALL_HID_INPUT | 1,

    // Работа с файлами

    /// @brief Открытие файла
    /// @param ESI Указатель на строку-путь к файлу
    /// @param EDI Режим открытия файла
    /// @return Дескриптор открытого файла
    OpenFile = SYSCALL_FILES | 0,
    /// @brief Закрытие файла
    /// @param ESI Дескриптор закрываемого файла
    CloseFile = SYSCALL_FILES | 1,
    /// @brief Чтение куска файла в память
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер читаемого блока, Б
    /// @param ECX Начало считываемого блока
    /// @param EDX Дескриптор файла
    /// @return Размер считанного блока, Б
    Read = SYSCALL_FILES | 2,
    /// @brief Чтение одного символа из файла в память
    /// @param ESI Сдвиг символа по файлу, Б
    /// @param EDI Дескриптор файла
    /// @return Считанный символ или -1 в случае провала
    ReadChar = SYSCALL_FILES | 3,
    /// @brief Запись данных в файл
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер записываемого блока, Б
    /// @param ECX Начало записываемого блока
    /// @param EDX Дескриптор файла
    /// @return Размер записанного блока, Б
    Write = SYSCALL_FILES | 4,
    /// @brief Запись одного символа в файл
    /// @param ESI Записываемый символ
    /// @param EDI Сдвиг записываемого символа
    /// @param ECX Дескриптор файла
    WriteChar = SYSCALL_FILES | 5,
    /// @brief Получение размера файла
    /// @param ESI Дескриптор файла
    /// @return Размер файла, Б
    GetFileSize = SYSCALL_FILES | 6,
    /// @brief Выделение числа под временный файл
    /// @return Число от 0x00001 до 0xFFFFF или -1 в случае провала
    /// @note Для формата временного файла см. src/libc/stdio.h.
    AllocateTmpFile = SYSCALL_FILES | 7,
    /// @brief Удаление файла
    /// @param ESI Название удаляемого файла
    /// @return 0 в случае успеха
    RemoveFile = SYSCALL_FILES | 8,
    /// @brief Перемещение файла
    /// @param ESI Название старого файла
    /// @param EDI Название нового файла
    /// @return 0 в случае успеха
    MoveFile = SYSCALL_FILES | 9,

    // Управление ресурсами и исполнением

    /// @brief Выделение памяти для процесса
    /// @param ESI Размер нужного блока, Б
    /// @return Указатель на блок
    Malloc = SYSCALL_EXEC_CTRL | 0,
    /// @brief Освобождение памяти, используемой процессом
    /// @param ESI Адрес освобождаемого блока
    Free = SYSCALL_EXEC_CTRL | 1,
    /// @brief Вызов команды в обработчике
    /// @param ESI Указатель на строку-команду
    /// @return Код выхода команды
    System = SYSCALL_EXEC_CTRL | 2,
    /// @brief Получение переменной окружения
    /// @param ESI Название переменной
    /// @return Значение переменной
    GetEnvVar = SYSCALL_EXEC_CTRL | 3,
    /// @brief Установка значения переменной окружения
    /// @param ESI Название переменной
    /// @param EDI Значение переменной (строка)
    /// @return 0 в случае успеха
    SetEnvVar = SYSCALL_EXEC_CTRL | 4,
    /// @brief Время в формате UNIX Timestamp
    /// @return Время в формате UNIX Timestamp
    GetUnixTime = SYSCALL_EXEC_CTRL | 5,
    /// @brief Время с точностью до наносекунды (от 0 до 1 с)
    /// @return Время с точностью до наносекунды (от 0 до 1 с)
    GetNanosecTime = SYSCALL_EXEC_CTRL | 6,
    /// @brief Время исполнения процесса (в тактах процессора)
    /// @return Время исполнения процесса (в тактах процессора)
    GetProcTime = SYSCALL_EXEC_CTRL | 7
};

/// @brief Обрабатывает системный вызов.
/// @param call Номер системного вызова
/// @param pid PID процесса-владельца
/// @param arg1 1-й аргумент вызова
/// @param arg2 2-й аргумент вызова
/// @param arg3 3-й аргумент вызова
/// @param arg4 4-й аргумент вызова
/// @param arg5 5-й аргумент вызова
/// @return Возвращаемое значение вызова (если такое имеется)
dword processSyscall(Syscall call, word pid, dword arg1, dword arg2, dword arg3, dword arg4, dword arg5);

#endif