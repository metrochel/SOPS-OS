//
//  Оболочка
//
//  - Временная консольная оболочка до того момента, пока не выйдем в графику.
//

/// @brief Основная функция оболочки.
void shellMain();

/// @brief Отвечает за логику команды `shutdown`.
void cmdShutdown();
/// @brief Отвечает за логику команды `reboot`.
void cmdReboot();
/// @brief Отвечает за логику команды `time`.
void cmdTime();
/// @brief Отвечает за логику команды `ls`.
void cmdLs();
