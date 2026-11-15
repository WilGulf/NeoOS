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

#define PAGING_TOTAL_ENTRIES_PER_TABLE 512

#define PAGE_SIZE 4096

enum {
    PAGING_MAP_LEVEL_4 = 4,
    PAGING_MAP_LEVEL_5 = 5,
};

typedef uint8_t paging_map_level_t;

enum {
    PAGING_PLM4T_MAX_ADDRESSABLE = 0x8000000000,
    PAGING_PDPT_MAX_ADDRESSABLE = 0x40000000,
    PAGING_PD_MAX_ADDRESSABLE = 0x200000,
    PAGING_PAGE_MAX_ADDRESSABLE = PAGE_SIZE,
};

struct paging_descriptor_entry {
    uint64_t present : 1;
    uint64_t read_write : 1;
    uint64_t user_supervisor : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t ignored : 1;
    uint64_t reserved0 : 1;
    uint64_t reserved1 : 4;
    uint64_t address : 40;
    uint64_t available : 11;
    uint64_t execute_disable : 1;
} __attribute__((packed));

struct paging_pml_entries {
    struct paging_descriptor_entry entries[PAGING_TOTAL_ENTRIES_PER_TABLE];
} __attribute__((packed));

struct paging_descriptor {
    struct paging_pml_entries *pml;
    paging_map_level_t level;
} __attribute__((packed));


struct paging_pml_entries *paging_pml4_entries_new();
struct paging_descriptor *paging_current_descriptor();
bool paging_is_aligned(void *addr);
void *paging_align_address(void *ptr);
void *paging_align_to_lower_page(void *addr);
struct paging_descriptor *paging_descriptor_new(paging_map_level_t root_map_level);
int paging_map(struct paging_descriptor *desc, void *virt, void *phys, int flags);
int paging_map_range(struct paging_descriptor *desc, void *virt, void *phys, size_t count, int flags);
int paging_map_to(struct paging_descriptor *desc, void *virt, void *phys, void *phys_end, int flags);
int paging_map_e820_memory_regions(struct paging_descriptor *desc);
void *paging_get_physical_address(struct paging_descriptor *desc, void *vaddr);

void paging_invalidate_tlb_entry(void *addr);
void paging_load_directory(uintptr_t *directory);
void paging_switch(struct paging_descriptor *desc);

#endif