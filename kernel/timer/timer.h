#ifndef TIMER_H
#define TIMER_H

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

#define PIT_BASE_FREQUENCY 1193182
#define PIT_FREQUENCY 1000

#include "../include/stdint.h"

void timer_tick(void);
void timer_init(void);
uint32_t timer_get_ms(void);

#endif