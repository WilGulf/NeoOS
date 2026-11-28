#include "stdlib.h"
#include "stdio.h"
#include "va_list.h"

void declarations(void) {
    promise(PROMISE_EXEC | PROMISE_FB);
}

int logf(const char *fmt, ...) {
    va_list args;

    char message[1024];

    uint32_t ms = get_ticks_ms();
    int sec = ms / 1000;
    int sec_ms = ms % 1000;

    va_start(args, fmt);

    vsprintf(message, fmt, args);

    va_end(args);

    printf(
        "[ LAUNCHER ] [%d.%d] %s",
        sec,
        sec_ms,
        message
    );

    return 0;
}

int main(int argc, char **argv) {
    logf("BOOTING\n");
    drop_privilege(2);
    fork_as("@:/execs/sh", 3);
    while (1) {}
}