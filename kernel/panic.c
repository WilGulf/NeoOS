#include "panic.h"

#include "drivers/io/io.h"
#include "include/va_list.h"

int pprintf(const char *fmt, ...) {
    va_list args;

    char message[1024];
    char buffer[1152];

    uint32_t ms = 0;//timer_get_ms();
    int sec = ms / 1000;
    int sec_ms = ms % 1000;

    va_start(args, fmt);

    vsprintf(message, fmt, args);

    va_end(args);

    sprintf(
        buffer,
        "[ PANIC ] [%d.%d] %s",
        sec,
        sec_ms,
        message
    );

    writer(buffer);

    return 0;
}

void panic(const char *msg, int err_code) {
    pprintf("%d, %s", err_code, msg);
    while (1) {}
}