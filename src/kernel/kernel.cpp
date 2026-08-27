/*
* 
* Наконец-то это случилось! Теперь мы можем писать
* на плюсах!
* 
* Так будет сильно проще, чем на ассемблере.
*
*/

#include "kernel.hpp"
#include "graphics/graphics.hpp"
#include "memmgr/memmgr.hpp"
#include "libk/util.hpp"
#include "io/com/com.hpp"
#include "int/int.hpp"
#include "dbg/dbgutil.hpp"

// Указатель на данные загрузчика
BootLoaderData* bld;

/// @brief Инициализирует данные, добытые в загрузчике.
void initBLD() {
    bld = (BootLoaderData*)0x100000;
    bld->DiskNo &= 0x7F;
}

/// @brief Подготавливает ядро к запуску.
/// @note Вызывается из @c _start.
extern "C"
void kernel_prep() {
    interrupt::cli();
    initBLD();
    initMemMgr();
    graphics::init();
    interrupt::init();
}

/// @brief Точка входа в ядро.
void main() {
    cout << "Добро пожаловать в СОпС вер. 0.2.0!\n\n";

    com::init();
    cout << "Инициализировано " << com::com_ports.length() << " COM-портов.\n";
}