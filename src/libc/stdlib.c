#include "include/stdlib.h"
#include "etc/syscalls.h"

void exit(int code) {
	syscall1(0, code);
}