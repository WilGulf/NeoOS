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

#include "paging.h"
#include "memory.h"
#include "kheap.h"
#include "heap.h"

#include "../panic.h"
#include "../drivers/io/io.h"
#include "../include/util.h"
#include "../include/status.h"

static struct paging_descriptor *current_paging_descriptor = 0;

struct paging_pml_entries *paging_pml4_entries_new() {
    struct paging_pml_entries *entries_descriptor = kzalloc(sizeof(struct paging_pml_entries));
    return entries_descriptor;
}

static kbool paging_map_level_is_valid(paging_map_level_t level) {
    return level == PAGING_MAP_LEVEL_4;
}

struct paging_descriptor *paging_current_descriptor() {
    return current_paging_descriptor;
}

void *paging_align_address(void *ptr) {
    if ((uintptr_t)ptr % PAGE_SIZE) {
        return (void *)((uintptr_t)ptr + PAGE_SIZE - ((uintptr_t)ptr % PAGE_SIZE));
    }
    return ptr;
}

void *paging_align_to_lower_page(void *addr) {
    uintptr_t _addr = (uintptr_t)addr;
    _addr -= (_addr % PAGE_SIZE);
    return (void *)_addr;
}

void paging_switch(struct paging_descriptor *desc) {
    current_paging_descriptor = desc;
    paging_load_directory((uint64_t *)(&desc->pml->entries[0]));
}

kbool paging_is_aligned(void *addr) {
    return ((uintptr_t)addr % PAGE_SIZE) == 0;
}

static kbool paging_null_entry(struct paging_descriptor_entry *entry) {
    struct paging_descriptor_entry null_desc = {0};
    return memcmp(entry, &null_desc, sizeof(struct paging_descriptor_entry)) == 0;
}

struct paging_descriptor *paging_descriptor_new(paging_map_level_t root_map_level) {
    if (!paging_map_level_is_valid(root_map_level)) {
        return NULL;
    }

    struct paging_descriptor *descriptor = kzalloc(sizeof(struct paging_descriptor));
    if (!descriptor) {
        return NULL;
    }

    descriptor->pml = paging_pml4_entries_new();
    descriptor->level = root_map_level;
    return descriptor;
}

void paging_descriptor_entry_free(struct paging_descriptor_entry *table_entry, paging_map_level_t level) {
    if (paging_null_entry(table_entry)) {
        return;
    }

    if (level == 0) {
        panic("", 1);
    }

    if (level > PAGING_MAP_LEVEL_4) {
        panic("", 1);
    }

    if (level > 1) {
        for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
            struct paging_descriptor_entry *entry = &table_entry[i];
            if (!paging_null_entry(entry)) {
                struct paging_descriptor_entry *child_entry = (struct paging_descriptor_entry *)((uint64_t)(entry->address) << 12);
                if (child_entry) {
                    paging_descriptor_entry_free(child_entry, level - 1);
                }
            }
        }
    }

    kfree(table_entry);
}

void paging_descriptor_free(struct paging_descriptor *descriptor) {
    paging_map_level_t level = descriptor->level;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        struct paging_descriptor_entry *entry = &descriptor->pml->entries[i];
        if (!paging_null_entry(entry)) {
            struct paging_descriptor_entry *child_entry = (struct paging_descriptor_entry *)((uint64_t)(entry->address) << 12);
            if (child_entry) {
                paging_descriptor_entry_free(child_entry, level - 1);
            }
        }
    }

    kfree(descriptor->pml);
    kfree(descriptor);
}

int paging_map(struct paging_descriptor *desc, void *virt, void *phys, int flags) {
    uintptr_t va = (uintptr_t)virt;
    size_t pml4_index = (va >> 39) & 0x1FF;
    size_t pdpt_index = (va >> 30) & 0x1FF;
    size_t pd_index = (va >> 21) & 0x1FF;
    size_t pt_index = (va >> 12) & 0x1FF;

    struct paging_descriptor_entry *pml4_entry = &desc->pml->entries[pml4_index];
    if (paging_null_entry(pml4_entry)) {
        void *new_pdpt = kzalloc(sizeof(struct paging_descriptor_entry) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        pml4_entry->address = ((uintptr_t)new_pdpt) >> 12;
        pml4_entry->present = 1;
        pml4_entry->read_write = 1;
        pml4_entry->user_supervisor = 1;
    }

    struct paging_descriptor_entry *pdpt_entries = (struct paging_descriptor_entry *)((uintptr_t)(pml4_entry->address) << 12);

    struct paging_descriptor_entry *pdpt_entry = &pdpt_entries[pdpt_index];
    if (paging_null_entry(pdpt_entry)) {
        void *new_pd = kzalloc(sizeof(struct paging_descriptor_entry) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        pdpt_entry->address = ((uintptr_t)new_pd) >> 12;
        pdpt_entry->present = 1;
        pdpt_entry->read_write = 1;
        pdpt_entry->user_supervisor = 1;
    }

    struct paging_descriptor_entry *pd_entries = (struct paging_descriptor_entry *)((uintptr_t)(pdpt_entry->address) << 12);

    struct paging_descriptor_entry *pd_entry = &pd_entries[pd_index];
    if (paging_null_entry(pd_entry)) {
        void *new_pt = kzalloc(sizeof(struct paging_descriptor_entry) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        pd_entry->address = ((uintptr_t)new_pt) >> 12;
        pd_entry->present = 1;
        pd_entry->read_write = 1;
        pd_entry->user_supervisor = 1;
    }

    struct paging_descriptor_entry *pt_entries = (struct paging_descriptor_entry *)((uintptr_t)(pd_entry->address) << 12);

    struct paging_descriptor_entry *pt_entry = &pt_entries[pt_index];
    if (!paging_null_entry(pt_entry)) {
        paging_invalidate_tlb_entry(virt);
    }
    pt_entry->address = ((uintptr_t)phys) >> 12;
    pt_entry->present = (flags & PAGING_IS_PRESENT) ? 1 : 0;
    pt_entry->read_write = (flags & PAGING_IS_WRITEABLE) ? 1 : 0;
    pt_entry->user_supervisor = (flags & PAGING_ACCESS_FROM_ALL) ? 1 : 0;

    return 0;
}

int paging_map_e820_memory_regions(struct paging_descriptor *desc) {
    paging_map_to(desc, (void *)0x00, (void *)0x00, (void *)0x100000, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT);

    size_t total_entries = e820_total_entries();
    for (size_t i = 0; i < total_entries; i++) {
        struct e820_entry *entry = e820_entry(i);
        if (entry->type == 1) {
            void *base_addr = (void *)entry->base_addr;
            void *end_addr = (void *)entry->base_addr + entry->length;

            if (!paging_is_aligned(base_addr)) {
                base_addr = paging_align_address(base_addr);
            }

            if (!paging_is_aligned(end_addr)) {
                end_addr = paging_align_to_lower_page(end_addr);
            }

            paging_map_to(desc, base_addr, base_addr, end_addr, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT);
        }
    }

    return 0;
}

int paging_map_range(struct paging_descriptor *desc, void *virt, void *phys, size_t count, int flags) {
    for (int i = 0; i < count; i++) {        
        paging_map(desc, virt, phys, flags);
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }
    
    return 0;
}

int paging_map_to(struct paging_descriptor *desc, void *virt, void *phys, void *phys_end, int flags) {
    int res = 0;

    if ((uintptr_t)virt % PAGE_SIZE) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    if ((uintptr_t)phys % PAGE_SIZE) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    if ((uintptr_t)phys_end % PAGE_SIZE) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    if ((uintptr_t)phys_end < (uintptr_t)phys) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    uint64_t total_bytes = phys_end - phys;
    size_t total_pages = total_bytes / PAGE_SIZE;
    res = paging_map_range(desc, virt, phys, total_pages, flags);

out:
    return res;
}

struct paging_descriptor_entry *paging_get(struct paging_descriptor *desc, void *virt) {
    uint64_t va = (uint64_t)virt;
    size_t pml4_index = (va >> 39) & 0x1FF;
    size_t pdpt_index = (va >> 30) & 0x1FF;
    size_t pd_index = (va >> 21) & 0x1FF;
    size_t pt_index = (va >> 12) & 0x1FF;

    struct paging_descriptor_entry *pml4_entry = &desc->pml->entries[pml4_index];
    if (paging_null_entry(pml4_entry)) {
        return NULL;
    }

    struct paging_descriptor_entry *pdpt_entries = (struct paging_descriptor_entry *)((uint64_t)(pml4_entry->address) << 12);
    if (paging_null_entry(pdpt_entries)) {
        return NULL;
    }

    struct paging_descriptor_entry *pdpt_entry = &pdpt_entries[pdpt_index];
    if (paging_null_entry(pdpt_entries)) {
        return NULL;
    }

    struct paging_descriptor_entry *pd_entries = (struct paging_descriptor_entry *)((uint64_t)(pdpt_entry->address) << 12);
    if (paging_null_entry(pd_entries)) {
        return NULL;
    }

    struct paging_descriptor_entry *pd_entry = &pd_entries[pd_index];
    if (paging_null_entry(pd_entry)) {
        return NULL;
    }

    struct paging_descriptor_entry *pt_entries = (struct paging_descriptor_entry *)((uint64_t)(pd_entry->address) << 12);
    if (paging_null_entry(pt_entries)) {
        return NULL;
    }

    struct paging_descriptor_entry *pt_entry = &pt_entries[pt_index];

    return pt_entry;
}

void *paging_get_physical_address(struct paging_descriptor *desc, void *vaddr) {
    struct paging_descriptor_entry *desc_entry = paging_get(desc, vaddr);
    if (!desc_entry) {
        return NULL;
    }

    uint64_t phys_base_addr = ((uint64_t)desc_entry->address) << 12;
    uint64_t offset = ((uint64_t)vaddr) & 0xFFF;

    uint64_t full_addr = phys_base_addr + offset;

    return (void *)full_addr;
}