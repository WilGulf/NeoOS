#ifndef KERNEL_H
#define KERNEL_H

struct kernel_version {
    char name[64];
    int version_major;
    int version_minor;
    int version_patch;
    char extension[16];
};

struct kernel_info {
    struct kernel_version version;
};

#endif