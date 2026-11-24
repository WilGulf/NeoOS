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

#include "task.h"

#include "../include/util.h"
#include "../include/status.h"
#include "../panic.h"
#include "../memory/paging.h"
#include "../memory/kheap.h"
#include "../idt/idt.h"
#include "formats/elf_loader.h"
#include "../kernel.h"

kbool tasker_running = false;

struct task *current_task = 0;
struct task *task_tail = 0;
struct task *task_head = 0;

struct task *task_current() {
    return current_task;
}

struct task *task_get_next() {
    if (!current_task->next) {
        return task_head;
    }

    return current_task->next;
}

void task_list_remove(struct task *task) {
    if (!task) {
        return;
    }
    
    if (task->prev) {
        task->prev->next = task->next;
    }
    if (task->next) {
        task->next->prev = task->prev;
    }

    if (task == task_head) {
        task_head = task->next;
    }

    if (task == task_tail) {
        task_tail = task->prev;
    }

    task->next = 0;
    task->prev = 0;
}

int task_free(struct task *task) {
    if (task->parent != 0) {
        task_add(task->parent);
    }

    paging_descriptor_free(task->paging_desc);
    task_list_remove(task);
    kfree(task);

    return 0;
}

int task_switch(struct task *task) {
    current_task = task;
    paging_switch(task->paging_desc);
    return 0;
}

int copy_string_from_task(struct task *task, void *virt, void *phys, int max) {
    if (max >= PAGE_SIZE) {
        return -ERROR_INVALID_ARG;
    }

    int res = 0;
    char *tmp = kzalloc(max);
    if (!tmp) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    void *phys_tmp = paging_get_physical_address(kernel_descriptor(), tmp);
    struct paging_descriptor *task_desc = task->paging_desc;
    struct paging_descriptor_entry old_entry;
    memcpy(&old_entry, paging_get(task_desc, phys_tmp), sizeof(struct paging_descriptor_entry));

    int old_entry_flags = 0;
    old_entry_flags |= old_entry.read_write | old_entry.present | old_entry.user_supervisor;
    
    paging_map(task_desc, phys_tmp, phys_tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    task_page_task(task);

    strncpy(tmp, virt, max);

    kernel_page();

    strncpy(phys, tmp, max);

    paging_map(task_desc, phys_tmp, (void *)(uint64_t)(old_entry.address << 12), old_entry_flags);
out:
    if (tmp) {
        kfree(tmp);
    }

    return res;
}

void task_save_state(struct task *task, struct interrupt_frame *frame) {
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.rsp = frame->rsp;
    task->registers.ss = frame->ss;
    task->registers.rax = frame->rax;
    task->registers.rbp = frame->rbp;
    task->registers.rbx = frame->rbx;
    task->registers.rcx = frame->rcx;
    task->registers.rdi = frame->rdi;
    task->registers.rdx = frame->rdx;
    task->registers.rsi = frame->rsi;
}

void task_current_save_state(struct interrupt_frame *frame) {
    if (task_current() == 0) {
        return;
        panic("No current task to save", 0);
    }

    struct task *task = task_current();
    task_save_state(task, frame);
}

int task_page() {
    if (!current_task) {
        return 0;
    }
    
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_page_task(struct task *task) {
    user_registers();
    paging_switch(task->paging_desc);
    return 0;
}

int task_init(struct task *task, struct process *process) {
    memset(task, 0, sizeof(struct task));
    task->paging_desc = paging_descriptor_new(PAGING_MAP_LEVEL_4);
    if (!task->paging_desc) {
        return -ERROR_IO;
    }

    paging_map_e820_memory_regions(task->paging_desc);

    task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
    if (process->filetype == PROCESS_FILETYPE_ELF) {
        task->registers.ip = elf_header(process->elf_file)->e_entry;
    }
    
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.rsp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    task->process = process;
    task->parent = 0;

    return 0;
}

void task_next() {
    struct task *next_task = task_get_next();
    if (!next_task && tasker_running) {
        panic("No more tasks\n", 0);
    }
    
    task_switch(next_task);
    task_return(&next_task->registers);
}

void task_run_first_ever_task() {
    tasker_running = true;
    
    if (!current_task) {
        panic(" task_run_first_ever_task(): No current task exists!\n", 0);
    }

    task_switch(task_head);
    task_return(&task_head->registers);

    //enable_interrupts();
}

void task_add(struct task *task) {
    if (task_head == 0) {
        task_head = task;
        task_tail = task;
        current_task = task;
        return;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;
}

struct task *task_new(struct process *process) {
    int res = 0;
    struct task *task = kzalloc(sizeof(struct task));
    if (!task) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != ALL_OK) {
        goto out;
    }

    task_add(task);
out:
    if (ISERR(res)) {
        task_free(task);
        return ERROR(res);
    }

    return task;

}

void *task_get_stack_item(struct task *task, int index) {
    void *result = 0;
    uintptr_t *sp_ptr = (uintptr_t *)task->registers.rsp;
    task_page_task(task);
    
    result = (void *)sp_ptr[index];
    
    kernel_page();
    return result;
}

void *task_virtual_address_to_physical(struct task *task, void *vaddr) {
    return paging_get_physical_address(task->paging_desc, vaddr);
}