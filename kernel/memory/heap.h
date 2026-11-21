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

#ifndef HEAP_H
#define HEAP_H

#include "../include/stdint.h"

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0B10000000
#define HEAP_BLOCK_IS_FIRST 0B01000000

#define HEAP_BLOCK_SIZE 0x1000

#define MINIMAL_HEAP_ADDRESS 0x01100000
#define MINIMAL_HEAP_TABLE_ADDRESS 0x01000000
#define MINIMAL_HEAP_TABLE_SIZE MINIMAL_HEAP_ADDRESS - MINIMAL_HEAP_TABLE_ADDRESS

#define HEAP_MINIMUM_SIZE_BYTES 104857600

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;
typedef void *(*HEAP_BLOCK_ALLOCATED_CALLBACK_FUNCTION)(void *ptr, size_t size);
typedef void (*HEAP_BLOCK_FREE_CALLBACK_FUNCTION)(void *ptr);

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY *entries;
    size_t total;
};

struct heap {
    struct heap_table *table;
    void *saddr;
    void *eaddr;
    size_t total_blocks;
    size_t free_blocks;
    size_t used_blocks;
    HEAP_BLOCK_ALLOCATED_CALLBACK_FUNCTION block_allocated_callback;
    HEAP_BLOCK_FREE_CALLBACK_FUNCTION block_free_callback;
};

uintptr_t heap_align_value_to_upper(uintptr_t val);
uintptr_t heap_align_value_to_lower(uintptr_t val);

kbool heap_is_address_within_heap(struct heap *heap, void *addr);
int64_t heap_address_to_block(struct heap *heap, void *address);

void heap_callbacks_set(struct heap *heap, HEAP_BLOCK_ALLOCATED_CALLBACK_FUNCTION allocated_function, HEAP_BLOCK_FREE_CALLBACK_FUNCTION free_function);

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table);
void *heap_malloc(struct heap *heap, size_t size);
void heap_free(struct heap *heap, void *ptr);
void *heap_zalloc(struct heap *heap, size_t size);

size_t heap_total_size(struct heap *heap);
size_t heap_total_used(struct heap *heap);
size_t heap_total_available(struct heap *heap);

size_t heap_allocation_block_count(struct heap *heap, void *saddr);

#endif