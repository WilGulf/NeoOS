#include "idt.h"

#include "../kernel.h"
#include "../include/util.h"
#include "../include/status.h"
#include "../panic.h"
#include "../task/task.h"
#include "../task/process.h"
#include "../drivers/io/io.h"

struct idt_desc idt_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void *interrupt_pointer_table[TOTAL_INTERRUPTS];

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[MAX_ISR80H_COMMANDS];

extern void idt_load(struct idtr_desc *ptr);
extern void no_interrupt();
extern void isr80h_wrapper();

void no_interrupt_handler() {
    outb(0x20, 0x20);
}

int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback) {
    if (interrupt < 0 || interrupt >= TOTAL_INTERRUPTS) {
        return -ERROR_INVALID_ARG;
    }

    interrupt_callbacks[interrupt] = interrupt_callback;
    return 0;
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command) {
    if (command_id < 0 || command_id >= MAX_ISR80H_COMMANDS) {
        panic("Trying to register a command out of bounds\n", 0);
    }

    if (isr80h_commands[command_id]) {
        panic("Trying to register a command already registered\n", 0);
    }

    isr80h_commands[command_id] = command;
}

void *isr80h_handle_command(int command, struct interrupt_frame *frame) {
    void *result = 0;
    if (command < 0 || command >= MAX_ISR80H_COMMANDS) {
        return 0;  
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func) {
        return 0;
    }

    result = command_func(frame);

    return result;
}

void *isr80h_handler(int command, struct interrupt_frame *frame) {
    void *res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}

void interrupt_handler(int interrupt, struct interrupt_frame *frame) {
    kernel_page();
    if (interrupt_callbacks[interrupt] != 0) {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt]();
    }

    task_page();
    outb(0x20, 0x20);
}

void idt_set(int interrupt_no, void *address) {
    struct idt_desc *desc = &idt_descriptors[interrupt_no];
    desc->offset_lower = (uint32_t)address & 0x0000FFFF;
    desc->selector = 0x08;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_upper = (uint32_t)address >> 16;
}

void idt_handle_exception() {
    process_terminate(task_current()->process);
    kprintf("IDT\n");
    task_next();
}

void idt_clock() {
    outb(0x20, 0x20);
    task_next();
}

void idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = idt_descriptors;

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    
    for (int i = 0; i < TOTAL_INTERRUPTS; i++) {
        idt_set(i, interrupt_pointer_table[i]);
    }

    for (int i = 0; i < 0x20; i++) {
        idt_register_interrupt_callback(i, idt_handle_exception);
    }

    idt_register_interrupt_callback(0x20, idt_clock);

    idt_set(0x80, isr80h_wrapper);

    idt_load(&idtr_descriptor);
}