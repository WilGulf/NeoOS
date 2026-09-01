#ifndef ISR80H_IO_H
#define ISR80H_IO_H

struct interrupt_frame;
void *isr80h_command4_print(struct interrupt_frame *frame);
void *isr80h_command5_putchar(struct interrupt_frame *frame);
void *isr80h_command6_fb_clear(struct interrupt_frame *frame);
void *isr80h_command7_getkey(struct interrupt_frame *frame);
void *isr80h_command8_get_key_event(struct interrupt_frame *frame);

void *isr80h_command24_disable_cursor(struct interrupt_frame *frame);
void *isr80h_command25_enable_cursor(struct interrupt_frame *frame);

#endif