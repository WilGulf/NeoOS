#ifndef DISK_H
#define DISK_H

#include "file.h"
#include "../../include/stdint.h"

#define DISK_TYPE_REAL 0
#define DISK_TYPE_PSEUDO 1
#define SECTOR_SIZE 512

typedef unsigned int DISK_TYPE;

struct disk {
    DISK_TYPE type;
    int sector_size;

    char id;

    struct filesystem* filesystem;

    void *fs_private;
};

void disk_search_and_init();
void disk_dev_init();

struct disk *disk_get(char id);
int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf);

#endif