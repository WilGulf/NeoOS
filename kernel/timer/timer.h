#ifndef TIMER_H
#define TIMER_H

#include "../include/stdint.h"

void timer_tick(void);
uint32_t timer_get_ms(void);

#endif