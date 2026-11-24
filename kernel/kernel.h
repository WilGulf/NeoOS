#ifndef KERNEL_H
#define KERNEL_H

#include "drivers/fs/file.h"

extern struct file_descriptor *kernel_file_descriptors[MAX_FILE_DESCRIPTORS];

void kernel_page(void);
struct paging_descriptor *kernel_descriptor(void);
void kernel_registers(void);

struct kernel_version {
    char name[64];
    int version_major;
    int version_minor;
    int version_patch;
    char extension[16];
};

struct kernel_version *get_kernel_version(void);

#endif