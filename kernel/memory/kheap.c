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

#include "kheap.h"
#include "heap.h"

#include "../drivers/io/io.h"
#include "../include/stdint.h"
#include "../include/util.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init() {
    int total_table_entries = KHEAP_SIZE_BYTES / KHEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY *)(KHEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void *end = (void *)(KHEAP_ADDRESS + KHEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *)(KHEAP_ADDRESS), end, &kernel_heap_table);
    if (res < 0) {
        kprintf("Failed to create kheap\n");
    }
}

void *kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void *kzalloc(size_t size) {
    void *ptr = kmalloc(size);
    if (!ptr) {
        return 0;
    }

    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void *ptr) {
    heap_free(&kernel_heap, ptr);
}