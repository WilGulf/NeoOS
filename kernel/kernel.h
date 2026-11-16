#ifndef KERNEL_H
#define KERNEL_H

void kernel_page();
struct paging_descriptor *kernel_descriptor();
void kernel_registers();

struct kernel_version {
    char name[64];
    int version_major;
    int version_minor;
    int version_patch;
    char extension[16];
};

struct kernel_version *get_kernel_version();

#endif