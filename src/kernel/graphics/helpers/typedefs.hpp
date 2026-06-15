/*
 * graphics_typedefs.hpp - Определения типов для графической подсистемы
 *
 * Определяет необходимые типы, используемые графическим драйвером.
 */

#ifndef _GRAPHICS_TYPEDEFS_INCL
#define _GRAPHICS_TYPEDEFS_INCL

#include "libk/nums.hpp"
#include "../glyphs/glyphs.hpp"

namespace graphics {
    /// @c adapters - это перечень всех адаптеров, которые могут использоваться графической системой.
    enum adapter {
        none = 0,                   /// Нет графического адаптера
        vbe_linear_framebuf = 1     /// Графический адаптер через линейный буфер кадров VBE
    };

    typedef dword (*compute_pixoff_t)(dword, dword);
    typedef dword (*encode_col_t)(byte, byte, byte);
    typedef void (*putpixel_t)(dword, dword, dword);
    typedef void (*fill_t)(dword, dword, dword, dword, dword);
    typedef void (*blit_t)(dword, dword, dword, dword, dword*);
    typedef void (*putglyph_t)(const glyph&, dword, dword, dword, dword);

    /// @c adapter_funcs - это набор функций, достаточных для работы графического адаптера с системой.
    struct adapter_funcs {
        compute_pixoff_t compute_pixoff;    /// Функция @c compute_pixoff
        encode_col_t encode_col;            /// Функция @c encode_col
        putpixel_t putpixel;                /// Функция @c putpixel
        fill_t fill;                        /// Функция @c fill
        blit_t blit;                        /// Функция @c blit
        putglyph_t putglyph;                /// Функция @c putglyph
    };
}

#endif //_GRAPHICS_TYPEDEFS_INCL
