#ifndef MEMORY_H
#define MEMORY_H

#include "../include/stdint.h"

#define MEMORY_MAP_TOTAL_ENTRIES_LOCATION 0x210000
#define MEMORY_MAP_LOCATION 0x210008

struct e820_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t ext_attr;
} __attribute__((packed));

struct e820_entry *e820_entry(size_t index);
size_t e820_total_entries();
struct e820_entry *e820_entry(size_t index);
struct e820_entry *e820_largest_free_entry();
size_t e820_total_accessible_memory();

#endif