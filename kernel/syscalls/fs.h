#ifndef ISR80H_FS_H
#define ISR80H_FS_H

struct interrupt_frame;

void *isr80h_command18_fopen(struct interrupt_frame *frame);
void *isr80h_command19_fclose(struct interrupt_frame *frame);

void *isr80h_command20_fread(struct interrupt_frame *frame);
void *isr80h_command21_fstat(struct interrupt_frame *frame);

#endif