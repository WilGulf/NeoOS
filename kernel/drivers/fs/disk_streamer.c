#include "disk_streamer.h"
#include "../../include/util.h"
#include "../../include/status.h"
#include "../../memory/kheap.h"
#include "../io/io.h"

struct disk_stream *disk_streamer_new(char disk_id) {
    struct disk *disk = disk_get(disk_id);
    if (!disk) {
        return 0;
    }

    struct disk_stream *streamer = kzalloc(sizeof(struct disk_stream));
    if (!streamer) {
        return -ERROR_NO_MEM;
    }

    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int disk_streamer_seek(struct disk_stream *stream, int pos) {
    stream->pos = pos;
    return 0;
}

int disk_streamer_read(struct disk_stream *stream, void *out, int total) {
    int sector = stream->pos / SECTOR_SIZE;
    int offset = stream->pos % SECTOR_SIZE;
    int total_to_read = total > SECTOR_SIZE ? SECTOR_SIZE : total;
    
    kbool overflow = (offset + total_to_read) > SECTOR_SIZE;
    char buf[SECTOR_SIZE];

    if (overflow) {
        total_to_read -= (offset + total_to_read) - SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0) {
        goto out;
    }

    for (int i = 0; i < total_to_read; i++) {
        *(char *)out++ = buf[offset + i];
    }

    stream->pos += total_to_read;
    if (overflow) {
        res = disk_streamer_read(stream, out, total - total_to_read);
    }

out:
    return res;
}

int disk_streamer_write(struct disk_stream *stream, void *in, int total) {
    int sector = stream->pos / SECTOR_SIZE;
    int offset = stream->pos % SECTOR_SIZE;
    int total_to_write = total > SECTOR_SIZE ? SECTOR_SIZE : total;
    
    kbool overflow = (offset + total_to_write) > SECTOR_SIZE;
    char buf[SECTOR_SIZE];

    if (overflow) {
        total_to_write -= (offset + total_to_write) - SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0) {
        goto out;
    }

    memcpy(buf + offset, in, total_to_write);
    disk_write_block(stream->disk, sector, 1, buf);
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

void disk_stream_close(struct disk_stream *stream) {
    kfree(stream);
}