/*
 * syscalls_graphics.hpp - Графические системные вызовы
 *
 * Объявляет системные вызовы, связанные с графикой.
 */

#ifndef _SYSCALLS_GRAPHICS_INCL
#define _SYSCALLS_GRAPHICS_INCL

#define SYSCALL_BASE    0x1000
#include "../../syscall_base.hpp"

/// В пространстве имён `syscalls` объявлены все системные вызовы.
/// Это пространство имён должно использоваться в любом файле, объявляющем системные вызовы.
namespace syscalls {
    /// @brief Размещение символа на экране
    /// @param ESI Символ
    /// @param EDI Абсцисса символа (текстовые координаты)
    /// @param ECX Ордината символа (текстовые координаты)
    /// @param EDX Цвет символа
    /// @param EBX Цвет заднего фона
    declare_syscall(put_character)

    /// @brief Очистка экрана
    declare_syscall(clear_screen)

    /// @brief Вывод строки на экран
    /// @param ESI Адрес строки
    declare_syscall(print)

    /// @brief Позиционный вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    declare_syscall(positioned_print)

    /// @brief Цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Цвет текста
    /// @param ECX Цвет заднего фона
    declare_syscall(coloured_print)

    /// @brief Позиционный цветной вывод строки на экран
    /// @param ESI Адрес строки
    /// @param EDI Абсцисса строки (текстовые координаты)
    /// @param ECX Ордината строки (текстовые координаты)
    /// @param EDX Цвет текста
    /// @param EBX Цвет заднего фона
    declare_syscall(positioned_coloured_print)

    /// @brief Установка границ экрана
    /// @param ESI Абсцисса левого верхнего угла (текстовые координаты)
    /// @param EDI Ордината левого верхнего угла (текстовые координаты)
    /// @param ECX Абсцисса правого нижнего угла (текстовые координаты)
    /// @param EDX Ордината правого нижнего угла (текстовые координаты)
    /// @note Эта функция влияет только на вызовы `print` и `coloured_print`.
    /// @note Вывод вне границ экрана при помощи `positioned_print` или `put_character` по-прежнему допускается,
    /// @note но для первого автоматический перенос сломается. Лучше использовать `put_character`.
    declare_syscall(set_screen_bounds)

    /// @brief Получение размеров экрана
    /// @returns - Нижнее слово - ширина экрана в символах
    /// @returns - Верхнее слово - высота экрана в символах
    declare_syscall(get_screen_size)

    /// @brief Включение курсора
    declare_syscall(enable_cursor)

    /// @brief Выключение курсора
    declare_syscall(disable_cursor)
}

#undef SYSCALL_BASE

#endif
