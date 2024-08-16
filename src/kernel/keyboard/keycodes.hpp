#include <stdint.h>
//
//  Коды клавиш
//
//  - Здесь расписаны все коды клавиш для клавиатуры.
//

#ifndef _KCODE_INCL
#define _KCODE_INCL

#define KEYCODE_SPACEBAR 0
#define KEYCODE_A 1
#define KEYCODE_B 2
#define KEYCODE_C 3
#define KEYCODE_D 4
#define KEYCODE_E 5
#define KEYCODE_F 6
#define KEYCODE_G 7
#define KEYCODE_H 8
#define KEYCODE_I 9
#define KEYCODE_J 10
#define KEYCODE_K 11
#define KEYCODE_L 12
#define KEYCODE_M 13
#define KEYCODE_N 14
#define KEYCODE_O 15
#define KEYCODE_P 16
#define KEYCODE_Q 17
#define KEYCODE_R 18
#define KEYCODE_S 19
#define KEYCODE_T 20
#define KEYCODE_U 21
#define KEYCODE_V 22
#define KEYCODE_W 23
#define KEYCODE_X 24
#define KEYCODE_Y 25
#define KEYCODE_Z 26
#define KEYCODE_0 30
#define KEYCODE_1 31
#define KEYCODE_2 32
#define KEYCODE_3 33
#define KEYCODE_4 34
#define KEYCODE_5 35
#define KEYCODE_6 36
#define KEYCODE_7 37
#define KEYCODE_8 38
#define KEYCODE_9 39
#define KEYCODE_DOT 40
#define KEYCODE_COMMA 41
#define KEYCODE_SLASH 42
#define KEYCODE_SEMICOLON 43
#define KEYCODE_APOSTROPHE 44
#define KEYCODE_LEFT_BRACKET 45
#define KEYCODE_RIGHT_BRACKET 46
#define KEYCODE_BACKSLASH 47
#define KEYCODE_ACUTE 48
#define KEYCODE_MINUS 49
#define KEYCODE_EQUALS 50
#define KEYCODE_F1 51
#define KEYCODE_F2 52
#define KEYCODE_F3 53
#define KEYCODE_F4 54
#define KEYCODE_F5 55
#define KEYCODE_F6 56
#define KEYCODE_F7 57
#define KEYCODE_F8 58
#define KEYCODE_F9 59
#define KEYCODE_F10 60
#define KEYCODE_F11 61
#define KEYCODE_F12 62
#define KEYCODE_LEFT 65
#define KEYCODE_RIGHT 66
#define KEYCODE_UP 67
#define KEYCODE_DOWN 68
#define KEYCODE_LEFT_CTRL 70
#define KEYCODE_RIGHT_CTRL 71
#define KEYCODE_LEFT_SHIFT 72
#define KEYCODE_RIGHT_SHIFT 73
#define KEYCODE_LEFT_ALT 74
#define KEYCODE_RIGHT_ALT 75
#define KEYCODE_ENTER 76
#define KEYCODE_BACKSPACE 77
#define KEYCODE_SUPER 78
#define KEYCODE_CAPS_LOCK 79
#define KEYCODE_TAB 80
#define KEYCODE_ESCAPE 81
#define KEYCODE_PRINT_SCREEN 90
#define KEYCODE_SCROLL_LOCK 91
#define KEYCODE_PAUSE_BREAK 92
#define KEYCODE_INSERT 93
#define KEYCODE_HOME 94
#define KEYCODE_PAGE_UP 95
#define KEYCODE_PAGE_DOWN 96
#define KEYCODE_DELETE 97
#define KEYCODE_END 98

/// @brief Получает по данному сканкоду код клавиши.
/// @param scancode Сканкод
/// @return Код клавиши
uint8_t getKeyCode(uint64_t scancode);

#endif