#include "filebase.hpp"
#include "../disk/disk.hpp"
#include "../io/com.hpp"

const dword defaultBufSize = 0x1000;

void File::create() {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Создание файла невозможно.\n");
}

bool File::read(byte *out) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Чтение из файла невозможно.\n");
}

bool File::read(dword start, dword size, byte *out) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Чтение из файла невозможно.\n");
}

bool File::write(byte *in, dword dataSize) {
    kdebug("ОШИБКА: Не указана файловая система\n");
    kdebug("Запись в файл невозможна.\n");
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