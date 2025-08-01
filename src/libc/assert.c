/*
 * assert.c
 *
 *
 */

#include "include/assert.h"
#include "include/stdio.h"
#include "include/stdlib.h"

void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *func) {
    printf("%s:%s:%d: Assertion \"%s\" failed\n", file, func, line, assertion);
    abort();
}