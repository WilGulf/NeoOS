#include "promise.h"

#include "../task/process.h"
#include "../include/stdint.h"

#include "../drivers/io/io.h"

kbool check_process_promise(struct process *process, uint32_t promise) {
    return (process->promises & promise) != 0;
}

void *isr80h_command10_promise(struct interrupt_frame *frame) {    
    uint32_t promise = (uint32_t)task_get_stack_item(task_current(), 0);
    
    if (task_current()->process->declared) {
        process_terminate(task_current()->process);
        task_next();
        return 0;
    }

    task_current()->process->promises = promise;
    task_current()->process->declared = true;
    return 0;
}

void *isr80h_command11_wait_for_promise(struct interrupt_frame *frame) {
    while (!task_current()->process->declared) {}

    return 0;
}