#include "disk_streamer.h"
#include "../../include/util.h"
#include "../../include/status.h"
#include "../../memory/kheap.h"
#include "../io/io.h"

struct disk_stream *disk_streamer_new(char disk_id) {
    struct disk_stream *streamer = kmalloc(sizeof(struct disk_stream));
    if (!streamer) {
        return -ERROR_NO_MEM;
    }
    memset(streamer, 0, sizeof(struct disk_stream));
    streamer->disk_id = disk_id;
    return streamer;
}

int disk_streamer_seek(struct disk_stream *stream, int pos) {
    stream->pos = pos;
    return 0;
}

int disk_streamer_read(struct disk_stream *stream, void *out, int total) {
    char *out_ptr = out;

    int start_sector = stream->pos / SECTOR_SIZE;
    struct disk *disk = disk_get(stream->disk_id);
    if (!disk) {
        return -ERROR_IO;
    }

    int offset_in_sector = stream->pos % SECTOR_SIZE;
    char buf[SECTOR_SIZE];
    if (disk_read_block(disk, start_sector, 1, buf) < 0) {
        return -ERROR_IO;
    }

    int to_read = SECTOR_SIZE - offset_in_sector;
    if (total < SECTOR_SIZE - offset_in_sector) {
        to_read = total;
    }
    memcpy(out_ptr, buf + offset_in_sector, to_read);
    
    stream->pos += to_read;
    out_ptr += to_read;
    int new_total = total - to_read;
    if (new_total > 0) {
        disk_streamer_read(stream, out_ptr, total - to_read);
    }

    return 0;
}

int disk_streamer_write(struct disk_stream *stream, void *in, int total) {
    struct disk *disk = disk_get(stream->disk_id);
    if (!disk) {
        return -ERROR_IO;
    }

    int start_sector = stream->pos / SECTOR_SIZE;
    int offset_in_sector = stream->pos % SECTOR_SIZE;
    int total_to_write = total > SECTOR_SIZE ? SECTOR_SIZE : total;
    
    kbool overflow = (offset_in_sector + total_to_write) > SECTOR_SIZE;
    char buf[SECTOR_SIZE];

    if (overflow) {
        total_to_write -= (offset_in_sector + total_to_write) - SECTOR_SIZE;
    }

    int res = disk_read_block(disk, start_sector, 1, buf);
    if (res < 0) {
        goto out;
    }

    memcpy(buf + offset_in_sector, in, total_to_write);
    disk_write_block(disk, start_sector, 1, buf);
    if (res < 0) {
        goto out;
    }

    stream->pos += total_to_write;
    if (overflow) {
        res = disk_streamer_write(stream, (void *)in + total_to_write, total - total_to_write);
    }

out:
    return res;
}

int disk_stream_close(struct disk_stream *stream) {
    kfree(stream);
    return 0;
}