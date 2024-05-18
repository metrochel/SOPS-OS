#include <stdint-gcc.h>

uint16_t some_epic_function(uint16_t* W_pointer) {
    return *W_pointer;
}

char string[13] = "Hello World!";
uint8_t mybyte  = 0;

int main() {
    while (mybyte < 255)
        mybyte++;
    return mybyte;
}