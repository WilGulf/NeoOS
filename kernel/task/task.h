#ifndef TASK_H
#define TASK_H

#include "process.h"

#include "../gdt/gdt.h"
#include "../memory/paging.h"

#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define USER_PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END PROGRAM_VIRTUAL_STACK_ADDRESS_START - USER_PROGRAM_STACK_SIZE 

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1B

struct interrupt_frame;

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;

struct task {
    struct paging_4gb_chunk *page_directory;
    struct registers registers;
    struct process *process;
    struct task *next;
    struct task *prev;
};

struct task *task_new(struct process *process);
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

#endif