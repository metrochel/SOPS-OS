#include "time.hpp"
#include "../str/str.hpp"
#include "../io/io.hpp"
#include "../io/com.hpp"
#include "../dbg/dbg.hpp"

Time systime = {0,0,0,0,0,0,0};

byte Time::asString(char* out) {
    *out++ = (this->hours / 10) + 0x30;
    *out++ = (this->hours % 10) + 0x30;
    *out++ = ':';
    *out++ = (this->minutes / 10) + 0x30;
    *out++ = (this->minutes % 10) + 0x30;
    *out++ = ':';
    *out++ = (this->seconds / 10) + 0x30;
    *out++ = (this->seconds % 10) + 0x30;
    return 8;
}

byte Time::asStringFull(char* out) {
    char *initOut = out;
    out += numasstr(this->day, out);
    *out++ = ' ';
    out += this->asStringMonth(out);
    *out++ = ' ';
    out += numasstr(this->year, out);
    *out++ = ',';
    *out++ = ' ';
    out += this->asString(out);
    return out - initOut;
}

byte Time::asStringWeekday(char* out) {
    switch (this->weekday) {
        case 1: return strcpy((char*)"Вс", out); break;
        case 2: return strcpy((char*)"Пн", out); break;
        case 3: return strcpy((char*)"Вт", out); break;
        case 4: return strcpy((char*)"Ср", out); break;
        case 5: return strcpy((char*)"Чт", out); break;
        case 6: return strcpy((char*)"Пт", out); break;
        case 7: return strcpy((char*)"Сб", out); break;
    }
}

byte Time::asStringMonth(char* out) {
    switch (this->month) {
        case 1: return strcpy((char*)"января", out);
        case 2: return strcpy((char*)"февраля", out);
        case 3: return strcpy((char*)"марта", out);
        case 4: return strcpy((char*)"апреля", out);
        case 5: return strcpy((char*)"мая", out);
        case 6: return strcpy((char*)"июня", out);
        case 7: return strcpy((char*)"июля", out);
        case 8: return strcpy((char*)"августа", out);
        case 9: return strcpy((char*)"сентября", out);
        case 10: return strcpy((char*)"октября", out);
        case 11: return strcpy((char*)"ноября", out);
        case 12: return strcpy((char*)"декабря", out);
    }
}

bool Time::operator==(Time t) {
    return t.seconds == this->seconds && t.minutes == this->minutes && t.hours == this->hours && t.day == this->day && t.month == this->month && t.year == this->year && t.weekday == this->weekday;
}

void sleepM(dword ms) {
    sleepU(ms * 1000);
}

void sleepU(dword us) {
    qword ticks = ((qword)(us*1000) + 29) / 30;
    for (qword i = 0; i < ticks; i++)
        io_wait();
}