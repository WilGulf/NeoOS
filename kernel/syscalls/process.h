#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

#include "../include/config.h"
#include "../include/stdint.h"

struct process_public {
    uint16_t id;
    uint8_t privilege;
    char filename[MAX_PATH];
};

struct interrupt_frame;

void *isr80h_command6_process_load_start(struct interrupt_frame *frame);
void *isr80h_command7_system(struct interrupt_frame *frame);
void *isr80h_command8_get_process_arguments(struct interrupt_frame *frame);
void *isr80h_command0_exit(struct interrupt_frame *frame);
void *isr80h_command12_fork(struct interrupt_frame *frame);
void *isr80h_command21_get_processes(struct interrupt_frame *frame);
void *isr80h_command22_terminate_process(struct interrupt_frame *frame);

#endif