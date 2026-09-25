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

#include "heap.h"
#include "promise.h"

#include "../task/task.h"
#include "../task/process.h"
#include "../include/stdint.h"

void *isr80h_command9_malloc(struct interrupt_frame *frame) {
    size_t size = (size_t)task_get_stack_item(task_current(), 0);
    return process_malloc(task_current()->process, size);
}

void *isr80h_command10_free(struct interrupt_frame *frame) {    
    void *ptr = task_get_stack_item(task_current(), 0);
    process_free(task_current()->process, ptr);
    return 0;
}