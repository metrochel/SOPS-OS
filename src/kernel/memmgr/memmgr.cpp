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

void memset(uint8_t *ptr, uint32_t count, uint8_t val) {
    for (uint32_t i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

void memcpy(uint8_t* a, uint8_t* b, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        a[n] = b[n];
    }
}

void* kmalloc(uint32_t amount) {
    // TODO
}

void kfree(void* ptr) {
    // TODO
}