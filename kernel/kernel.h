#ifndef KERNEL_H
#define KERNEL_H

void kernel_page();
void kernel_registers();

struct kernel_info {
    char name[64];
    int version_major;
    int version_minor;
    int version_patch;
};

#endif