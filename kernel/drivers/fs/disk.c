#include "disk.h"
#include "file.h"

#include "ramfs/ramfs.h"

#include "../io/io.h"
#include "../../include/stdint.h"
#include "../../include/status.h"
#include "../../include/util.h"

struct disk disk;
struct disk dev;

int disk_read_sector(int lba, int total, void *buf) {
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (uint8_t)(lba & 0xFF));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    uint16_t *ptr = (uint16_t *) buf;
    for (int b = 0; b < total; b++) {
        char c = inb(0x1F7);
        while(!(c & 0x08)) {
            c = inb(0x1F7);
        }

        for (int i = 0; i < 256; i++) {
            *ptr = inw(0x1F0);
            ptr++;
        }
    }

    return 0;
}

void disk_search_and_init() {
    memset(&disk, 0, sizeof(disk));
    disk.type = DISK_TYPE_REAL;
    disk.sector_size = SECTOR_SIZE;
    disk.id = '0';
    disk.filesystem = fs_resolve(&disk);
}

void disk_dev_init() {
    memset(&dev, 0, sizeof(dev));
    dev.type = DISK_TYPE_PSEUDO;
    dev.id = 'D';
    dev.filesystem = ramfs_init();
}

struct disk *disk_get(char id) {
    if (id == 'D') {
        return &dev;
    }

    if (char_is_digit(id)) {
        int index = char_to_int(id);
        if (index == 0) {
            return &disk;
        }
    }

    return 0;
}

int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf) {
    if (idisk != &disk) {
        return -ERROR_IO;
    }

    return disk_read_sector(lba, total, buf);
}