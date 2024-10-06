#include "acpi.hpp"
#include "../graphics/glyphs.hpp"

__attribute__((interrupt)) void sciHandler(IntFrame *frame) {
    kdebug("Возникло SCI.\n");

    word sts = readPM1aSts() | readPM1bSts();
    kdebug("Статус PM1: %b.\n", sts);

    if (sts & 256) {
        kdebug("Событие вызвано нажатием кнопки питания.\n");
        writePM1aSts(256);
        writePM1bSts(256);
        int_exit_slave();
        kshutdown();
    }

    if (sts & 512) {
        kdebug("Событие вызвано нажатием кнопки сна.\n");
        writePM1aSts(512);
        writePM1bSts(512);
        int_exit_slave();
        ksleep();
    }

    int_exit_slave();
}