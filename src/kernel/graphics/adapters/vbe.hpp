/*
 * vbe.hpp - Функции адаптера через линейный буфер кадров VBE
 *
 * Реализует функции адаптера для поддержки ОС.
 */

#ifndef _VBE_INCL
#define _VBE_INCL

#include "../../libk/nums.hpp"
#include "../helpers/macros.hpp"
#include "../helpers/typedefs.hpp"

#define decl_funcs_for_bpp(bpp)                                                 \
    extern force_inline void inline_putpixel_##bpp(dword offset, dword col);    \
    decl_graphics_funcs(, vbe, bpp)

namespace graphics::inline vbe {
    /// @c vbe_mode_info - это структура, содержащая данные о текущем режиме VBE.
    /// Извлекается из загрузчика.
    struct vbe_mode_info {
        // Основной блок данных

        word  attributes;               /// Флаги режима
        byte  window_a;                 /// (не используется)
        byte  window_b;                 /// (не используется)
        word  granularity;              /// (не используется)
        word  window_size;              /// (не используется)
        word  segment_a;                /// (не используется)
        word  segment_b;                /// (не используется)
        dword bank_switch_ptr;          /// (не используется)
        word  pitch;                    /// Количество байтов на строку
        word  width;                    /// Ширина экрана в пикселах
        word  height;                   /// Высота экрана в пикселах
        byte  wchar;                    /// (не используется)
        byte  ychar;                    /// (не используется)
        byte  planes;                   /// (не используется)
        byte  bpp;                      /// Количество бит на пиксел
        byte  num_banks;                /// (не используется)
        byte  memory_model;             /// (не используется)
        byte  bank_size;                /// (не используется)
        byte  image_pages;              /// (не используется)
        byte  reserved_0;               /// (резервировано)

        // Блок данных режима прямого цвета

        byte  red_mask_size;            /// Размер маски в коде пиксела, отвечающей за красный канал
        byte  red_pos;                  /// Сдвиг маски в коде пиксела, отвечающей за красный канал
        byte  green_mask_size;          /// Размер маски в коде пиксела, отвечающей за зелёный канал
        byte  green_pos;                /// Сдвиг маски в коде пиксела, отвечающей за зелёный канал
        byte  blue_mask_size;           /// Размер маски в коде пиксела, отвечающей за синий канал
        byte  blue_pos;                 /// Сдвиг маски в коде пиксела, отвечающей за синий канал
        byte  reserved_mask;            /// Размер маски в коде пиксела, биты которой резервированы
        byte  reserved_pos;             /// Сдвиг маски в коде пиксела, биты которой резервированы
        byte  direct_color_attr;        /// Свойства режима прямого цвета

        // Блок данных о буфере кадра

        dword frame_buffer;             /// Буфер кадра (куда пикселы класть)
        dword off_screen_mem_offset;    /// Сдвиг в буфере кадров, память, начиная с которого, на экран не выводится
        dword off_screen_mem_size;      /// Размер памяти в буфере кадров, которая не выводится на экран
        byte  reserved_1[206];          /// (резервировано)
    } __attribute__ ((packed));

    extern const vbe_mode_info *vbe_info;

    extern byte bpp;

    decl_funcs_for_bpp(8)
    decl_funcs_for_bpp(16)
    decl_funcs_for_bpp(24)
    decl_funcs_for_bpp(32)
    decl_funcs_for_bpp(general)

    extern const adapter_funcs vbe_adapter_funcs_8;
    extern const adapter_funcs vbe_adapter_funcs_16;
    extern const adapter_funcs vbe_adapter_funcs_24;
    extern const adapter_funcs vbe_adapter_funcs_32;
    extern const adapter_funcs vbe_adapter_funcs_general;

    void init_vbe(vbe_mode_info *info_ptr);

    const adapter_funcs& get_adapter_funcs();
}

#endif //_VBE_INCL
