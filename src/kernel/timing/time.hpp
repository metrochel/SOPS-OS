//
//  Общая библиотека времени
//
//  - Управляет временем. (:
//
#ifndef _TIME_INCL
#define _TIME_INCL

#include "../util/nums.hpp"

// ### Время
// Класс, содержащий в себе описание о неком моменте времени.
class Time {

public:
    word year;      // Год
    byte month;      // Месяц
    byte day;        // День
    byte hours;      // Часы
    byte minutes;    // Минуты
    byte seconds;    // Секунды
    byte weekday;    // День недели

    byte asString(char* out);
    byte asStringFull(char* out);
    byte asStringWeekday(char* out);
    byte asStringMonth(char* out);

    bool operator==(Time t);
};

/// @brief Делает маленькую задержку.
inline void tinyWait() {
    __asm__ volatile ("nop");
};

// Системное время
extern Time systime;

/// @brief Достаёт время системы.
inline Time kgettime() {
    return systime;
}

/// @brief Изменяет время системы.
inline void ksettime(Time newtime) {
    systime = newtime;
}

#endif