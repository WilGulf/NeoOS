#ifndef GDT_H
#define GDT_H

#include "../include/stdint.h"

struct gdt_entry {
    uint16_t segment;
    uint16_t base_first;
    uint8_t base;
    uint8_t access;
    uint8_t flags;
    uint8_t base_last;

} __attribute__((packed));

struct tss_descriptor {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t type;
    uint8_t limit1_flags;
    uint8_t base2;

    uint32_t base3;
    uint32_t reserved;
};

struct tss {
    uint16_t link;
    uint16_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;

    uint64_t reserved1;
    
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;

    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;

} __attribute__((packed));

void gdt_set(struct gdt_entry *entry, void *addr, uint16_t limit, uint8_t access, uint8_t flags);
void gdt_set_tss(struct tss_descriptor *desc, void *tss_addr, uint16_t limit, uint8_t type, uint8_t flags);
void tss_load(int tss_segment);

#endif
