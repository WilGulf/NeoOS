#ifndef ISR80H_PROMISE_H
#define ISR80H_PROMISE_H

#define PROMISE_FB     0B000001
#define PROMISE_INPUT  0B000010
#define PROMISE_EXEC   0B000100
#define PROMISE_MEMORY 0B001000
#define PROMISE_FS     0B010000
#define PROMISE_KILL   0B100000

#include "../include/stdint.h"
#include "../task/process.h"

kbool check_process_promise(struct process *process, uint32_t promise);

struct interrupt_frame;

void *isr80h_command10_promise(struct interrupt_frame *frame);

#endif