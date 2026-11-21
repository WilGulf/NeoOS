#include <efi.h>
#include <efilib.h>

#include <stdbool.h>
#include <stdint.h>
#include "string.h"

#include "fs.h"
#include "disk.h"
#include "path_parser.h"

struct master_block {
    uint64_t magic;

    uint64_t block_size;

    uint64_t bitmap_meta_block;

    uint64_t first_block;
    uint64_t block_count;
} __attribute__((packed));

struct meta_block {
    char filename[8];
    char ext[3];

    uint8_t is_dir;
    uint8_t flags;
    uint64_t size;

    uint64_t start;
    uint64_t next;
    uint64_t parent;
} __attribute__((packed));

struct block {
    uint64_t next;
} __attribute__((packed));

#define NEOFS_MAGIC 0x4E454F46

#define BLOCK_DATA_OFFSET sizeof(struct block)

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

    uint64_t BLOCK_SIZE;
    uint64_t DATA_SIZE;
};

struct neofs_file_descriptor {
    int meta_block;
    FILE_MODE mode;
    FILE_TYPE type;
    uint64_t pos;
};

static struct neofs_private private;

int read_file(uint64_t file_meta_block_n, void *out);
int write_file(uint64_t file_meta_block_n, void *in, uint64_t total);

static void neofs_init_private(struct neofs_private *private) {
    ZeroMem(private, sizeof(*private));
}

static bool block_is_valid(uint64_t block) {
    return block < private.master_block.block_count;
}

static EFI_STATUS neofs_read_block(uint64_t block, uint64_t size, void *out) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;
    
    EFI_STATUS res = 0;
    
    res = disk_seek(block * private.BLOCK_SIZE);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = disk_read(out, size);
    if (EFI_ERROR(res)) {
        return res;
    }

    return EFI_SUCCESS;
}

static EFI_STATUS neofs_read_block_data(uint64_t block, uint64_t size, void *out) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;

    EFI_STATUS res = 0;

    res = disk_seek((block * private.BLOCK_SIZE) + BLOCK_DATA_OFFSET);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = disk_read(out, size);
    if (EFI_ERROR(res)) {
        return res;
    }

    return EFI_SUCCESS;
}

static EFI_STATUS neofs_write_block(uint64_t block, uint64_t size, void *in) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;

    EFI_STATUS res = disk_seek(block * private.BLOCK_SIZE);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = disk_write(in, size);
    return res;
}

static EFI_STATUS get_master_block(struct master_block *out) {
    ZeroMem(out, sizeof(*out));
    
    EFI_STATUS res = 0;
    
    res = disk_seek(0);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = disk_read(out, sizeof(struct master_block));
    if (EFI_ERROR(res)) {
        return res;
    }

    return EFI_SUCCESS;
}

static EFI_STATUS get_meta_block(uint64_t block, struct meta_block *out) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;

    return neofs_read_block(block, sizeof(struct meta_block), out);
}

static EFI_STATUS get_block(uint64_t block, struct block *out) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;

    return neofs_read_block(block, sizeof(struct block), out);
}

static uint64_t get_free_block(uint64_t start) {
    struct meta_block bitmap;
    if (EFI_ERROR(get_meta_block(private.master_block.bitmap_meta_block, &bitmap)))
        return 0;

    unsigned char *buffer = AllocatePool(bitmap.size);
    if (!buffer) {
        return 0;
    }

    if (read_file(private.master_block.bitmap_meta_block, buffer) < 0) {
        return 0;
    }

    for (uint64_t bit = start; bit < private.master_block.block_count; bit++) {
        unsigned char mask = 1 << (bit % 8);
        if (!(buffer[bit / 8] & mask)) {
            FreePool(buffer);
            if (!block_is_valid(bit))
                return 0;

            return bit;
        }
    }

    FreePool(buffer);

    return 0;
}

EFI_STATUS set_block_on_bitmap(uint64_t block, bool state) {
    if (!block_is_valid(block))
        return EFI_INVALID_PARAMETER;
    
    struct meta_block bitmap;
    if (EFI_ERROR(get_meta_block(private.master_block.bitmap_meta_block, &bitmap)))
        return EFI_DEVICE_ERROR;

    unsigned char *buffer = AllocatePool(bitmap.size);
    if (!buffer) {
        return EFI_OUT_OF_RESOURCES;
    }
    
    if (read_file(private.master_block.bitmap_meta_block, buffer) < 0) {
        return EFI_DEVICE_ERROR;
    }

    unsigned char mask = 1 << (block % 8);
    if (state) {
        buffer[block / 8] |= mask;
    } else {
        buffer[block / 8] &= ~mask;
    }

    if (write_file(private.master_block.bitmap_meta_block, buffer, bitmap.size) < 0) {
        return EFI_DEVICE_ERROR;
    }

    FreePool(buffer);

    return EFI_SUCCESS;
}

static uint64_t get_next_block(uint64_t block) {
    struct block read_block;
    if (EFI_ERROR(get_block(block, &read_block)))
        return 0;

    return read_block.next;
}

static EFI_STATUS remove_meta_block(uint64_t block) {
    struct meta_block empty_block;
    SetMem(&empty_block, 0, sizeof(empty_block));

    EFI_STATUS res = 0;

    res = neofs_write_block(block, sizeof(empty_block), &empty_block);
    if (EFI_ERROR(res)) {
        return res;
    }
    res = set_block_on_bitmap(block, false);
    if (EFI_ERROR(res)) {
        return res;
    }

    return EFI_SUCCESS;
}

static EFI_STATUS remove_block(uint64_t block) {
    struct block empty_block;
    SetMem(&empty_block, 0, sizeof(empty_block));

    EFI_STATUS res = 0;
    res = neofs_write_block(block, sizeof(empty_block), &empty_block);
    if (EFI_ERROR(res)) {
        return res;
    }

    res = set_block_on_bitmap(block, false);
    if (EFI_ERROR(res)) {
        return res;
    }

    return EFI_SUCCESS;
}

EFI_STATUS neofs_resolve(void) {
    EFI_STATUS res = EFI_SUCCESS;
    neofs_init_private(&private);

    struct master_block master_block;

    if (EFI_ERROR(get_master_block(&master_block))) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }
    
    if (master_block.magic != NEOFS_MAGIC) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    if (!master_block.block_count) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    if (master_block.block_size < sizeof(struct meta_block)) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    if (!master_block.block_size) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    if (!master_block.first_block) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    if (master_block.first_block >= master_block.block_count) {
        res = EFI_VOLUME_CORRUPTED;
        goto out;
    }

    private.master_block = master_block;
    private.BLOCK_SIZE = master_block.block_size;
    private.DATA_SIZE = master_block.block_size - sizeof(struct block);

out:
    return res;
}

static uint64_t create_meta_block(uint64_t parent, bool is_dir, const char *name) {
    uint64_t new_block_n = get_free_block(1);
    if (new_block_n < 1) {
        return 0;
    }

    struct meta_block new_block;
    SetMem(new_block.filename, 0, sizeof(new_block.filename));
    SetMem(new_block.ext, 0, sizeof(new_block.ext));

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

    neofs_write_block(new_block_n, sizeof(new_block), &new_block);

    struct meta_block parent_block;
    if (EFI_ERROR(get_meta_block(parent, &parent_block)))
        return 0;
    
    if (!parent_block.start) {
        parent_block.start = new_block_n;
        neofs_write_block(parent, sizeof(parent_block), &parent_block);
    } else {
        struct meta_block child_block;
        if (EFI_ERROR(get_meta_block(parent_block.start, &child_block)))
            return 0;

        int prev_child_block_n = parent_block.start;
        while (child_block.next) {
            prev_child_block_n = child_block.next;
            if (EFI_ERROR(get_meta_block(child_block.next, &child_block)))
                return 0;
        }

        if (EFI_ERROR(get_meta_block(prev_child_block_n, &child_block)))
            return 0;

        child_block.next = new_block_n;
        neofs_write_block(prev_child_block_n, sizeof(child_block), &child_block);
    }

    set_block_on_bitmap(new_block_n, true);
    return new_block_n;
}

static bool name_matches(struct meta_block *meta_block, const char *part) {
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

int read_file(uint64_t file_meta_block_n, void *out) {    
    struct meta_block file_meta_block;
    if (EFI_ERROR(get_meta_block(file_meta_block_n, &file_meta_block)))
        return -1;

    int block = file_meta_block.start;
    size_t remaining = file_meta_block.size;
    int total_to_read = 0;

    while (block && remaining > 0) {
        size_t to_read = (remaining > private.DATA_SIZE) ? private.DATA_SIZE : remaining;
        char buffer[private.DATA_SIZE];
        int res = neofs_read_block_data(block, sizeof(buffer), buffer);
        memcpy(out, buffer, to_read);

        out += to_read;
        remaining -= to_read;
        total_to_read += to_read;
        block = get_next_block(block);;
    }

    return total_to_read;
}

int write_file(uint64_t file_meta_block_n, void *in, uint64_t total) {
    struct meta_block file_meta_block;
    if (EFI_ERROR(get_meta_block(file_meta_block_n, &file_meta_block)))
        return -1;

    int block = file_meta_block.start;
    if (!block) {
        int new_block_n = get_free_block(1);
        if (new_block_n < 1) {
            return -1;
        }

        struct block new_block;
        set_block_on_bitmap(new_block_n, true);
        new_block.next = 0;
        neofs_write_block(new_block_n, sizeof(new_block), &new_block);

        file_meta_block.start = new_block_n;
        neofs_write_block(file_meta_block_n, sizeof(file_meta_block), &file_meta_block);
        block = file_meta_block.start;
    }

    int blocks_written = 0;

    int blocks_needed = total / private.DATA_SIZE;
    if (total % private.DATA_SIZE) {
        blocks_needed++;
    }

    int curr = block;
    int prev = curr;
    int blocks_allocated = 1;
    while (blocks_allocated < blocks_needed) {
        curr = get_next_block(curr);
        if (!curr) {
            int new_block_n = get_free_block(1);
            if (new_block_n < 1) {
                return -1;
            }

            struct block new_block;
            set_block_on_bitmap(new_block_n, true);
            new_block.next = 0;
            neofs_write_block(new_block_n, sizeof(new_block), &new_block);

            struct block prev_block;
            if (EFI_ERROR(get_block(prev, &prev_block)))
                return -1;

            prev_block.next = new_block_n;
            curr = new_block_n;
            neofs_write_block(prev, sizeof(prev_block), &prev_block);
        }

        prev = curr;
        blocks_allocated++;
    }

    size_t remaining = total;
    curr = block;
    while (curr) {
        char buffer[private.BLOCK_SIZE];
        struct block curr_block;
        if (EFI_ERROR(get_block(curr, &curr_block)))
            return -1;

        SetMem(buffer, 0, sizeof(buffer));
        memcpy(&buffer, &curr_block, sizeof(curr_block));

        size_t to_copy = (remaining > private.DATA_SIZE) ? private.DATA_SIZE : remaining;
        memcpy(buffer + sizeof(curr_block), in, to_copy);
        neofs_write_block(curr, private.BLOCK_SIZE, buffer);
        curr = curr_block.next;
        in += to_copy;
        remaining -= to_copy;
        blocks_written++;
    }

    file_meta_block.size = total;
    neofs_write_block(file_meta_block_n, sizeof(file_meta_block), &file_meta_block);

    return total;
}

static int neofs_get_path_meta_block(struct path_part *path_part, bool allow_creation) {
    int res = 0;

    struct master_block master_block;
    if (EFI_ERROR(get_master_block(&master_block)))
        return -1;

    if (!path_part || !path_part->part) {
        return master_block.first_block;
    }
    
    struct meta_block root;
    if (EFI_ERROR(get_meta_block(master_block.first_block, &root)))
        return -1;
    
    struct meta_block curr;
    struct meta_block dir;

    int dir_n = master_block.first_block; // Dir being searched
    int curr_n = root.start; // Inspected file in dir

    if (EFI_ERROR(get_meta_block(master_block.first_block, &dir)))
        return -1;

    if (dir.start) {
        curr_n = dir.start;
        if (EFI_ERROR(get_meta_block(curr_n, &curr)))
            return -1;
    }

    while (path_part->part) {
        bool found = false;
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
            if (EFI_ERROR(get_meta_block(curr_n, &curr)))
                return -1;
        }

        if (found && path_part->next) {
            if (!curr.is_dir) {
                res = -1;
                goto out;
            }

            dir_n = curr_n; // Jump into new dir
            if (EFI_ERROR(get_meta_block(dir_n, &dir)))
                return -1;

            curr_n = dir.start; // Start next search on first child in new dir
            if (curr_n) {
                if (EFI_ERROR(get_meta_block(curr_n, &curr)))
                    return -1;
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

            int new_dir = create_meta_block(dir_n, true, path_part->part);
            
            dir_n = new_dir;
            if (EFI_ERROR(get_meta_block(dir_n, &dir)))
                return -1;

            curr_n = dir.start;
            path_part = path_part->next;
            continue;
        }

        if (!found && !path_part->next) {
            if (!allow_creation) {
                res = -1;
                goto out;
            }

            int new_file = create_meta_block(dir_n, false, path_part->part);
            res = new_file;
            goto out;
        }
    }

out:
    return res;
}
   
int neofs_seek(void *private, uint64_t offset, FILE_SEEK_MODE seek_mode) {
    int res = 0;
    struct neofs_file_descriptor *desc = private;

    struct meta_block meta_block;
    if (EFI_ERROR(get_meta_block(desc->meta_block, &meta_block))) {
        res = -1;
        goto out;
    }

    if (desc->mode == FILE_MODE_APPEND) {
        desc->pos = meta_block.size;
        goto out;
    }

    switch(seek_mode) {
        case SEEK_SET:
            desc->pos = offset;
            break;
        
        case SEEK_END:
            res = -1;
            break;

        case SEEK_CUR:
            desc->pos += offset;
            break;
        
        default:
            res = -1;
            break;
    }

out:
    return res;
}

void *neofs_open(struct path_part *path, FILE_MODE mode) {
    int err_code;
    struct neofs_file_descriptor *descriptor = 0;
    
    int meta_block_n = neofs_get_path_meta_block(path, mode == FILE_MODE_WRITE);
    if (meta_block_n < 1) {
        err_code = -1;
        goto err_out;
    }

    descriptor = AllocatePool(sizeof(struct neofs_file_descriptor));
    if (!descriptor) {
        err_code = -1;
        goto err_out;
    }

    descriptor->meta_block = meta_block_n;
    descriptor->mode = mode;
    descriptor->type = FILE_TYPE_FILE;

    struct meta_block meta_block;
    if (EFI_ERROR(get_meta_block(meta_block_n, &meta_block)))
        return (void *)NULL;

    if (meta_block.is_dir) {
        descriptor->type = FILE_TYPE_DIR;
    }

    if (descriptor->mode == FILE_MODE_APPEND) {
        descriptor->pos = meta_block.size;
    } else {
        descriptor->pos = 0;
    }

    return descriptor;

err_out:
    if (descriptor) {
        FreePool(descriptor);
    }

    return (void *)NULL;
}

int neofs_read(void *descriptor, uint64_t size, uint64_t nmemb, char *out_ptr) {
    struct neofs_file_descriptor *desc = descriptor;

    if (desc->mode != FILE_MODE_READ) {
        return -1;
    }

    struct meta_block meta_block;
    if (EFI_ERROR(get_meta_block(desc->meta_block, &meta_block)))
        return -1;

    if (!(meta_block.flags & FLAG_R)) {
        return -1;
    }

    if (desc->pos >= meta_block.size) {
        return -1;
    }

    size_t remaining = meta_block.size - desc->pos;
    uint64_t requested = size * nmemb;
    if (requested > remaining) {
        requested = remaining;
    }

    uint64_t total_to_read = 0;

    int blocks_offset = desc->pos / private.DATA_SIZE;

    uint64_t block = meta_block.start;
    struct block temp;
    if (EFI_ERROR(get_block(block, &temp)))
        return -1;
    
    int i = 0;
    while (i < blocks_offset) {
        block = temp.next;
        if (!block) {
            return -1;
        }

        if (EFI_ERROR(get_block(temp.next, &temp)))
            return -1;

        i++;
    }

    bool first = true;
    while (block && requested > 0) {    
        uint64_t offset;
        if (first) {
            offset = desc->pos % private.DATA_SIZE;
            first = false;
        } else {
            offset = 0;
        }

        uint64_t available = private.DATA_SIZE - offset;
        uint64_t size = requested < remaining ? requested : remaining;
        size_t to_read = (size > available) ? available : size;
        char buffer[private.DATA_SIZE];
        int res = neofs_read_block_data(block, private.DATA_SIZE, buffer);
        if (EFI_ERROR(res)) {
            return -1;
        }

        memcpy(out_ptr, buffer + offset, to_read);

        out_ptr += to_read;
        remaining -= to_read;
        requested -= to_read;
        total_to_read += to_read;
        block = get_next_block(block);;
    }

    desc->pos += total_to_read;

    return total_to_read;
}

int neofs_stat(void *private, struct file_stat *stat) {
    struct neofs_file_descriptor *desc = private;
    struct meta_block meta_block;
    if (EFI_ERROR(get_meta_block(desc->meta_block, &meta_block)))
        return -1;

    stat->filesize = meta_block.size;
    stat->flags = 0x00;

    return 0;
}

int neofs_write(void *descriptor, uint64_t size, uint64_t nmemb, const char *in) {
    uint64_t total = size * nmemb;

    struct neofs_file_descriptor *desc = descriptor;
    if (!(desc->mode == FILE_MODE_WRITE || desc->mode == FILE_MODE_APPEND)) {
        return -1;
    }
    
    struct meta_block file_meta_block;
    if (EFI_ERROR(get_meta_block(desc->meta_block, &file_meta_block)))
        return -1;

    if (!(file_meta_block.flags & FLAG_W)) {
        return -1;
    }
    
    if (desc->mode == FILE_MODE_APPEND) {
        uint64_t buffer_size = file_meta_block.size + total;
        char *buffer = AllocatePool(buffer_size);
        if (!buffer) {
            return -1;
        }
        
        read_file(desc->meta_block, buffer);
        memcpy(buffer + file_meta_block.size, in, total);
        int res = write_file(desc->meta_block, buffer, buffer_size);
        
        if (res >= 0) {
            desc->pos = res;
        }

        FreePool(buffer);
        return res;
    } else {
        return write_file(desc->meta_block, in, total);
    }
}

static int remove_file(uint64_t file_meta_block_n) {
    if (file_meta_block_n <= 1) {
        return -1;
    }

    struct meta_block file_meta_block;
    if (EFI_ERROR(get_meta_block(file_meta_block_n, &file_meta_block)))
        return -1;

    struct meta_block parent;
    if (EFI_ERROR(get_meta_block(file_meta_block.parent, &parent)))
        return -1;

    if (parent.start == file_meta_block_n) {
        parent.start = file_meta_block.next;
        neofs_write_block(file_meta_block.parent, sizeof(parent), &parent);
    } else {
        struct meta_block child;
        int child_n = parent.start;
        if (EFI_ERROR(get_meta_block(child_n, &child)))
            return -1;

        bool found = false;
        while (1) {
            if (child.next == file_meta_block_n) {
                child.next = file_meta_block.next;
                neofs_write_block(child_n, sizeof(child), &child);
                found = true;
                break;
            }

            if (!child.next) {
                break;
            }

            child_n = child.next;
            if (EFI_ERROR(get_meta_block(child_n, &child)))
                return -1;
        }

        if (!found) {
            return -1;
        }
    }

    if (!file_meta_block.start) {
        remove_meta_block(file_meta_block_n);
        return 0;
    }

    if (file_meta_block.is_dir) {
        struct meta_block child;
        int child_n = file_meta_block.start;
        if (EFI_ERROR(get_meta_block(file_meta_block.start, &child)))
            return -1;

        while (child.next) {
            int next = child.next;
            remove_file(child_n);
            if (EFI_ERROR(get_meta_block(next, &child)))
                return -1;

            child_n = next;
        }

        remove_file(child_n);

        remove_meta_block(file_meta_block_n);
    } else {
        struct block block;
        int block_n = file_meta_block.start;
        if (EFI_ERROR(get_block(file_meta_block.start, &block)))
            return -1;

        while (block.next) {
            int next = block.next;
            remove_block(block_n);
            if (EFI_ERROR(get_block(next, &block)))
                return -1;

            block_n = next;
        }

        remove_block(block_n);

        remove_meta_block(file_meta_block_n);
    }

    return 0;
}

int neofs_remove(struct path_part *path) {
    int file_meta_block_n = neofs_get_path_meta_block(path, false);
    return remove_file(file_meta_block_n);
}

struct dirent neofs_readdir(void *private) {
    struct dirent dirent;
    SetMem(&dirent, 0, sizeof(dirent));

    struct neofs_file_descriptor *desc = private;
    if (desc->type != FILE_TYPE_DIR) {
        return dirent;
    }

    struct meta_block dir;
    if (EFI_ERROR(get_meta_block(desc->meta_block, &dir)))
        return dirent;

    int i = 0;
    int curr = dir.start;
    bool found = false;
    while (curr) {
        if ((uint64_t)i == desc->pos) {
            found = true;
            break;
        }

        if (!curr) {
            break;
        }

        struct meta_block temp;
        if (EFI_ERROR(get_meta_block(curr, &temp)))
            return dirent;

        curr = temp.next;

        i++;
    }

    if (!found) {
        return dirent;
    }

    struct meta_block child;
    if (EFI_ERROR(get_meta_block(curr, &child)))
        return dirent;

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
    FreePool(desc);
    return 0;
}

int neofs_create(struct path_part *path) {
    struct path_part *tmp = path;
    while (tmp->next && tmp->next->next) {
        tmp = tmp->next;
    }

    if (!tmp->next) {
        struct master_block master;
        if (EFI_ERROR(get_master_block(&master))) {
            return -1;
        }

        return create_meta_block(master.first_block, false, tmp->part);
    }

    struct path_part *save = tmp->next;
    tmp->next = NULL;

    uint64_t parent = neofs_get_path_meta_block(path, false);

    tmp->next = save;

    if (parent > 0 && block_is_valid(parent)) {
        return create_meta_block(parent, false, save->part);
    } else {
        return -1;
    }
}

int neofs_mkdir(struct path_part *path) {
    struct path_part *tmp = path;
    while (tmp->next && tmp->next->next) {
        tmp = tmp->next;
    }

    if (!tmp->next) {
        struct master_block master;
        if (EFI_ERROR(get_master_block(&master))) {
            return -1;
        }
        
        int res = create_meta_block(master.first_block, true, tmp->part);
        if (res > 0) {
            return 0;
        } else {
            return -1;
        }
    }

    struct path_part *save = tmp->next;
    tmp->next = NULL;

    uint64_t parent = neofs_get_path_meta_block(path, false);

    tmp->next = save;

    if (parent > 0 && block_is_valid(parent)) {
        int res = create_meta_block(parent, true, save->part);
        if (res > 0) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}