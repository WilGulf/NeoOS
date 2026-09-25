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

#ifndef KHEAP_H
#define KHEAP_H

#include "../include/stdint.h"

#define KHEAP_SIZE_BYTES 104857600
#define KHEAP_BLOCK_SIZE 4096
#define KHEAP_ADDRESS 0x01000000
#define KHEAP_TABLE_ADDRESS 0x00007E00

void kheap_init();
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void kfree(void *ptr);

#endif