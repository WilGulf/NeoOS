#ifndef DISK_H
#define DISK_H

#include "../../include/stdint.h"

#define DISK_TYPE_REAL 0
#define SECTOR_SIZE 512

typedef unsigned int DISK_TYPE;

struct disk {
    DISK_TYPE type;
    int sector_size;
};

void disk_search_and_init();
struct disk *disk_get(int index);
int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf);

#endif