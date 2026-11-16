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

#ifndef TASK_H
#define TASK_H

#include "process.h"

#include "../gdt/gdt.h"
#include "../memory/paging.h"

#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define USER_PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END PROGRAM_VIRTUAL_STACK_ADDRESS_START - USER_PROGRAM_STACK_SIZE 

#define USER_DATA_SEGMENT 0x33
#define USER_CODE_SEGMENT 0x2B

struct interrupt_frame;

struct registers {
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
};

struct process;

struct task {
    struct paging_descriptor *paging_desc;
    struct registers registers;
    struct process *process;
    struct task *next;
    struct task *prev;
    struct task *parent;
};

struct task *task_new(struct process *process);
void task_list_remove(struct task *task);
struct task *task_current();
struct task *task_get_next();
void task_next();
int task_free(struct task *task);

int task_switch(struct task *task);
int task_page();
int task_page_task(struct task *task);

void task_run_first_ever_task();

void task_return(struct registers *regs);
void restore_general_purpose_registers(struct registers *regs);
void user_registers();

void task_current_save_state(struct interrupt_frame *frame);
int copy_string_from_task(struct task *task, void *virt, void *phys, int max);
void *task_get_stack_item(struct task *task, int index);

void *task_virtual_address_to_physical(struct task *task, void *vaddr);

void task_add(struct task *task);

#endif