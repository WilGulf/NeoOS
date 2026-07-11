#include "gdt.h"
#include "../drivers/io/io.h"
#include "../include/stdint.h"
#include "../include/util.h"

struct gdt gdt_ptr;
struct gdt_entry_struct gdt_entries[6];

struct tss_entry_struct tss_entry;

int gdt_init() {
    gdt_ptr.base = (uint32_t)(void*)&gdt_entries;
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;

    set_gdt_gate(0, 0, 0, 0, 0);
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    write_tss(5, 0x10, 0x0);

    load_gdt(&gdt_ptr);
    flush_tss();

    kprintf("GDT Initialized\n");

    return 0;
}

void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    
    gdt_entries[num].limit = (limit & 0xFFFF);
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    gdt_entries[num].flags |= (gran & 0xF0);

    gdt_entries[num].access = access;
}

void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    set_gdt_gate(num, base, limit, 0xE9, 0x0);
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;

    tss_entry.cs = 0x08 | 0x3;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}

void set_tss_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}