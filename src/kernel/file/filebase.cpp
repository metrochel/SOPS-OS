#include "filebase.hpp"
#include "../disk/disk.hpp"
#include "../io/com.hpp"

const dword defaultBufSize = 0x1000;

bool File::create() {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Создание файла невозможно.\n");
    return false;
}

dword File::read(byte *out) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Чтение из файла невозможно.\n");
    return 0;
}

dword File::read(dword start, dword size, byte *out) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Чтение из файла невозможно.\n");
    return 0;
}

dword File::write(dword write_start, dword write_size, byte *in) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Запись в файл невозможна.\n");
    return 0;
}

dword File::write(dword write_size, byte *in) {
    return write(size, write_size, in);
}

void File::rename(char *newname) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Переименование файла невозможно.\n");
}

void File::remove() {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Удаление файла невозможно.\n");
}

void File::clear() {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Очистка файла невозможна.\n");
}

bool File::operator!() {
    return name == nullptr;
}