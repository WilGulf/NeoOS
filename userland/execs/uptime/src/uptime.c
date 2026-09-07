#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    uint32_t ms = get_ticks_ms();
    uint32_t total_seconds = ms / 1000;

    uint32_t hours   = total_seconds / 3600;
    uint32_t minutes = (total_seconds / 60) % 60;
    uint32_t seconds = total_seconds % 60;

    printf("Uptime: ");

    if (hours) {
        printf("Hours: %d ", hours);
    }
    if (minutes) {
        printf("Minutes: %d ", minutes);
    }
    printf("Seconds: %d\n", seconds);

    return 0;
}