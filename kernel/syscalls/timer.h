#ifndef ISR80H_TIMER_H
#define ISR80H_TIMER_H

struct interrupt_frame;

void *isr80h_command23_get_ticks_ms(struct interrupt_frame *frame);

#endif