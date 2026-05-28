/*
 * syscalls_graphics.cpp - Графические системные вызовы
 */

#include "incl/syscalls_graphics.hpp"

#include "../../graphics/glyphs.hpp"
#include "../../dbg/dbg.hpp"
#include "../../memmgr/memmgr.hpp"
#include "../../run/process.hpp"
#include "../../libk/util.hpp"

#define handle(name) __syscall_ext_hdl_decl(name)

using namespace syscalls;

handle(put_character) {
    word ch = (word)arg1;
    dword x = (dword)arg2;
    dword y = (dword)arg3;
    dword letter_col = (dword)arg4;
    dword bg_col = (dword)arg5;
    setchar(x, y, ch, letter_col, bg_col);
    Glyph g = getglyph(ch);
    putglyph(g, x, y, letter_col, bg_col);
    return 0;
}

handle(clear_screen) {
    kdebug("Процесс %d очищает экран.\n", pid);
    Process p = getProcessData(pid);
    Character *buf = textBuffer;
    if (!p.clearedScreen) {
        buf = (Character*)kmalloc(textBufferBankSize * 2);
        auxTextCurX = textCurX;
        auxTextCurY = textCurY;
        auxTextBuffer = textBuffer;
        textBuffer = buf;
        magicBreakpoint();
        p.clearedScreen = 1;
    }
    memset(textBuffer, textBufferBankSize * 2, 0);
    textCurX = 1;
    textCurY = 1;
    drawLine = 0;
    refreshScreen();
    setProcessData(pid, p);
    return 0;
}

handle(print) {
    const char *text = (const char*)arg1;
    printStr(text, null, defaultTextCol, defaultBGCol, false);
    return 0;
}

handle(positioned_print) {
    const char *text = (const char*)arg1;
    word x = (word)arg2;
    word y = (word)arg3;

    word _textCurX = textCurX;
    word _textCurY = textCurY;
    textCurX = x;
    textCurY = y;
    printStr(text, null, defaultTextCol, defaultBGCol, false);
    textCurX = _textCurX;
    textCurY = _textCurY;
    return 0;
}

handle(coloured_print) {
    const char *text = (const char*)arg1;
    dword char_col = (dword)arg2;
    dword bg_col = (dword)arg3;

    printStr(text, null, char_col, bg_col, false);
    return 0;
}

handle(positioned_coloured_print) {
    const char *text = (const char*)arg1;
    word x = (word)arg2;
    word y = (word)arg3;
    dword char_col = arg4;
    dword bg_col = arg5;

    word _textCurX = textCurX;
    word _textCurY = textCurY;
    disableCursor();
    textCurX = x;
    textCurY = y;
    printStr(text, null, char_col, bg_col, false);
    textCurX = _textCurX;
    textCurY = _textCurY;
    return 0;
}

handle(set_screen_bounds) {
    dword left_bound_x = (dword)arg1;
    dword left_bound_y = (dword)arg2;
    dword right_bound_x = (dword)arg1;
    dword right_bound_y = (dword)arg1;

    Process p = getProcessData(pid);
    kdebug("Процесс %d изменяет границы экрана.\n");
    kdebug("Левый верхний угол: (%d; %d).\n", arg1, arg2);
    kdebug("Правый нижний угол: (%d; %d).\n", arg3, arg4);

    if (!bound_check(left_bound_x, left_bound_y) || !bound_check(right_bound_x, right_bound_y)) {
        kdebug("ОШИБКА: Процесс попытался установить невозможные границы\n");
        return -1;
    }

    if (right_bound_x <= left_bound_x || right_bound_y <= left_bound_y) {
        kdebug("ОШИБКА: Процесс попытался установить невозможные границы\n");
        return -1;
    }

    textLeftBoundX = left_bound_x;
    textLeftBoundY = left_bound_y;
    textRightBoundX = right_bound_x;
    textRightBoundY = right_bound_y;
    disableCursor();
    textCurX = textLeftBoundX;
    textCurY = textLeftBoundY;
    enableCursor();
    p.changedBounds = 1;
    setProcessData(pid, p);
    return 0;
}

handle(get_screen_size) {
    return ((dword)(textRightBoundY - textLeftBoundY) << 16) | (textRightBoundX - textLeftBoundX);
}

handle(disable_cursor) {
    disableCursor();
    return 0;
}

handle(enable_cursor) {
    enableCursor();
    return 0;
}