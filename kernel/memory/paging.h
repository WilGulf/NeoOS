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

#ifndef PAGING_H
#define PAGING_H

#include "../include/stdint.h"

#define PAGING_CACHE_DISABLED   0B00010000
#define PAGING_WRITE_THROUGH    0B00001000
#define PAGING_ACCESS_FROM_ALL  0B00000100
#define PAGING_IS_WRITEABLE     0B00000010
#define PAGING_IS_PRESENT       0B00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGE_SIZE 4096

struct paging_4gb_chunk {
    uint32_t *directory_entry;
};

struct paging_4gb_chunk *paging_new_4gb(uint8_t flags);
void paging_free_4gb(struct paging_4gb_chunk *chunk);

void paging_switch(struct paging_4gb_chunk *chunk);
void enable_paging();

int paging_set(uint32_t *directory, void *vaddr, uint32_t val);
kbool paging_is_aligned(void *addr);

void *paging_align_address(void *ptr);
void *paging_align_to_lower_page(void *addr);

int paging_map_to(struct paging_4gb_chunk *directory, void *virt, void *phys, void *phys_end, int flags);
int paging_map_range(struct paging_4gb_chunk *directory, void *virt, void *phys, int count, int flags);
int paging_map(struct paging_4gb_chunk *directory, void *virt, void *phys, int flags);

uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk);

uint32_t paging_get(uint32_t *directory, void *virt);

void *paging_get_physical_address(uint32_t *directory, void *virt);

#endif