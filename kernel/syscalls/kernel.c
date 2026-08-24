#include "kernel.h"

#include "../kernel.h"
#include "../include/util.h"
#include "../task/task.h"
#include "../drivers/io/io.h"

void *isr80h_command3_get_kernel_info(struct interrupt_frame *frame) {
    struct kernel_info kernel_info;
    void *out_ptr = task_get_stack_item(task_current(), 0);

    memcpy(&kernel_info.version, get_kernel_version(), sizeof(kernel_info.version));

    task_page();
    memcpy(out_ptr, &kernel_info, sizeof(kernel_info));
    kernel_page();
    
    return 0;
}