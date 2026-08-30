#include "timer.h"
#include "../include/stdint.h"
#include "../drivers/io/io.h"

volatile uint32_t kernel_milliseconds = 0;

void timer_tick(void) {
    kernel_milliseconds++;
}

void timer_init(void) {
    uint16_t divisor = PIT_BASE_FREQUENCY / PIT_FREQUENCY;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint32_t timer_get_ms(void) {
    return kernel_milliseconds;
}