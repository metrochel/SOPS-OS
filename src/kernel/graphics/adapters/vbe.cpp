/*
 * vbe.cpp - Функции адаптера через линейный буфер кадров VBE
 *
 */

#include "vbe.hpp"
#include "../graphics.hpp"

using namespace graphics::vbe;

/* === Глобальные переменные === */

byte *vbe_framebuf_ptr = nullptr;
byte graphics::vbe::bpp = 0;
const vbe_mode_info *graphics::vbe::vbe_info;

#define decl_adapter_funcs_for_bpp(bpp) \
    const graphics::adapter_funcs graphics::vbe::vbe_adapter_funcs_##bpp = { \
        &vbe_compute_pixoff_##bpp,      \
        &vbe_putpixel_##bpp,            \
        &vbe_fill_##bpp,                \
        &vbe_blit_##bpp,                \
        &vbe_putglyph_##bpp             \
    };

decl_adapter_funcs_for_bpp(8)
decl_adapter_funcs_for_bpp(16)
decl_adapter_funcs_for_bpp(24)
decl_adapter_funcs_for_bpp(32)
decl_adapter_funcs_for_bpp(general)

/* === inline-функции putpixel === */

template<typename T, size_t mask, byte bpp>
force_inline void template_vbe_inline_putpixel(dword offset, T col) {
    *(T*)(vbe_framebuf_ptr + offset) = col & mask;
}

#define decl_inline_func(bpp, type, mask) \
    force_inline void graphics::vbe::inline_putpixel_##bpp (dword offset, dword col) {          \
        template_vbe_inline_putpixel<type, mask, bpp / 8>(offset, col);                         \
    }

decl_inline_func(8, byte, maxbyte)
decl_inline_func(16, word, maxword)
decl_inline_func(24, dword, 0xFFFFFF)
decl_inline_func(32, dword, maxdword)

/* === Шаблоны графических функций === */

template<byte bpp>
inline dword template_vbe_compute_pixoff(dword x, dword y) {
    return y * vbe_info->pitch + x * bpp;
}

template<typename T, size_t mask, byte bpp>
inline void template_vbe_putpixel(dword x, dword y, dword col) {
    dword pixoff = template_vbe_compute_pixoff<bpp>(x, y);
    template_vbe_inline_putpixel<T,mask,bpp>(pixoff, col);
}

template<typename T, size_t mask, byte bpp>
inline void template_vbe_fill(dword x, dword y, dword width, dword height, dword col) {
    dword offset = template_vbe_compute_pixoff<bpp>(x, y);
    dword byte_width = width * bpp;
    for (dword i = 0; i < height; i++) {
        for (dword j = 0; j < width; j++) {
            template_vbe_inline_putpixel<T, mask, bpp>(offset, col);
            offset += bpp;
        }
        offset -= byte_width;
        offset += vbe_info->pitch;
    }
}

template<typename T, size_t mask, byte bpp>
inline void template_vbe_blit(dword x, dword y, dword width, dword height, dword *cols) {
    dword pixoff = template_vbe_compute_pixoff<bpp>(x, y);
    dword offset = 0;
    dword byte_width = width * bpp;
    for (dword i = 0; i < height; i++) {
        for (dword j = 0; j < width; j++) {
            template_vbe_inline_putpixel<T, mask, bpp>(pixoff, cols[offset]);
            pixoff += bpp;
            offset++;
        }
        pixoff -= byte_width;
        pixoff += vbe_info->pitch;
    }
}

template<typename T, size_t mask, byte bpp>
inline void template_vbe_putglyph(const graphics::glyph &g, dword x, dword y, dword fg_col, dword bg_col) {
    dword pixoff = template_vbe_compute_pixoff<bpp>(x, y);
    dword byte_width = graphics::glyph_width * bpp;
    for (dword i = 0; i < graphics::glyph_height; i++) {
        dword bitmask = 1 << (graphics::glyph_width - 1);
        for (int j = graphics::glyph_width - 1; j >= 0; j--) {
            if (g.lines[i] & bitmask)
                template_vbe_inline_putpixel<T, mask, bpp>(pixoff, fg_col);
            else
                template_vbe_inline_putpixel<T, mask, bpp>(pixoff, bg_col);
            pixoff += bpp;
            bitmask >>= 1;
        }
        pixoff -= byte_width;
        pixoff += vbe_info->pitch;
    }
}

/* === Основные функции === */

#define declare_funcs_for_bpp(bpp, type, mask)                                                                      \
    dword graphics::vbe::vbe_compute_pixoff_##bpp(dword x, dword y) {                                               \
        return template_vbe_compute_pixoff<bpp / 8>(x, y);                                                          \
    }                                                                                                               \
    void graphics::vbe::vbe_putpixel_##bpp (dword x, dword y, dword col) {                                          \
        template_vbe_putpixel<type, mask, bpp / 8>(x, y, col);                                                      \
    }                                                                                                               \
    void graphics::vbe::vbe_fill_##bpp(dword x, dword y, dword width, dword height, dword col) {                    \
        template_vbe_fill<type, mask, bpp / 8>(x, y, width, height, col);                                           \
    }                                                                                                               \
    void graphics::vbe::vbe_blit_##bpp(dword x, dword y, dword width, dword height, dword *cols) {                  \
        template_vbe_blit<type, mask, bpp / 8>(x, y, width, height, cols);                                          \
    }                                                                                                               \
    void graphics::vbe::vbe_putglyph_##bpp(const graphics::glyph &g, dword x, dword y, dword fg_col, dword bg_col) {\
        template_vbe_putglyph<type, mask, bpp / 8>(g, x, y, fg_col, bg_col);                                        \
    }

declare_funcs_for_bpp(8, byte, maxbyte)
declare_funcs_for_bpp(16, word, maxword)
declare_funcs_for_bpp(24, dword, 0xFFFFFF)
declare_funcs_for_bpp(32, dword, maxdword)

/* === Общие функции === */

dword graphics::vbe::vbe_compute_pixoff_general(dword x, dword y) {
    return y * vbe_info->pitch + x * (bpp / 8);
}

force_inline void graphics::vbe::inline_putpixel_general(dword offset, dword col) {
    byte *write_ptr = vbe_framebuf_ptr + offset;
    byte bytes = bpp / 8;
    for (byte i = 0; i < bytes; i++) {
        write_ptr[i] = col & maxbyte;
        col >>= 8;
    }
}

void graphics::vbe::vbe_putpixel_general(dword x, dword y, dword col) {
    dword offset = vbe_compute_pixoff_general(x, y);
    inline_putpixel_general(offset, col);
}

void graphics::vbe::vbe_fill_general(dword x, dword y, dword width, dword height, dword col) {
    dword offset = vbe_compute_pixoff_general(x, y);
    byte bytes_per_pix = bpp / 8;
    dword byte_width = width * bytes_per_pix;
    for (dword i = 0; i < height; i++) {
        for (dword j = 0; j < width; j++) {
            inline_putpixel_general(offset, col);
            offset += bytes_per_pix;
        }
        offset -= byte_width;
        offset += vbe_info->pitch;
    }
}

void graphics::vbe::vbe_blit_general(dword x, dword y, dword width, dword height, dword *cols) {
    dword pixoff = vbe_compute_pixoff_general(x, y);
    dword offset = 0;
    byte bytes_per_pix = bpp / 8;
    dword byte_width = width * bytes_per_pix;
    for (dword i = 0; i < height; i++) {
        for (dword j = 0; j < width; j++) {
            inline_putpixel_general(pixoff + offset, cols[offset]);
            pixoff += bytes_per_pix;
            offset++;
        }
        pixoff -= byte_width;
        pixoff += vbe_info->pitch;
    }
}

void graphics::vbe::vbe_putglyph_general(const graphics::glyph &g, dword x, dword y, dword fg_col, dword bg_col) {
    dword pixoff = vbe_compute_pixoff_general(x, y);
    byte bytes_per_pix = bpp / 8;
    dword byte_width = graphics::glyph_width * bytes_per_pix;
    for (dword i = 0; i < graphics::glyph_height; i++) {
        dword bitmask = 1;
        for (dword j = 0; j < graphics::glyph_width; j++) {
            if (g.lines[i] & bitmask)
                inline_putpixel_general(pixoff, fg_col);
            else
                inline_putpixel_general(pixoff, bg_col);
            pixoff += bytes_per_pix;
            bitmask <<= 1;
        }
        pixoff -= byte_width;
        pixoff += vbe_info->pitch;
    }
}

/* === Функции инициализации === */

const graphics::adapter_funcs& graphics::vbe::get_adapter_funcs() {
    switch (bpp) {
        case 8: return vbe_adapter_funcs_8;
        case 16: return vbe_adapter_funcs_16;
        case 24: return vbe_adapter_funcs_24;
        case 32: return vbe_adapter_funcs_32;
        default: return vbe_adapter_funcs_general;
    }
}

void graphics::vbe::init_vbe(vbe_mode_info *info_ptr) {
    vbe_info = info_ptr;

    vbe_framebuf_ptr = (byte*)(0xFC000000 + (info_ptr->frame_buffer & 0xFFF));
    bpp = info_ptr->bpp;

    screen_width = info_ptr->width;
    screen_height = info_ptr->height;
}