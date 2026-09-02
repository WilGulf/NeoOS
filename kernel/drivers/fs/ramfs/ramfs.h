#ifndef RAMFS_H
#define RAMFS_H

#define RAMFS_MAX_NAME 32

#include "../../../include/stdint.h"

struct disk;

struct filesystem *ramfs_init();
int ramfs_resolve(struct disk *disk);


struct ramfs_file {
    char name[32];

    uint8_t is_dir;
    uint8_t total_children;
    struct ramfs_file *first_child;

    void *memory;
    uint32_t size;
    uint32_t total_size;

    struct ramfs_file *parent;

    struct ramfs_file *next;
    struct ramfs_file *prev;
};

#endif