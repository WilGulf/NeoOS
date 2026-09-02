#include "timer.h"
#include "../include/stdint.h"
#include "../drivers/io/io.h"

volatile uint32_t kernel_milliseconds = 0;

void timer_tick(void) {
    kernel_milliseconds++;
}

void timer_init(void) {
    uint16_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t timer_get_ms(void) {
    return kernel_milliseconds;
}