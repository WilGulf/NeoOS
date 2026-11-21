#include "disk.h"
#include "../fs/file.h"

#include "../fs/ramfs/ramfs.h"

#include "../io/io.h"
#include "../../include/stdint.h"
#include "../../include/status.h"
#include "../../include/util.h"
#include "../../memory/kheap.h"

struct disk *disks[MAX_DISKS];
struct disk *system_disk;
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

int get_free_disk_index(void) {
    for (int i = 0; i < MAX_DISKS; i++) {
        if (disks[i] == NULL) {
            return i;
        }
    }

    return -1;
}

kbool id_available(char id) {
    if (!id) {
        return false;
    }

    if (id == '@') {
        //LINKED TO SYSTEM DISK, CAN'T BE USED FOR A DISK
        return false;
    }

    if (id == 'D') {
        //KERNELS RAM FILESYSTEM, CAN'T BE USED FOR A DISK
        return false;
    }

    //CAN'T BE USED IF ALREADY IN USE
    for (int i = 0; i < MAX_DISKS; i++) {
        if (disks[i]->id == id) {
            return false;
        }
    }

    //PASSED
    return true;
}

char get_id(void) {
    //LOOP THROUGH 0..9
    for (int i = 0; i < 10; i++) {
        if (id_available('0' + i)) {
            return '0' + i;
        }
    }

    //LOOP TROUGH A..Z
    for (int i = 0; i < 26; i++) {
        if (id_available('A' + i)) {
            return 'A' + i;
        }
    }

    return 0;
}

int disk_new(char id, int type, int starting_lba, int ending_lba, size_t sector_size, struct disk **disk_out, int ata_drive_port, int ata_base_port) {
    if (!id) {
        id = get_id();    
    }
    
    if (!id_available(id)) {
        return -ERROR_INVALID_ARG;
    }

    int res = 0;
    struct disk *disk = kzalloc(sizeof(struct disk));
    if (!disk) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    disk->id = id;
    disk->type = type;
    disk->sector_size = sector_size;
    disk->starting_lba = starting_lba;
    disk->ending_lba = ending_lba;
    disk->ata_drive_port = ata_drive_port;
    disk->ata_base_port = ata_base_port;
    
    disk->filesystem = fs_resolve(disk);
    if (disk->filesystem) {
        
    }

    if (disk_out) {
        *disk_out = disk;
    }

    int disk_index = get_free_disk_index();
    if (disk_index < 0) {
        res = -1;
        goto out;
    }

    disks[disk_index] = disk;
out:
    return res;
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
        disk_new(0, DISK_TYPE_REAL, 0, 0, SECTOR_SIZE, NULL, 0xA0, 0x1F0);
        disks_found++;
    }
    if (check_drive(0xB0, 0x1F0) == true) {
        disk_new(0, DISK_TYPE_REAL, 0, 0, SECTOR_SIZE, NULL, 0xB0, 0x1F0);
        disks_found++;
    }
    if (check_drive(0xA0, 0x170) == true) {
        disk_new(0, DISK_TYPE_REAL, 0, 0, SECTOR_SIZE, NULL, 0xA0, 0x170);
        disks_found++;
    }
    if (check_drive(0xB0, 0x170) == true) {
        disk_new(0, DISK_TYPE_REAL, 0, 0, SECTOR_SIZE, NULL, 0xB0, 0x170);
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

    if (id == '@') {
        return system_disk;
    }

    for (int i = 0; i < MAX_DISKS; i++) {
        if (disks[i]->id == id) {
            return disks[i];
        }
    }

    return NULL;
}

int disk_read_block(struct disk *idisk, uint32_t lba, int total, void *buf) {
    size_t absolute_lba = idisk->starting_lba + lba;
    size_t absolute_ending_lba = absolute_lba + total;
    if (absolute_ending_lba > idisk->ending_lba) {
        if (idisk->starting_lba != 0 && idisk->ending_lba != 0) {
            return ERROR_IO;
        }
    }

    return disk_read_sector(idisk->ata_base_port, idisk->ata_drive_port, absolute_lba, total, buf);
}

int disk_write_block(struct disk *idisk, uint32_t lba, int total, void *buf) {
    size_t absolute_lba = idisk->starting_lba + lba;
    size_t absolute_ending_lba = absolute_lba + total;
    if (absolute_ending_lba > idisk->ending_lba) {
        if (idisk->starting_lba != 0 && idisk->ending_lba != 0) {
            return ERROR_IO;
        }
    }

    return disk_write_sector(idisk->ata_base_port, idisk->ata_drive_port, lba, total, buf);
}