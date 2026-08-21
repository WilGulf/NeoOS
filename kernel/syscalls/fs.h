#ifndef ISR80H_FS_H
#define ISR80H_FS_H

struct interrupt_frame;

void *isr80h_command16_fopen(struct interrupt_frame *frame);
void *isr80h_command17_fclose(struct interrupt_frame *frame);

void *isr80h_command18_fread(struct interrupt_frame *frame);
void *isr80h_command19_fstat(struct interrupt_frame *frame);

#endif