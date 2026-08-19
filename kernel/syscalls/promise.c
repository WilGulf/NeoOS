#include "promise.h"

#include "../task/process.h"
#include "../include/stdint.h"
#include "../include/status.h"
#include "../include/util.h"

#include "../drivers/io/io.h"

kbool check_process_promise(struct process *process, uint32_t promise) {
    if (process->promises & promise == 0) {
        kprintf("Promise broken\n");
        kprintf("Promise Terminated\n");
        process_terminate(process);
        task_next();
        return 0;
    }

    return 1;
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