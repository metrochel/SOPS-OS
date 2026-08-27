/*
 * graphics_macros.hpp - Макросы для объявления функций
 *
 */

#ifndef _GRAPHICS_MACROS_INCL
#define _GRAPHICS_MACROS_INCL

#include "../glyphs/glyphs.hpp"
#include "libk/objs.hpp"

/// Этот макрос нужен, чтобы значительно проще объявлять функции для адаптеров.
#define decl_graphics_funcs(mod, prefix, suffix) \
    mod dword prefix##_compute_pixoff_##suffix (dword x, dword y);                                      \
    mod dword prefix##_encode_col_##suffix (byte r, byte g, byte b);                                    \
    mod void prefix##_putpixel_##suffix (dword x, dword y, dword col);                                  \
    mod void prefix##_fill_##suffix (dword x, dword y, dword width, dword height, dword col);           \
    mod void prefix##_blit_##suffix (dword x, dword y, dword width, dword height, dword *cols);         \
    mod void prefix##_putglyph_##suffix (const graphics::glyph &g, dword x, dword y, dword fg_col, dword bg_col);

#endif //_GRAPHICS_MACROS_INCL
