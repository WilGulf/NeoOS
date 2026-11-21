#ifndef GPT_H
#define GPT_H

#include "../../include/stdint.h"

#define GPT_MBR_LBA 0
#define GPT_PARTITION_TABLE_HEADER_LBA 1
#define GPT_SIGNATURE "EFI PART"

struct gpt_partition_table_header {
    char signature[8];

    uint32_t revision;
    uint32_t hdr_size;
    uint32_t cr32_checksum;
    uint32_t reserved1;
    uint64_t lba;
    uint64_t lba_alternate;
    uint64_t data_block_lba;
    uint64_t data_block_lba_end;
    
    char guid[16];
    uint64_t guid_array_lba_start;
    uint32_t total_array_entries;
    uint32_t array_entry_size;
    uint32_t crc32_entry_array;

    char reserved2[];
};

struct gpt_partition_entry {
    char guid[16];
    char unique_partition_guid[16];
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    char partition_name[72];
};

int gpt_init(void);

#endif