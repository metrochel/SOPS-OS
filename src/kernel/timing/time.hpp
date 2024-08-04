//
//  Общая библиотека времени
//
//  - Управляет временем. (:
//

#define TIME_SIG

#include <stdint.h>

// ### Время
// Класс, содержащий в себе описание о неком моменте времени.
class Time {

public:
    uint16_t year;      // Год
    uint8_t month;      // Месяц
    uint8_t day;        // День
    uint8_t hours;      // Часы
    uint8_t minutes;    // Минуты
    uint8_t seconds;    // Секунды
    uint8_t weekday;    // День недели

    uint8_t asString(char* out);
    uint8_t asStringFull(char* out);
    uint8_t asStringWeekday(char* out);
    uint8_t asStringMonth(char* out);

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