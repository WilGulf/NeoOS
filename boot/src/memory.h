#ifndef MEMORY_H
#define MEMORY_H

#include <efi.h>
#include <efilib.h>

#define MEMORY_MAP_TOTAL_ENTRIES_LOCATION 0x210000
#define MEMORY_MAP_LOCATION 0x210008

typedef struct __attribute__((packed)) e820_entry_t {
    UINT64 base_addr;
    UINT64 length;
    UINT32 type;
    UINT32 extended_attr;
} e820_entry_t;

typedef struct __attribute__((packed)) e820_entries_t {
    UINT64 count;
    e820_entry_t entries[];
} e820_entries_t;

EFI_STATUS setup_memory_maps(void);

#endif