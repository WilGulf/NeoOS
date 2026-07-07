#include "idt.h"
#include "../drivers/io/io.h"
#include "../include/util.h"

struct idt_entry idt_entries[256];
struct idt idt_ptr;

int idt_init() {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

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

    set_idt_gate(0, (uint32_t)interrupt_handler_0, 0x08, 0x8E);
    set_idt_gate(1, (uint32_t)interrupt_handler_1, 0x08, 0x8E);
    set_idt_gate(2, (uint32_t)interrupt_handler_2, 0x08, 0x8E);
    set_idt_gate(3, (uint32_t)interrupt_handler_3, 0x08, 0x8E);
    set_idt_gate(4, (uint32_t)interrupt_handler_4, 0x08, 0x8E);
    set_idt_gate(5, (uint32_t)interrupt_handler_5, 0x08, 0x8E);
    set_idt_gate(6, (uint32_t)interrupt_handler_6, 0x08, 0x8E);
    set_idt_gate(7, (uint32_t)interrupt_handler_7, 0x08, 0x8E);
    set_idt_gate(8, (uint32_t)interrupt_handler_8, 0x08, 0x8E);
    set_idt_gate(9, (uint32_t)interrupt_handler_9, 0x08, 0x8E);
    set_idt_gate(10, (uint32_t)interrupt_handler_10, 0x08, 0x8E);
    set_idt_gate(11, (uint32_t)interrupt_handler_11, 0x08, 0x8E);
    set_idt_gate(12, (uint32_t)interrupt_handler_12, 0x08, 0x8E);
    set_idt_gate(13, (uint32_t)interrupt_handler_13, 0x08, 0x8E);
    set_idt_gate(14, (uint32_t)interrupt_handler_14, 0x08, 0x8E);
    set_idt_gate(15, (uint32_t)interrupt_handler_15, 0x08, 0x8E);
    set_idt_gate(16, (uint32_t)interrupt_handler_16, 0x08, 0x8E);
    set_idt_gate(17, (uint32_t)interrupt_handler_17, 0x08, 0x8E);
    set_idt_gate(18, (uint32_t)interrupt_handler_18, 0x08, 0x8E);
    set_idt_gate(19, (uint32_t)interrupt_handler_19, 0x08, 0x8E);
    set_idt_gate(20, (uint32_t)interrupt_handler_20, 0x08, 0x8E);
    set_idt_gate(21, (uint32_t)interrupt_handler_21, 0x08, 0x8E);
    set_idt_gate(22, (uint32_t)interrupt_handler_22, 0x08, 0x8E);
    set_idt_gate(23, (uint32_t)interrupt_handler_23, 0x08, 0x8E);
    set_idt_gate(24, (uint32_t)interrupt_handler_24, 0x08, 0x8E);
    set_idt_gate(25, (uint32_t)interrupt_handler_25, 0x08, 0x8E);
    set_idt_gate(26, (uint32_t)interrupt_handler_26, 0x08, 0x8E);
    set_idt_gate(27, (uint32_t)interrupt_handler_27, 0x08, 0x8E);
    set_idt_gate(28, (uint32_t)interrupt_handler_28, 0x08, 0x8E);
    set_idt_gate(29, (uint32_t)interrupt_handler_29, 0x08, 0x8E);
    set_idt_gate(30, (uint32_t)interrupt_handler_30, 0x08, 0x8E);
    set_idt_gate(31, (uint32_t)interrupt_handler_31, 0x08, 0x8E);

    set_idt_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    set_idt_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    set_idt_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    set_idt_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    set_idt_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    set_idt_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    set_idt_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    set_idt_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    set_idt_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    set_idt_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    set_idt_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    set_idt_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    set_idt_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    set_idt_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    set_idt_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    set_idt_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    set_idt_gate(128, (uint32_t)interrupt_handler_128, 0x08, 0xEE);
    set_idt_gate(177, (uint32_t)interrupt_handler_177, 0x08, 0x8E);

    load_idt((uint32_t)&idt_ptr);

    kprintf("IDT Initialized\n");

    return 0;
}

void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {

    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;

}

/*unsigned */char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void interrupt_handler(struct interrupt_registers* regs) {
    if (regs->int_no < 32) {
        //char* msg = (char*)exception_messages[regs->int_no];
        kprintf(exception_messages[regs->int_no]);
        kprintf("\n");
        kprintf("Exception! System Halted\n");
        for (;;);
    }
}

void *irq_routines[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

void irq_install_handler(int irq, void( *handler)(struct interrupt_registers *r)) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

void irq_handler(struct interrupt_registers *regs) {
    void (*handler)(struct interrupt_registers *regs);
    handler = irq_routines[regs->int_no - 32];

    if (handler) {
        (handler)(regs);
    }

    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}