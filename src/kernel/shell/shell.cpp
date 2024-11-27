#include "shell.hpp"
#include "../graphics/glyphs.hpp"
#include "../keyboard/keyboard.hpp"
#include "../timing/time.hpp"
#include "../str/str.hpp"
#include "../acpi/acpi.hpp"

void shellMain() {
    byte *stdin = (byte*)0x9300;
    while (true) {
        kprint("\n>");
        kread(stdin);
        kprint("\n");
        stdin = (byte*)0x9300;
        if (strcmp((char*)stdin, (char*)"time")) {
            cmdTime();
        } 
        else if (strcmp((char*)stdin, (char*)"shutdown")) {
            cmdShutdown();
        } 
        else if (strcmp((char*)stdin, (char*)"reboot")) {
            cmdReboot();
        } 
        else if (strcmp((char*)stdin, (char*)"ls")) {
            cmdLs();
        }
        else {
            kerror("ОШИБКА: Команды или исполняемого файла \"");
            kerror((const char*)stdin);
            kerror("\" не существует.\nПроверьте правильность написания команды.");
        }
    }
}

void cmdTime() {
    kprint("Сейчас ");
    char *out = (char*)0x11000;
    Time time = kgettime();
    out = (char*)0x11000;
    out += time.asStringWeekday(out);
    *out++ = ',';
    *out++ = ' ';
    time.asStringFull(out);
    out -= 6;
    kprint(out);
    kprint(".");
}

void cmdShutdown() {
    kprint("До свидания!");
    kshutdown();
}

void cmdReboot() {
    kprint("Перезагрузка!");
    krestart();
}

void cmdLs() {
    kprint("// TODO");
}