#include "disk.h"
#include "../fs/file.h"

#include "../fs/ramfs/ramfs.h"

#include "../io/io.h"
#include "../../include/stdint.h"
#include "../../include/status.h"
#include "../../include/util.h"

struct disk disks[MAX_DISKS];
struct disk ram;

int disk_read_sector(int ata_base_port, int ata_drive_port, int lba, int total, void *buf) {
    outb(ata_base_port + 6, ata_drive_port | 0x40 | ((lba >> 24) & 0x0F)); // 0x1F6
    outb(ata_base_port + 2, total); // 0x1F2
    outb(ata_base_port + 3, (uint8_t)(lba & 0xFF)); // 0x1F3
    outb(ata_base_port + 4, (uint8_t)(lba >> 8)); // 0x1F4
    outb(ata_base_port + 5, (uint8_t)(lba >> 16)); // 0x1F5
    outb(ata_base_port + 7, 0x20); // 0x1F7

    uint16_t *ptr = buf;

    int timeout = 1000000;

    int i = 0;
    while (i < total) {
        char status = inb(ata_base_port + 7); // 0x1F7
        while (!(status & 0x08)) {
            if (timeout-- <= 0) {
                return -ERROR_IO;
            }
            status = inb(ata_base_port + 7); // 0x1F7
        }
        i++;

        for (int j = 0; j < 256; j++) {
            *ptr = inw(ata_base_port + 0); // 0x1F0
            ptr++;
        }
    }

    timeout = 1000000;

    while (1) {
        char status = inb(ata_base_port + 7); // 0x1F7

        if (status & 0x01) {
            return -ERROR_IO;
        }

        if (status & 0x20) {
            return -ERROR_IO;
        }

        if (!(status & 0x80)) {
            break;
        }

        if (--timeout == 0) {
            return -ERROR_IO;
        }
    }

    return 0;
}

int disk_write_sector(int ata_base_port, int ata_drive_port, int lba, int total, void *buf) {
    outb(ata_base_port + 6, ata_drive_port | 0x40 | ((lba >> 24) & 0x0F)); // 0x1F6
    outb(ata_base_port + 2, total); // 0x1F2
    outb(ata_base_port + 3, (uint8_t)(lba & 0xFF)); // 0x1F3
    outb(ata_base_port + 4, (uint8_t)(lba >> 8)); // 0x1F4
    outb(ata_base_port + 5, (uint8_t)(lba >> 16)); // 0x1F5
    outb(ata_base_port + 7, 0x30); // 0x1F7

    uint16_t *ptr = (uint16_t *) buf;
    int i = 0;
    while (i < total) {
        int timeout = 1000000;

        char status = inb(ata_base_port + 7);
        while (!(status & 0x08)) {
            if (--timeout == 0) {
                return -ERROR_IO;
            }

            status = inb(ata_base_port + 7);
        }

        for (int i = 0; i < 256; i++) {
            outw(ata_base_port + 0, *ptr);
            ptr++;
        }

        i++;
    }

    int timeout = 1000000;

    while (1) {
        uint8_t status = inb(ata_base_port + 7);

        if (status & 0x01) {
            return -ERROR_IO;
        }

        if (status & 0x20) {
            return -ERROR_IO;
        }

        if (!(status & 0x80)) {
            break;
        }

        if (--timeout == 0) {
            return -ERROR_IO;
        }
    }

    return 0;
}

int ata_wait(int ata_base_port) {
    int timeout = 1000000;

    while (1) {
        char status = inb(ata_base_port + 7);
        if (status & 0x01) {
            return -ERROR_IO;
        }

        if (status & 0x20) {
            return -ERROR_IO;
        }

        if (!(status & 0x80)) {
            break;
        }

        if (--timeout == 0) {
            return -ERROR_IO;
        }
    }

    return 0;
}

struct disk *get_free_disk() {
    for (int i = 0; i < MAX_DISKS; i++) {
        if (!disks[i].filesystem) {
            return &disks[i];
        }
    }

    return 0;
}

int init_disk(int ata_drive_port, int ata_base_port, char id) {
    struct disk *disk = get_free_disk();
    if (!disk) {
        return -ERROR_IO;
    }

    memset(disk, 0, sizeof(struct disk));
    disk->type = DISK_TYPE_REAL;
    disk->sector_size = SECTOR_SIZE;
    disk->id = id;
    disk->ata_drive_port = ata_drive_port;
    disk->ata_base_port = ata_base_port;
    disk->filesystem = fs_resolve(disk);

    return 0;
}

int check_drive(int drive, int ata_base_port) {
    outb(ata_base_port + 6, drive); // 1F6
    outb(ata_base_port + 3, 0); // 1F3
    outb(ata_base_port + 4, 0); // 1F4
    outb(ata_base_port + 5, 0); // 1F5
    outb(ata_base_port + 7, 0xEC); // 1F7
    char status = inb(ata_base_port + 7); // 1F7
    if (ata_wait(ata_base_port) < 0) {
        return -ERROR_IO;
    }

    if (!inb(ata_base_port + 4) && !inb(ata_base_port + 5)) {
        int timeout = 1000000;

        while (!(status & 0x08)) {
            if (--timeout == 0) {
                return -ERROR_IO;
            }
            if (status & 0x01) {
                break;
            }

            status = inb(ata_base_port + 7);
        }

        uint16_t buf[256];
        uint16_t *ptr = buf;
        for (int j = 0; j < 256; j++) {
            ptr[j] = inw(ata_base_port + 0); // 0x1F0
        }

        return true;
    }

    return false;
}

int disks_search_and_init() {
    int disks_found = 0;
    
    if (check_drive(0xA0, 0x1F0) == true) {
        init_disk(0xA0, 0x1F0, '0');
        disks_found++;
    }
    if (check_drive(0xB0, 0x1F0) == true) {
        init_disk(0xB0, 0x1F0, '1');
        disks_found++;
    }
    if (check_drive(0xA0, 0x170) == true) {
        init_disk(0xA0, 0x170, '2');
        disks_found++;
    }
    if (check_drive(0xB0, 0x170) == true) {
        init_disk(0xB0, 0x170, '3');
        disks_found++;
    }

    return disks_found;
}

void ramdisk_init() {
    memset(&ram, 0, sizeof(ram));
    ram.type = DISK_TYPE_PSEUDO;
    ram.id = 'D';
    ram.filesystem = ramfs_init();
}

struct disk *disk_get(char id) {
    if (id == 'D') {
        return &ram;
    }

    if (char_is_digit(id)) {
        int index = char_to_int(id);
        for (int i = 0; i < MAX_DISKS; i++) {
            if (disks[i].id == id) {
                return &disks[i];
            }
        }
    }

    return 0;
}

int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf) {
    return disk_read_sector(idisk->ata_base_port, idisk->ata_drive_port, lba, total, buf);
}

int disk_write_block(struct disk *idisk, uint32_t lba, int total, void *buf) {
    return disk_write_sector(idisk->ata_base_port, idisk->ata_drive_port, lba, total, buf);
}