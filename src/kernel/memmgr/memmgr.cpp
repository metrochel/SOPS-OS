#include "memmgr.hpp"

//
//  Вообще говоря, менеджер памяти будет
//  обязателен, когда мы начнём работать
//  с процессами, но так как до этого ещё
//  очень далеко, и я не до конца понимаю,
//  каким должен быть менеджер памяти,
//  здесь будет заглушка. Пока что будем
//  распределять память вручную.
//


void initMemMgr() {
    // TODO
}

void* kmalloc(dword amount) {
    // TODO
    return nullptr;
}

void kfree(void* ptr) {
    // TODO
}