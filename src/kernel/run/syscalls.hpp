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

// ### Syscall
// Центральная часть системы. Отвечает за системные вызовы, доступные программе.
// Программа осуществляет системный вызов путём команды `INT 0xC0` и помещает в 
// регистр EAX номер системного вызова, соответствующий этому перечню. Так программа
// может как-либо взаимодействовать с системой.
enum Syscall {
    // Выход из программы
    Exit = 0,

    // Графика

    /// @brief Размещение символа на экране
    /// @param ESI Символ
    /// @param EDI Абсцисса символа (текстовые координаты)
    /// @param ECX Ордината символа (текстовые координаты)
    /// @param EDX Цвет символа
    /// @param EBX Цвет заднего фона
    PutCharacter = 0x100,
    /// @brief Очистка экрана
    ClearScreen = 0x101,
    /// @brief Вывод строки на экран
    /// @param ESI Адрес строки
    Print = 0x102,
    /// @brief Позиционный вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    PositionedPrint = 0x103,
    /// @brief Цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Цвет текста
    /// @param ECX Цвет заднего фона
    ColouredPrint = 0x104,
    /// @brief Позиционный цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    /// @param EDX Цвет текста
    /// @param EBX Цвет заднего фона
    PositionedColouredPrint = 0x105,
    /// @brief Установка границ экрана
    /// @param ESI Абсцисса левого верхнего угла (текстовые координаты)
    /// @param EDI Ордината левого верхнего угла (текстовые координаты)
    /// @param ECX Абсцисса правого нижнего угла (текстовые координаты)
    /// @param EDX Ордината правого нижнего угла (текстовые координаты)
    /// @note Эта функция влияет только на вызовы `Print` и `ColouredPrint`. 
    /// @note Вывод вне границ экрана при помощи `PositionedPrint` или `PutCharacter` по-прежнему допускается,
    /// @note но для первого автоматический перенос сломается. Лучше использовать `PutCharacter`.
    SetScreenBounds = 0x106,
    /// @brief Получение размеров экрана
    /// @returns - Нижнее слово - ширина экрана в символах
    /// @returns - Верхнее слово - высота экрана в символах
    GetScreenBounds = 0x107,
    /// @brief Включение курсора
    EnableCursor = 0x108,
    /// @brief Выключение курсора
    DisableCursor = 0x109,

    // Ввод с клавиатуры

    /// @brief Чтение одной клавиши
    /// @return - Верхний байт - статус нажатия (клавиши-модификаторы: Ctrl, Alt и т.д.)
    /// @return - Нижний байт - код нажатой клавиши
    ReadKey = 0x200,
    /// @brief Чтение строки с клавиатуры
    /// @param ESI Адрес строки-буфера для данных
    ReadStr = 0x201,

    // Работа с файлами

    /// @brief Открытие файла
    /// @param ESI Указатель на строку-путь к файлу
    /// @param EDI Режим открытия файла
    /// @return Указатель на структуру `FileHandle` - обработчик файла
    OpenFile = 0x300,
    /// @brief Закрытие файла
    /// @param ESI Указатель на структуру `FileHandle`
    CloseFile = 0x301, 
    /// @brief Чтение куска файла в память
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер читаемого блока
    /// @param ECX Указатель на `FileHandle`
    /// @return 0, если чтение удалось
    Read = 0x302,
    /// @brief Чтение всего файла в память
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Указатель на `FileHandle`
    /// @return 0, если чтение удалось
    ReadFull = 0x303,
    /// @brief Запись в файл
    /// @param ESI Указатель на буфер для данных
    /// @param EDI Размер записываемого блока
    /// @param ECX Указатель на `FileHandle`
    /// @return 0, если запись удалась
    Write = 0x304,
    /// @brief Получение размера файла
    /// @param ESI Указатель на `FileHandle`
    /// @return Размер файла, Б
    GetFileSize = 0x305,

    // Управление ресурсами и исполнением

    /// @brief Выделение памяти для процесса
    /// @param ESI Размер нужного блока, Б
    /// @return Указатель на блок
    Malloc = 0x400,
    /// @brief Освобождение памяти, используемой процессом
    /// @param ESI Адрес освобождаемого блока
    Free = 0x401,
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