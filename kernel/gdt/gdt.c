#include "gdt.h"

#include "../panic.h"
#include "../include/stdint.h"
#include "../include/util.h"

void gdt_set(
    struct gdt_entry *entry,
    void *addr,
    uint16_t limit,
    uint8_t access,
    uint8_t flags
) {
    if ((uintptr_t)addr > 0xFFFFFFFF) {
        panic("", 1);
    }

    uint32_t base = (uint32_t)(uintptr_t)addr;
    entry->base_first = base & 0xFFFF;
    entry->base = (base >> 16) & 0xFF;
    entry->base_last = (base >> 24) & 0xFF;

    entry->segment = limit;
    entry->access = access;
    entry->flags = flags;
}

void gdt_set_tss(
    struct tss_descriptor *desc,
    void *tss_addr,
    uint16_t limit,
    uint8_t type,
    uint8_t flags
) {
    memset(desc, 0, sizeof(struct tss_descriptor));
    desc->limit0 = limit & 0xFFFF;
    desc->limit1_flags = (uint8_t)((limit >> 16) & 0x0F);
    
    uint64_t base = (uint64_t)tss_addr;
    desc->base0 = (uint16_t)(base & 0xFFFF);
    desc->base1 = (uint8_t)((base >> 16) & 0xFF);
    desc->base2 = (uint8_t)((base >> 24) & 0xFF);
    desc->base3 = (uint32_t)((base >> 32) & 0xFFFFFFFF);

    desc->type = type;

    desc->reserved = 0;
}