#include "memory.h"

#include "../include/stdint.h"

size_t e820_total_entries() {
    return *((uint16_t *)MEMORY_MAP_TOTAL_ENTRIES_LOCATION);
}

struct e820_entry *e820_entry(size_t index) {
    if (index >= e820_total_entries()) {
        return NULL;
    }

    struct e820_entry *entries = (struct e820_entry *)MEMORY_MAP_LOCATION;
    return &entries[index];
}

struct e820_entry *e820_largest_free_entry() {
    size_t total_memory_entries = e820_total_entries();
    struct e820_entry *entries = (struct e820_entry *)MEMORY_MAP_LOCATION;

    struct e820_entry *chosen_entry = NULL;
    for (int i = 0; i< total_memory_entries; i++) {
        struct e820_entry *entry = &entries[i];
        if (entry->type == 1) {
            if (chosen_entry == NULL) {
                chosen_entry = entry;
                continue;
            }

            if (entry->length > chosen_entry->length) {
                chosen_entry = entry;
            }
        }
    }

    return chosen_entry;
}

size_t e820_total_accessible_memory() {
    size_t total_memory_entries = e820_total_entries();
    struct e820_entry *entries = (struct e820_entry *)MEMORY_MAP_LOCATION;
    
    size_t total_memory = 0;
    for (int i = 0; i < total_memory_entries; i++) {
        struct e820_entry *entry = &entries[i];
        if (entry->type == 1) {
            total_memory += entry->length;
        }
    }

    return total_memory;
}