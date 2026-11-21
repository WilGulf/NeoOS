#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"
#include "../../include/stdint.h"

#define MAX_DISKS 36
//0..9 & A..Z

#define DISK_TYPE_REAL 0
#define DISK_TYPE_PARTITION 1
#define DISK_TYPE_PSEUDO 2
#define SECTOR_SIZE 512

typedef unsigned int DISK_TYPE;

struct disk {
    DISK_TYPE type;
    int sector_size;

    char id;

    struct filesystem* filesystem;

    size_t starting_lba;
    size_t ending_lba;

    void *fs_private;

    uint32_t ata_drive_port;
    uint32_t ata_base_port;
};

int disk_new(char id, int type, int starting_lba, int ending_lba, size_t sector_size, struct disk **disk_out, int ata_drive_port, int ata_base_port);

int disks_search_and_init();
void ramdisk_init();

struct disk *disk_get(char id);
int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf);
int disk_write_block(struct disk *idisk, uint32_t lba, int total, void *buf);

#endif