/*
 * vbe.cpp - Функции адаптера через линейный буфер кадров VBE
 *
 */

#include "vbe.hpp"
#include "../graphics.hpp"

NAMESPACE_BEGIN(graphics::vbe)

/* === Глобальные переменные === */

byte *vbe_framebuf_ptr = nullptr;
byte bpp = 0;
const vbe_mode_info *vbe_info;

#define decl_adapter_funcs_for_bpp(bpp) \
    const adapter_funcs vbe_adapter_funcs_##bpp = { \
        &vbe_compute_pixoff_##bpp,      \
        &vbe_encode_col_##bpp,          \
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
always_inline void template_vbe_inline_putpixel(dword offset, T col) {
    *(T*)(vbe_framebuf_ptr + offset) = col & mask;
}

#define decl_inline_func(bpp, type, mask) \
    always_inline void inline_putpixel_##bpp (dword offset, dword col) {         \
        template_vbe_inline_putpixel<type, mask, bpp / 8>(offset, col);          \
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

inline dword template_vbe_encode_col(byte r, byte g, byte b) {
    byte conv_r = ((double)r / 256.0) * (1 << vbe_info->red_mask_size);
    byte conv_g = ((double)g / 256.0) * (1 << vbe_info->green_mask_size);
    byte conv_b = ((double)b / 256.0) * (1 << vbe_info->blue_mask_size);

    dword red_shifted = (dword)conv_r << vbe_info->red_pos;
    dword green_shifted = (dword)conv_g << vbe_info->green_pos;
    dword blue_shifted = (dword)conv_b << vbe_info->blue_pos;

    return red_shifted | green_shifted | blue_shifted;
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
inline void template_vbe_putglyph(const glyph &g, dword x, dword y, dword fg_col, dword bg_col) {
    dword pixoff = template_vbe_compute_pixoff<bpp>(x, y);
    dword byte_width = glyph_width * bpp;
    for (dword i = 0; i < glyph_height; i++) {
        dword bitmask = 1 << (glyph_width - 1);
        for (int j = glyph_width - 1; j >= 0; j--) {
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

#define declare_funcs_for_bpp(bpp, type, mask)                                                      \
    dword vbe_compute_pixoff_##bpp(dword x, dword y) {                                              \
        return template_vbe_compute_pixoff<bpp / 8>(x, y);                                          \
    }                                                                                               \
    dword vbe_encode_col_##bpp(byte r, byte g, byte b) {                                            \
        return template_vbe_encode_col(r, g, b);                                                    \
    }                                                                                               \
    void vbe_putpixel_##bpp (dword x, dword y, dword col) {                                         \
        template_vbe_putpixel<type, mask, bpp / 8>(x, y, col);                                      \
    }                                                                                               \
    void vbe_fill_##bpp(dword x, dword y, dword width, dword height, dword col) {                   \
        template_vbe_fill<type, mask, bpp / 8>(x, y, width, height, col);                           \
    }                                                                                               \
    void vbe_blit_##bpp(dword x, dword y, dword width, dword height, dword *cols) {                 \
        template_vbe_blit<type, mask, bpp / 8>(x, y, width, height, cols);                          \
    }                                                                                               \
    void vbe_putglyph_##bpp(const glyph &g, dword x, dword y, dword fg_col, dword bg_col) {         \
        template_vbe_putglyph<type, mask, bpp / 8>(g, x, y, fg_col, bg_col);                        \
    }

declare_funcs_for_bpp(8, byte, maxbyte)
declare_funcs_for_bpp(16, word, maxword)
declare_funcs_for_bpp(24, dword, 0xFFFFFF)
declare_funcs_for_bpp(32, dword, maxdword)

/* === Общие функции === */

dword vbe_compute_pixoff_general(dword x, dword y) {
    return y * vbe_info->pitch + x * (bpp / 8);
}

dword vbe_encode_col_general(byte r, byte g, byte b) {
    return template_vbe_encode_col(r, g, b);
}

always_inline void inline_putpixel_general(dword offset, dword col) {
    byte *write_ptr = vbe_framebuf_ptr + offset;
    byte bytes = bpp / 8;
    for (byte i = 0; i < bytes; i++) {
        write_ptr[i] = col & maxbyte;
        col >>= 8;
    }
}

void vbe_putpixel_general(dword x, dword y, dword col) {
    dword offset = vbe_compute_pixoff_general(x, y);
    inline_putpixel_general(offset, col);
}

void vbe_fill_general(dword x, dword y, dword width, dword height, dword col) {
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

void vbe_blit_general(dword x, dword y, dword width, dword height, dword *cols) {
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

void vbe_putglyph_general(const glyph &g, dword x, dword y, dword fg_col, dword bg_col) {
    dword pixoff = vbe_compute_pixoff_general(x, y);
    byte bytes_per_pix = bpp / 8;
    dword byte_width = glyph_width * bytes_per_pix;
    for (dword i = 0; i < glyph_height; i++) {
        dword bitmask = 1;
        for (dword j = 0; j < glyph_width; j++) {
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

const adapter_funcs& get_adapter_funcs() {
    switch (bpp) {
        case 8: return vbe_adapter_funcs_8;
        case 16: return vbe_adapter_funcs_16;
        case 24: return vbe_adapter_funcs_24;
        case 32: return vbe_adapter_funcs_32;
        default: return vbe_adapter_funcs_general;
    }
}

void init_vbe(vbe_mode_info *info_ptr) {
    vbe_info = info_ptr;

    vbe_framebuf_ptr = (byte*)(0xFC000000 + (info_ptr->frame_buffer & 0xFFF));
    bpp = info_ptr->bpp;

    screen_width = info_ptr->width;
    screen_height = info_ptr->height;
}

NAMESPACE_END(graphics::vbe)