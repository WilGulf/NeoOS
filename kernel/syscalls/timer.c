#include "timer.h"
#include "../timer/timer.h"
#include "../include/stdint.h"

void *isr80h_command23_get_ticks_ms(struct interrupt_frame *frame) {
    return timer_get_ms();
}