#ifndef GDT_H
#define GDT_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct gdt_entry {
    unsigned short limit;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char flags;
    unsigned char base_high;

} __attribute__((packed));

struct gdt {
    uint16_t limit;
    unsigned int base;
} __attribute__((packed));

int gdt_init();

void load_gdt(struct gdt * address);

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif
