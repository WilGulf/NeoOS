#include "heap.h"
#include "promise.h"

#include "../task/task.h"
#include "../task/process.h"
#include "../include/stdint.h"

void *isr80h_command4_malloc(struct interrupt_frame *frame) {
    if (!check_process_promise(task_current()->process, PROMISE_MEMORY)) {
        //process_terminate(task_current()->process);
        //task_next();
        //return 0;
    };

    size_t size = (size_t)task_get_stack_item(task_current(), 0);
    return process_malloc(task_current()->process, size);
}

void *isr80h_command5_free(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_MEMORY);
    
    void *ptr = task_get_stack_item(task_current(), 0);
    process_free(task_current()->process, ptr);
    return 0;
}