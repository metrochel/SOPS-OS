//
//  Библиотека для управления SCI
//
//  - Содержит обработчик SCI.
//
#include "../int/int.hpp"

/// @brief Обрабатывает SCI.
/// @param frame Структура прерывания
__attribute__((interrupt)) void sciHandler(IntFrame *frame);