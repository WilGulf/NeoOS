#include "timer.h"
#include "../include/stdint.h"
#include "../drivers/io/io.h"

volatile uint32_t kernel_milliseconds = 0;

void timer_tick(void) {
    kernel_milliseconds++;
}

uint32_t timer_get_ms(void) {
    return kernel_milliseconds;
}