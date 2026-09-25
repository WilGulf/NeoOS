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

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY *entries;
    size_t total;
};

struct heap {
    struct heap_table *table;
    void *saddr;
};

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table);
void *heap_malloc(struct heap *heap, size_t size);
void heap_free(struct heap *heap, void *ptr);

#endif