#include "graphics.hpp"
#include "glyphs/glyphs.hpp"
#include "adapters/vbe.hpp"
#include "../memmgr/memmgr.hpp"
#include "../kernel.hpp"
#include "../libk/util.hpp"

/// Перевод текстовых координат в графические
#define ttg_x(x) ((x) * 16)
/// Перевод текстовых координат в графические
#define ttg_y(y) ((y) * 24)

#define is_newline(symb) ((symb) == 0x0A)

using namespace graphics;

graph_ostream cout(default_fg_col, default_bg_col);
graph_ostream cwrn(warn_fg_col, warn_bg_col);
graph_ostream cerr(error_fg_col, error_bg_col);

NAMESPACE_BEGIN(graphics)

dword text_cur_x = 1;
dword text_cur_y = 1;

dword screen_width, screen_height;
dword text_screen_width, text_screen_height;

dword default_fg_col, default_bg_col;
dword warn_fg_col, warn_bg_col;
dword error_fg_col, error_bg_col;

#define usable_text_width (graphics::text_screen_width - 2 * text_border)
#define usable_text_height (graphics::text_screen_height - 2 * text_border)
#define usable_text_surface (usable_text_width * usable_text_height)

const adapter_funcs* cur_adapter_funcs = nullptr;
adapter cur_adapter = none;

character *screen_chars, *viewport;

dword compute_pixoff(dword x, dword y) {
    return cur_adapter_funcs->compute_pixoff(x, y);
}

dword encode_col(byte r, byte g, byte b) {
    return cur_adapter_funcs->encode_col(r, g, b);
}

void putpixel(dword x, dword y, dword col) {
    cur_adapter_funcs->putpixel(x, y, col);
}

void fill(dword x, dword y, dword width, dword height, dword col) {
    cur_adapter_funcs->fill(x, y, width, height, col);
}

void blit(dword x, dword y, dword width, dword height, dword *cols) {
    cur_adapter_funcs->blit(x, y, width, height, cols);
}

void putglyph(const glyph &g, dword x, dword y, dword fg_col, dword bg_col) {
    cur_adapter_funcs->putglyph(g, x, y, fg_col, bg_col);
}

void reg_char(dword ch, dword fg_col, dword bg_col, dword x, dword y) {
    viewport[y * usable_text_width + x] = { ch, fg_col, bg_col };
}

void scroll() {
    character *vp_pre_update = viewport;
    viewport += usable_text_width;
    if (viewport - screen_chars >= text_screen_width * text_screen_height) {
        memcpy(viewport, screen_chars, usable_text_surface * sizeof((character){}));
        viewport = screen_chars;
    }
    refresh_text(vp_pre_update);
}

void refresh_text(character *vp_pre_update) {
    for (dword i = 0; i <= usable_text_height; i++) {
        for (dword j = 0; j <= usable_text_width; j++) {
            dword idx = i * usable_text_width + j;
            if (viewport[idx].code != vp_pre_update[idx].code
            ||  viewport[idx].fg_col != vp_pre_update[idx].fg_col
            ||  viewport[idx].bg_col != vp_pre_update[idx].bg_col)
            {
                character &ch = viewport[idx];
                const glyph &g = glyph_from_symbol(ch.code);
                putglyph(g, ttg_x(j), ttg_y(i), ch.fg_col, ch.bg_col);
            }
        }
    }
}

void init() {
    vbe_mode_info *vbe = &(bld->VBEInfo);
    init_vbe(vbe);

    cur_adapter = vbe_linear_framebuf;
    cur_adapter_funcs = &vbe::get_adapter_funcs();

    text_screen_width = screen_width / glyph_width;
    text_screen_height = screen_height / glyph_height;

    default_fg_col = encode_col(255, 255, 255);
    default_bg_col = encode_col(0, 0, 0);

    warn_fg_col = encode_col(255, 255, 0);
    warn_bg_col = encode_col(128, 128, 0);

    error_fg_col = encode_col(255, 0, 0);
    error_bg_col = encode_col(128, 0, 0);

    screen_chars = (character*)kmalloc(sizeof ((character){}) * 2 * usable_text_surface);

    if (!screen_chars) {
        // TODO: бросать исключение
        return;
    }

    viewport = screen_chars;
}

NAMESPACE_END(graphics)

void graph_ostream::put(dword symb) {
    if (is_newline(symb)) {
        text_cur_x = text_border;
        text_cur_y ++;

        if (text_cur_y >= text_screen_height - text_border) {
            scroll();
            text_cur_y--;
        }

        return;
    }

    const glyph &g = glyph_from_symbol(symb);

    dword x = ttg_x(text_cur_x);
    dword y = ttg_y(text_cur_y);

    reg_char(symb, fg_col, bg_col, text_cur_x, text_cur_y);
    putglyph(g, x, y, fg_col, bg_col);

    text_cur_x++;

    if (text_cur_x >= text_screen_width - text_border) {
        text_cur_x = text_border;
        text_cur_y++;
    }

    if (text_cur_y >= text_screen_height - text_border) {
        scroll();
        text_cur_y--;
    }
}

graph_ostream::graph_ostream(dword fg, dword bg) : fg_col(fg), bg_col(bg) {
    set_send_utf8(true);
}