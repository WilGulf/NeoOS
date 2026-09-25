/*
 *
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
  * This file is drerived from the PeachOS Kernel (github.com/nibblebits/PeachOS).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * 
 * See NOTICE for the full list of GPL-derived files in this project.
 */

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
int disk_write_block(struct disk *idisk, uint32_t lba, int total, void *buf);

#endif