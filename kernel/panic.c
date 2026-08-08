#include "panic.h"

#include "drivers/io/io.h"
#include "include/va_list.h"

void panic(const char *msg, int err_code) {
    kprintf("PANIC ERROR: %d, %s", err_code, msg);

    while (1) {}
}