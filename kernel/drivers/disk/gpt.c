#include "gpt.h"
#include "disk.h"
#include "disk_streamer.h"

#include "../../include/status.h"
#include "../../include/util.h"

struct disk *gpt_primary_disk = NULL;

size_t gpt_partition_table_header_real_size(struct gpt_partition_table_header *header) {
    return sizeof(*header) + (header->hdr_size - offsetof(struct gpt_partition_table_header, reserved2));
}

int gpt_partition_table_header_read(struct gpt_partition_table_header *header_out) {
    int res = 0;
    char sector[gpt_primary_disk->sector_size];
    res = disk_read_block(gpt_primary_disk, GPT_PARTITION_TABLE_HEADER_LBA, 1, sector);
    if (res < 0) {
        goto out;
    }

    memcpy(header_out, sector, sizeof(*header_out));
out:
    return res;
}

int gpt_mount_partitions(struct gpt_partition_table_header *partition_header) {
    int res = 0;
    size_t total_entries = partition_header->total_array_entries;
    uint64_t starting_lba = partition_header->guid_array_lba_start;
    uint64_t starting_byte = starting_lba * gpt_primary_disk->sector_size;
    size_t entry_size = partition_header->array_entry_size;
    struct disk_stream *streamer = disk_streamer_new(gpt_primary_disk->id);
    if (!streamer) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    res = disk_streamer_seek(streamer, (int)starting_byte);
    if (res < 0) {
        goto out;
    }

    for (size_t i = 0; i < total_entries; i++) {
        char buffer[entry_size];
        res = disk_streamer_read(streamer, buffer, sizeof(buffer));
        if (res < 0) {
            goto out;
        }

        struct gpt_partition_entry *entry = (struct gpt_partition_entry *)buffer;
        char guid_empty[16] = {0};
        if (memcmp(entry->guid, guid_empty, sizeof(entry->guid)) == 0) {
            continue;
        }

        res = disk_new(0, DISK_TYPE_PARTITION, entry->starting_lba, entry->ending_lba, gpt_primary_disk->sector_size, NULL, gpt_primary_disk->ata_drive_port, gpt_primary_disk->ata_base_port);
        if (res < 0) {
            goto out;
        }


    }
out:
    return res;
}

int gpt_init(void) {
    int res = 0;
    struct gpt_partition_table_header partition_header = {0};
    gpt_primary_disk = disk_get('0');

    if (!gpt_primary_disk) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    res = gpt_partition_table_header_read(&partition_header);
    if (res < 0) {
        goto out;
    }

    if (memcmp(partition_header.signature, GPT_SIGNATURE, sizeof(partition_header.signature)) != 0) {
        res = -ERROR_INVALID_FORMAT;
        goto out;
    }

    res = gpt_mount_partitions(&partition_header);
out:
    return res;
}