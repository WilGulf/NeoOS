#include "gdt.h"
#include "../drivers/io/io.h"
#include "../include/stdint.h"

struct gdt gdt_ptr;
struct gdt_entry gdt_entries[3];

int gdt_init() {
    gdt_ptr.base = (unsigned int)(void*)&gdt_entries;
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;

    set_gdt_gate(0, 0, 0, 0, 0);
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    load_gdt(&gdt_ptr);

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