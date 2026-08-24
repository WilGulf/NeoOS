#ifndef ISR80H_KERNEL_H
#define ISR80H_KERNEL_H

#include "../kernel.h"

struct interrupt_frame;

struct kernel_info {
    struct kernel_version version;
};

void *isr80h_command3_get_kernel_info(struct interrupt_frame *frame);

#endif