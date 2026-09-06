#include "neofs.h"

#include "../file.h"
#include "../disk_streamer.h"

#include "../../../include/stdint.h"
#include "../../../include/status.h"
#include "../../../include/util.h"
#include "../../../memory/kheap.h"

struct master_block {
    uint32_t magic;

    uint32_t first_block;
    uint32_t block_count;
} __attribute__((packed));

struct meta_block {
    uint8_t status;

    char filename[8];
    char ext[3];

    uint8_t is_dir;
    uint8_t flags;
    uint32_t size;

    uint32_t start;
    uint32_t next;
    uint32_t parent;
} __attribute__((packed));

struct block {
    uint8_t status;
    uint32_t next;
} __attribute__((packed));

#define NEOFS_MAGIC 0x4E454F46

#define MAX_PATH 256

#define BLOCK_DATA_OFFSET sizeof(struct block)
#define BLOCK_SIZE 512
#define DATA_SIZE (BLOCK_SIZE - sizeof(struct block))

#define BLOCK_STATUS_FREE 0x00
#define BLOCK_STATUS_USED 0x01

#define FLAG_R 0b00000001
#define FLAG_W 0b00000010
#define FLAG_X 0b00000100
#define FLAG_4 0b00001000
#define FLAG_5 0b00010000
#define FLAG_6 0b00100000
#define FLAG_7 0b01000000
#define FLAG_8 0b10000000


struct neofs_private {
    struct master_block master_block;
    struct disk_stream *stream;
};

struct neofs_file_descriptor {
    int meta_block;
    FILE_MODE mode;
    FILE_TYPE type;
    uint32_t pos;
};

int neofs_resolve(struct disk *disk);
int neofs_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
void *neofs_open(struct disk *disk, struct path_part *path, FILE_MODE mode);
int neofs_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr);
int neofs_stat(struct disk *disk, void *private, struct file_stat *stat);
int neofs_write(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, const char *in);
int neofs_remove(struct disk *disk, struct path_part *path);
int neofs_close(void *private);
struct dirent neofs_readdir(struct disk *disk, void *private);

struct filesystem neofs = {
    .resolve = neofs_resolve,
    .open = neofs_open,
    .seek = neofs_seek,
    .read = neofs_read,
    .stat = neofs_stat,
    .write = neofs_write,
    .remove = neofs_remove,
    .close = neofs_close,
    .readdir = neofs_readdir,
};

struct filesystem *neofs_init() {
    strcpy(neofs.name, "NEOFS");
    return &neofs;
}

static void neofs_init_private(struct disk *disk, struct neofs_private *private) {
    memset(private, 0, sizeof(struct neofs_private));
    private->stream = disk_streamer_new(disk->id);
}

static int neofs_read_block(struct disk *disk, uint32_t block, uint32_t size, void *out) {
    struct neofs_private *private = disk->fs_private;

    if (disk_streamer_seek(private->stream, block * BLOCK_SIZE) != ALL_OK) {
        return -ERROR_IO;
    }

    return disk_streamer_read(private->stream, out, size);
}

static int neofs_read_block_data(struct disk *disk, uint32_t block, uint32_t size, void *out) {
    struct neofs_private *private = disk->fs_private;

    if (disk_streamer_seek(private->stream, (block * BLOCK_SIZE) + BLOCK_DATA_OFFSET) != ALL_OK) {
        return -ERROR_IO;
    }

    return disk_streamer_read(private->stream, out, size);
}

static int neofs_write_block(struct disk *disk, uint32_t block, uint32_t size, void *in) {
    struct neofs_private *private = disk->fs_private;

    if (disk_streamer_seek(private->stream, block * BLOCK_SIZE) != ALL_OK) {
        return -ERROR_IO;
    }

    int res = disk_streamer_write(private->stream, in, size);

    return res;
}

static int get_master_block(struct master_block *out, struct disk *disk) {
    return neofs_read_block(disk, 0, sizeof(struct master_block), out);
}

static int get_meta_block(int block, struct meta_block *out, struct disk *disk) {
    return neofs_read_block(disk, block, sizeof(struct meta_block), out);
}

static int get_block(int block, struct block *out, struct disk *disk) {
    return neofs_read_block(disk, block, sizeof(struct block), out);
}

static int get_free_block(int start, struct disk *disk) {
    int pos = start;
    struct block block;

    struct neofs_private *private = disk->fs_private;
    uint32_t i = start;
    while (i < private->master_block.block_count) {
        neofs_read_block(disk, pos, sizeof(block), &block);

        if (!block.status) {
            return pos;
        }

        pos++;
        i++;
    }

    return -1;
}

static uint32_t get_next_block(uint32_t block, struct disk *disk) {
    struct block read_block;
    get_block(block, &read_block, disk);
    return read_block.next;
}

static int remove_meta_block(int block, struct disk *disk) {
    struct meta_block empty_block;
    memset(&empty_block, 0, sizeof(empty_block));

    neofs_write_block(disk, block, sizeof(empty_block), &empty_block);
    return 0;
}

static int remove_block(int block, struct disk *disk) {
    struct block empty_block;
    memset(&empty_block, 0, sizeof(empty_block));

    neofs_write_block(disk, block, sizeof(empty_block), &empty_block);
    return 0;
}

int neofs_resolve(struct disk *disk) {
    int res = 0;
    struct neofs_private *private = kzalloc(sizeof(struct neofs_private));
    neofs_init_private(disk, private);

    disk->fs_private = private;
    disk->filesystem = &neofs;

    struct disk_stream *stream = disk_streamer_new(disk->id);
    if (!stream) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    struct master_block master_block;
    get_master_block(&master_block, disk);
    
    if (master_block.magic != NEOFS_MAGIC) {
        res = -ERROR_IO;
        goto out;
    }

    if (!master_block.first_block) {
        res = -ERROR_IO;
        goto out;
    }

    private->master_block = master_block;

out:
    if (stream) {
        disk_stream_close(stream);
    }

    if (res < 0) {
        kfree(private);
        disk->fs_private = 0;
    }

    return res;
}

static int create_meta_block(int parent, kbool is_dir, const char *name, struct disk *disk) {
    int new_block_n = get_free_block(1, disk);
    if (new_block_n < 0) {
        return -1;
    }

    struct meta_block new_block;
    memset(new_block.filename, 0, sizeof(new_block.filename));
    memset(new_block.ext, 0, sizeof(new_block.ext));

    char buffer[strlen(name) + 1];
    strcpy(buffer, name);
    char *dot = strchr(buffer, '.');
    if (dot != 0) {
        *dot = '\0';

        char *ext = dot + 1;
        size_t ext_len = (strlen(ext) > sizeof(new_block.ext)) ? sizeof(new_block.ext) : strlen(ext);
        strncpy(new_block.ext, ext, ext_len);
    }

    char *filename = buffer;
    size_t filename_len = (strlen(filename) > sizeof(new_block.filename)) ? sizeof(new_block.filename) : strlen(filename);
    strncpy(new_block.filename, filename, filename_len);

    new_block.is_dir = is_dir;
    new_block.next = 0;
    new_block.parent = parent;
    new_block.size = 0;
    new_block.start = 0;
    new_block.flags = FLAG_R | FLAG_W;
    new_block.status = BLOCK_STATUS_USED;

    neofs_write_block(disk, new_block_n, sizeof(new_block), &new_block);

    struct meta_block parent_block;
    get_meta_block(parent, &parent_block, disk);
    
    if (!parent_block.start) {
        parent_block.start = new_block_n;
        neofs_write_block(disk, parent, sizeof(parent_block), &parent_block);
    } else {
        struct meta_block child_block;
        get_meta_block(parent_block.start, &child_block, disk);

        int prev_child_block_n = parent_block.start;
        while (child_block.next) {
            prev_child_block_n = child_block.next;
            get_meta_block(child_block.next, &child_block, disk);
        }

        get_meta_block(prev_child_block_n, &child_block, disk);
        child_block.next = new_block_n;
        neofs_write_block(disk, prev_child_block_n, sizeof(child_block), &child_block);
    }

    return new_block_n;
}

static int name_matches(struct meta_block *meta_block, const char *part) {
    char buffer[strlen(part) + 1];
    strcpy(buffer, part);

    char *dot = strchr(buffer, '.');
    if (dot != 0) {
        *dot = '\0';

        char *filename = buffer;
        char *ext = dot + 1;

        if (strncmp(filename, meta_block->filename, sizeof(meta_block->filename))) {
            return false;
        }
        if (strncmp(ext, meta_block->ext, sizeof(meta_block->ext))) {
            return false;
        }

        return true;
    } else {
        if (strncmp(buffer, meta_block->filename, sizeof(meta_block->filename))) {
            return false;
        }

        return true;
    }
}

static int neofs_get_path_meta_block(struct path_part *path_part, kbool allow_creation, struct disk *disk) {
    int res = 0;

    struct master_block master_block;
    get_master_block(&master_block, disk);
    
    struct meta_block root;
    get_meta_block(master_block.first_block, &root, disk);
    
    struct meta_block curr;
    struct meta_block dir;

    int dir_n = master_block.first_block; // Dir being searched
    int curr_n = root.start; // Inspected file in dir

    get_meta_block(master_block.first_block, &dir, disk);

    if (dir.start) {
        curr_n = dir.start;
        get_meta_block(curr_n, &curr, disk);
    }

    while (path_part->part) {
        kbool found = false;
        while (1) {
            if (!dir.start) {
                break;
            }

            if (name_matches(&curr, path_part->part)) {
                found = true;
                break;
            }

            if (!curr.next) {
                break;
            }

            curr_n = curr.next;
            get_meta_block(curr_n, &curr, disk);
        }

        if (found && path_part->next) {
            if (!curr.is_dir) {
                res = -1;
                goto out;
            }

            dir_n = curr_n; // Jump into new dir
            get_meta_block(dir_n, &dir, disk);

            curr_n = dir.start; // Start next search on first child in new dir
            if (curr_n) {
                get_meta_block(curr_n, &curr, disk);
            }

            path_part = path_part->next; // Go to the next path_part
            continue;
        }

        if (found && !path_part->next) {
            res = curr_n; // File found
            goto out;
        }

        if (!found && path_part->next) {
            if (!allow_creation) {
                res = -1;
                goto out;
            }

            int new_dir = create_meta_block(dir_n, true, path_part->part, disk);
            
            dir_n = new_dir;
            get_meta_block(dir_n, &dir, disk);
            curr_n = dir.start;
            path_part = path_part->next;
            continue;
        }

        if (!found && !path_part->next) {
            if (!allow_creation) {
                res = -1;
                goto out;
            }

            int new_file = create_meta_block(dir_n, false, path_part->part, disk);
            res = new_file;
            goto out;
        }
    }

out:
    return res;
}
   
int neofs_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode) {
    int res = 0;
    struct neofs_file_descriptor *desc = private;

    switch(seek_mode) {
        case SEEK_SET:
            desc->pos = offset;
            break;
        
        case SEEK_END:
            res = -ERROR_UNIMPL;
            break;

        case SEEK_CUR:
            desc->pos += offset;
            break;
        
        default:
            res = -ERROR_INVALID_ARG;
            break;
    }

out:
    return res;
}

void *neofs_open(struct disk *disk, struct path_part *path, FILE_MODE mode) {
    int err_code;
    struct neofs_file_descriptor *descriptor = 0;
    
    int meta_block_n = neofs_get_path_meta_block(path, mode == FILE_MODE_WRITE, disk);
    if (meta_block_n < 0) {
        err_code = -ERROR_IO;
        goto err_out;
    }

    descriptor = kzalloc(sizeof(struct neofs_file_descriptor));
    if (!descriptor) {
        err_code = -ERROR_NO_MEM;
        goto err_out;
    }

    descriptor->meta_block = meta_block_n;
    descriptor->mode = mode;
    descriptor->pos = 0;
    descriptor->type = FILE_TYPE_FILE;

    struct meta_block meta_block;
    get_meta_block(meta_block_n, &meta_block, disk);
    if (meta_block.is_dir) {
        descriptor->type = FILE_TYPE_DIR;
    }

    return descriptor;

err_out:
    if (descriptor) {
        kfree(descriptor);
    }

    return err_code;
}

int neofs_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr) {
    struct neofs_file_descriptor *desc = descriptor;

    if (desc->mode != FILE_MODE_READ) {
        return -ERROR_INVALID_ARG;
    }

    struct meta_block meta_block;
    get_meta_block(desc->meta_block, &meta_block, disk);

    if (!meta_block.flags & FLAG_R) {
        return -ERROR_IO;
    }

    if (desc->pos >= meta_block.size) {
        return -ERROR_INVALID_ARG;
    }

    size_t remaining = meta_block.size - desc->pos;
    uint32_t requested = size * nmemb;
    if (requested > remaining) {
        requested = remaining;
    }

    uint32_t total_to_read = 0;

    int blocks_offset = desc->pos / DATA_SIZE;

    uint32_t block = meta_block.start;
    struct block temp;
    get_block(block, &temp, disk);
    
    int i = 0;
    while (i < blocks_offset) {
        block = temp.next;
        if (!block) {
            return -ERROR_IO;
        }

        get_block(temp.next, &temp, disk);
        i++;
    }

    kbool first = true;
    while (block && requested > 0) {    
        uint32_t offset;
        if (first) {
            offset = desc->pos % DATA_SIZE;
            first = false;
        } else {
            offset = 0;
        }

        uint32_t available = DATA_SIZE - offset;
        uint32_t size = requested < remaining ? requested : remaining;
        size_t to_read = (size > available) ? available : size;
        char buffer[DATA_SIZE];
        int res = neofs_read_block_data(disk, block, DATA_SIZE, buffer);
        if (res < 0) {
            return -ERROR_IO;
        }

        memcpy(out_ptr, buffer + offset, to_read);

        out_ptr += to_read;
        remaining -= to_read;
        requested -= to_read;
        total_to_read += to_read;
        block = get_next_block(block, disk);;
    }

    desc->pos += total_to_read;

    return total_to_read;
}

int neofs_stat(struct disk *disk, void *private, struct file_stat *stat) {
    struct neofs_file_descriptor *desc = private;
    struct meta_block meta_block;
    get_meta_block(desc->meta_block, &meta_block, disk);

    stat->filesize = meta_block.size;
    stat->flags = 0x00;

    return 0;
}

int neofs_write(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, const char *in) {
    uint32_t total = size * nmemb;

    struct neofs_file_descriptor *desc = private;
    if (desc->mode != FILE_MODE_WRITE) {
        return -ERROR_INVALID_ARG;
    }
    
    struct meta_block file_meta_block;
    get_meta_block(desc->meta_block, &file_meta_block, disk);

    if (!file_meta_block.flags & FLAG_W) {
        return -ERROR_IO;
    }


    int block = file_meta_block.start;
    if (!block) {
        int new_block_n = get_free_block(1, disk);
        if (new_block_n < 0) {
            return -1;
        }

        struct block new_block;
        new_block.status = BLOCK_STATUS_USED;
        new_block.next = 0;
        neofs_write_block(disk, new_block_n, sizeof(new_block), &new_block);

        file_meta_block.start = new_block_n;
        neofs_write_block(disk, desc->meta_block, sizeof(file_meta_block), &file_meta_block);
        block = file_meta_block.start;
    }

    int blocks_written = 0;

    int blocks_needed = total / DATA_SIZE;
    if (total % DATA_SIZE) {
        blocks_needed++;
    }

    int curr = block;
    int prev = curr;
    int blocks_allocated = 1;
    while (blocks_allocated < blocks_needed) {
        curr = get_next_block(curr, disk);
        if (!curr) {
            int new_block_n = get_free_block(1, disk);
            if (new_block_n < 0) {
                return -1;
            }

            struct block new_block;
            new_block.status = BLOCK_STATUS_USED;
            new_block.next = 0;
            neofs_write_block(disk, new_block_n, sizeof(new_block), &new_block);

            struct block prev_block;
            get_block(prev, &prev_block, disk);
            prev_block.next = new_block_n;
            curr = new_block_n;
            neofs_write_block(disk, prev, sizeof(prev_block), &prev_block);
        }

        prev = curr;
        blocks_allocated++;
    }

    size_t remaining = total;
    curr = block;
    while (curr) {
        char buffer[BLOCK_SIZE];
        struct block curr_block;
        get_block(curr, &curr_block, disk);
        memset(buffer, 0, sizeof(buffer));
        memcpy(&buffer, &curr_block, sizeof(curr_block));

        size_t to_copy = (remaining > DATA_SIZE) ? DATA_SIZE : remaining;
        memcpy(buffer + sizeof(curr_block), in, to_copy);
        neofs_write_block(disk, curr, BLOCK_SIZE, buffer);
        curr = curr_block.next;
        in += to_copy;
        remaining -= to_copy;
        blocks_written++;
    }

    file_meta_block.size = total;
    neofs_write_block(disk, desc->meta_block, sizeof(file_meta_block), &file_meta_block);

    return total;
}

static int remove_file(int file_meta_block_n, struct disk *disk) {
    if (file_meta_block_n <= 1) {
        return -1;
    }

    struct meta_block file_meta_block;
    get_meta_block(file_meta_block_n, &file_meta_block, disk);

    struct meta_block parent;
    get_meta_block(file_meta_block.parent, &parent, disk);
    if (parent.start == file_meta_block_n) {
        parent.start = file_meta_block.next;
        neofs_write_block(disk, file_meta_block.parent, sizeof(parent), &parent);
    } else {
        struct meta_block child;
        int child_n = parent.start;
        get_meta_block(child_n, &child, disk);

        kbool found = false;
        while (1) {
            if (child.next == file_meta_block_n) {
                child.next = file_meta_block.next;
                neofs_write_block(disk, child_n, sizeof(child), &child);
                found = true;
                break;
            }

            if (!child.next) {
                break;
            }

            child_n = child.next;
            get_meta_block(child_n, &child, disk);
        }

        if (!found) {
            return -1;
        }
    }

    if (!file_meta_block.start) {
        remove_meta_block(file_meta_block_n, disk);
        return 0;
    }

    if (file_meta_block.is_dir) {
        struct meta_block child;
        int child_n = file_meta_block.start;
        get_meta_block(file_meta_block.start, &child, disk);
        while (child.next) {
            int next = child.next;
            remove_file(child_n, disk);
            get_meta_block(next, &child, disk);
            child_n = next;
        }

        remove_file(child_n, disk);

        remove_meta_block(file_meta_block_n, disk);
    } else {
        struct block block;
        int block_n = file_meta_block.start;
        get_block(file_meta_block.start, &block, disk);
        while (block.next) {
            int next = block.next;
            remove_block(block_n, disk);
            get_block(next, &block, disk);
            block_n = next;
        }

        remove_block(block_n, disk);

        remove_meta_block(file_meta_block_n, disk);
    }

    return 0;
}

int neofs_remove(struct disk *disk, struct path_part *path) {
    int file_meta_block_n = neofs_get_path_meta_block(path, false, disk);
    remove_file(file_meta_block_n, disk);
    return 0;
}

struct dirent neofs_readdir(struct disk *disk, void *private) {
    struct dirent dirent;
    memset(&dirent, 0, sizeof(dirent));

    struct neofs_file_descriptor *desc = private;
    if (desc->type != FILE_TYPE_DIR) {
        return dirent;
    }

    struct meta_block dir;
    get_meta_block(desc->meta_block, &dir, disk);

    int i = 0;
    int curr = dir.start;
    kbool found = false;
    while (1) {
        if (i == desc->pos) {
            found = true;
            break;
        }

        if (!curr) {
            break;
        }

        struct meta_block temp;
        get_meta_block(curr, &temp, disk);
        if (!temp.next) {
            break;
        }

        curr = temp.next;

        i++;
    }

    if (!found) {
        return dirent;
    }

    struct meta_block child;
    get_meta_block(curr, &child, disk);

    strncpy(dirent.name, child.filename, sizeof(dirent.name));

    if (child.is_dir) {
        dirent.type = DIRENT_TYPE_DIR;
    } else {
        if (child.flags & FLAG_X) {
            dirent.type = DIRENT_TYPE_EXEC;
        } else {
            dirent.type = DIRENT_TYPE_FILE;
        }
    }

    desc->pos++;

    return dirent;
}

int neofs_close(void *private) {
    struct neofs_file_descriptor *desc = private;
    kfree(desc);
    return 0;
}