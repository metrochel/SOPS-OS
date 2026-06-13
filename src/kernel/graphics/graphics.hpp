/*
 * graphics.hpp - Графика
 *
 * Предоставляет общие графические функции.
 */

#ifndef _GRAPHICS_INCL
#define _GRAPHICS_INCL

#include "../libk/objs.hpp"
#include "glyphs/glyphs.hpp"
#include "helpers/typedefs.hpp"

/// @c graphics - это пространство имён, предоставляющее графические функции.
namespace graphics {
    /// @c text_cur_x и @c text_cur_y описывают текущее положение курсора.
    extern dword text_cur_x, text_cur_y;

    /// @c screen_width и @c screen_height описывают размеры экрана в пикселах.
    extern dword screen_width, screen_height;

    /// @c screen_width и @c screen_height описывают размеры экрана в символах.
    extern dword text_screen_width, text_screen_height;

    /// @c cur_adapter указывает на функции, используемые текущим графическим адаптером.
    extern const adapter_funcs *cur_adapter_funcs;

    extern adapter cur_adapter;

    inline const dword text_border = 1;

    /// Цвет для обычного текста. Используется при выводе через @c cout.
    inline const dword default_fg_col = 0xFFFFFF;
    /// Цвет для заднего фона обычного текста. Используется при выводе через @c cout.
    inline const dword default_bg_col = 0x000000;

    /// Цвет для обычного текста. Используется при выводе через @c cwrn.
    inline const dword warn_fg_col = 0xFFFF00;
    /// Цвет для заднего фона обычного текста. Используется при выводе через @c cwrn.
    inline const dword warn_bg_col = 0x787800;

    /// Цвет для обычного текста. Используется при выводе через @c cerr.
    inline const dword error_fg_col = 0xFF0000;
    /// Цвет для заднего фона обычного текста. Используется при выводе через @c cerr.
    inline const dword error_bg_col = 0x780000;

    /// @c character - это структура, описывающая некоторый символ на экране.
    struct character {
        dword code;
        dword fg_col;
        dword bg_col;
    };

    /// @c screen_chars содержит данные о символах на экране.
    extern character *screen_chars;
    /// @c viewport указывает на символ в @c screen_chars, находящийся в левом верхнем углу экрана.
    extern character *viewport;

    /// @brief Вычисляет аппаратно-зависимый сдвиг для пиксела.
    /// @param x Абсцисса пиксела
    /// @param y Ордината пиксела
    /// @return Аппаратно-зависимый сдвиг
    /// @note Если вычисленный сдвиг находится за пределами экрана, то вернётся @c maxdword.
    extern dword compute_pixoff(dword x, dword y);

    /// @brief Раскрашивает определённый пиксел в данный цвет.
    /// @param x Абсцисса пиксела
    /// @param y Ордината пиксела
    /// @param col Аппаратно-зависимый цвет пиксела
    extern void putpixel(dword x, dword y, dword col);

    /// @brief Заполняет прямоугольную область данным цветом.
    /// @param x Абсцисса левого верхнего угла области
    /// @param y Ордината левого верхнего угла области
    /// @param width Ширина области
    /// @param height Высота области
    /// @param col Аппаратно-зависимый цвет пиксела
    extern void fill(dword x, dword y, dword width, dword height, dword col);

    /// @brief Заполняет прямоугольную область цветами, записанными в массиве.
    /// @param x Абсцисса левого верхнего угла области
    /// @param y Ордината левого верхнего угла области
    /// @param width Ширина области
    /// @param height Высота области
    /// @param cols Массив аппаратно-зависимых цветов пикселов
    extern void blit(dword x, dword y, dword width, dword height, dword *cols);

    /// @brief Рисует символ на экране.
    /// @param g Графические данные символа
    /// @param x Абсцисса левого верхнего угла символа
    /// @param y Ордината левого верхнего угла символа
    /// @param fg_col Цвет символа
    /// @param bg_col Цвет заднего фона символа
    extern void putglyph(const glyph &g, dword x, dword y, dword fg_col, dword bg_col);

    /// @brief Производит вывод символа @c symb на экран.
    /// @param symb Выводимый символ
    /// @param fg_col Цвет символа
    /// @param bg_col Цвет заднего фона символа
    extern void graphical_put(dword symb, dword fg_col, dword bg_col);

    extern void reg_char(dword ch, dword fg_col, dword bg_col, dword x, dword y);

    extern void refresh_text(character *vp_pre_update);

    /// @brief Пролистывает экран на одну строчку вниз.
    extern void scroll();

    /// @brief Инициализирует графическую систему.
    extern void init();
}

/// @c cout - это поток стандартного вывода.
/// Запись в него будет выводить данные на экран посредством функции @c graphical_put.
extern ostream cout;

/// @c cwrn - это поток вывода предупреждений.
/// Запись в него будет выводить данные на экран посредством функции @c graphical_put.
extern ostream cwrn;

/// @c cerr - это поток вывода ошибок.
/// Запись в него будет выводить данные на экран посредством функции @c graphical_put.
extern ostream cerr;


#endif //_GRAPHICS_INCL
