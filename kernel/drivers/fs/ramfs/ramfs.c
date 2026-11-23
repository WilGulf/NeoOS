#include "ramfs.h"

#include "../file.h"
#include "../../disk/disk.h"
#include "../../../include/stdint.h"
#include "../../../include/status.h"
#include "../../../include/util.h"
#include "../../../memory/kheap.h"
#include "../../../syscalls/promise.h"

int ramfs_resolve(struct disk *disk);
void *ramfs_open(struct disk *disk, struct path_part *path, FILE_MODE mode);
int ramfs_read(struct disk *disk, void *descriptor, uint64_t size, uint64_t nmemb, char *out_ptr);
int ramfs_stat(struct disk *disk, void *descriptor, struct file_stat *stat);
int ramfs_seek(struct disk *disk, void *descriptor, uint64_t offset, FILE_SEEK_MODE seek_mode);
int ramfs_write(struct disk *disk, void *private, uint64_t size, uint64_t nmemb, const char *in);
int ramfs_remove(struct disk *disk, struct path_part *path);
int ramfs_close(void *private);
int ramfs_volume_name(void *private, char *name_out, size_t max);
int ramfs_volume_uuid(void *private, uuid_t *uuid_out);

struct filesystem ramfs = {
    .resolve = ramfs_resolve,
    .open = ramfs_open,
    .seek = ramfs_seek,
    .read = ramfs_read,
    .stat = ramfs_stat,
    .write = ramfs_write,
    .remove = ramfs_remove,
    .close = ramfs_close,
    .volume_name = ramfs_volume_name,
    .volume_uuid = ramfs_volume_uuid,
};

struct ramfs_file_descriptor {
    FILE_MODE mode;
    uint64_t pos;
    struct ramfs_file *file;
};

static struct ramfs_file *ramfs_root;

int ramfs_resolve(struct disk *disk) {
    (void)disk;
    return -ERROR_FS_NOT_US;
}

struct filesystem *ramfs_init() {
    ramfs_root = kzalloc(sizeof(struct ramfs_file));
    strcpy(ramfs_root->name, "root\0");
    ramfs_root->type = RAMFILE_TYPE_DIR;
    ramfs_root->privilege_requirement = 6;
    ramfs_root->promise_requirement = PROMISE_FS;
    
    strcpy(ramfs.name, "RAMFS");
    return &ramfs;
}

struct ramfs_file *ramfs_create_file(struct path_part *path) {
    kbool changed = false;

    struct ramfs_file *current = ramfs_root;
    
    while (path && path->part) {
        if (current->type != RAMFILE_TYPE_DIR) {
            return NULL;
        }

        struct ramfs_file *last_sibling = NULL;
        struct ramfs_file *child = current->first_child;
        while (child && strncmp(child->name, path->part, RAMFS_MAX_NAME) != 0) {
            last_sibling = child;
            child = child->sibling;
        }

        if (!child) {
            changed = true;
            if (path->next) {
                struct ramfs_file *new_dir = kzalloc(sizeof(struct ramfs_file));
                if (!new_dir) {
                    return NULL;
                }

                strncpy(new_dir->name, path->part, RAMFS_MAX_NAME);
                new_dir->type = RAMFILE_TYPE_DIR;
                if (last_sibling) {
                    last_sibling->sibling = new_dir;
                } else {
                    current->first_child = new_dir;
                }

                child = new_dir;
            } else {
                struct ramfs_file *new_file = kzalloc(sizeof(struct ramfs_file));
                if (!new_file) {
                    return NULL;
                }

                strncpy(new_file->name, path->part, RAMFS_MAX_NAME);
                new_file->type = RAMFILE_TYPE_INVALID;

                if (last_sibling) {
                    last_sibling->sibling = new_file;
                } else {
                    current->first_child = new_file;
                }

                child = new_file;
            }
        }

        current = child;
        path = path->next;
    }

    if (!changed) {
        return NULL;
    }

    return current;
}

struct ramfs_file *ramfs_lookup(struct path_part *path) {
    struct ramfs_file *current = ramfs_root;
    while (path && path->part) {
        if (current->type != RAMFILE_TYPE_DIR) {
            return NULL;
        }

        struct ramfs_file *child = current->first_child;
        while (child && strncmp(child->name, path->part, RAMFS_MAX_NAME) != 0) {
            child = child->sibling;
        }

        if (!child) {
            return NULL;
        }

        current = child;
        path = path->next;
    }

    return current;
}

struct ramfs_file *ramfs_lookup_parent(struct path_part *path) {
    struct ramfs_file *parent = NULL;
    struct ramfs_file *current = ramfs_root;

    while (path && path->part) {
        if (current->type != RAMFILE_TYPE_DIR) {
            return NULL;
        }

        struct ramfs_file *child = current->first_child;
        while (child && strncmp(child->name, path->part, RAMFS_MAX_NAME) != 0) {
            child = child->sibling;
        }

        if (!child) {
            return NULL;
        }

        parent = current;
        current = child;
        path = path->next;
    }

    return parent;
}

int ramfs_remove_file(struct ramfs_file *file) {
    if (!file) {
        return -ERROR_INVALID_ARG;
    }

    if (file->type == RAMFILE_TYPE_DATA) {
        if (file->data) {
            kfree(file->data);
        }

        kfree(file);

        return ALL_OK;
    } else if (file->type == RAMFILE_TYPE_FUNCTIONS) {
        kfree(file);
        return ALL_OK;
    } else if (file->type == RAMFILE_TYPE_DIR) {
        struct ramfs_file *child = file->first_child;
        
        while (child) {
            struct ramfs_file *sibling = child->sibling;
            ramfs_remove_file(child);
            child = sibling;
        }

        kfree(file);
    }

    return -ERROR_IO;
}

int kernel_create_function_ramfile(const char *path, void *priv, size_t size, FUNCTION_RAMFILE_READ read_function, FUNCTION_RAMFILE_WRITE write_function, uint8_t privilege_req, uint8_t promise_req, uint8_t flags) {
    struct path_root *root_path = parse_path(path, NULL);
    if (!root_path || root_path->drive_id != 'D') {
        path_parser_free(root_path);
        return -ERROR_INVALID_ARG;
    }

    struct ramfs_file *file = ramfs_create_file(root_path->first);
    
    path_parser_free(root_path);
    
    if (!file) {
        return -ERROR_IO;
    }

    file->type = RAMFILE_TYPE_FUNCTIONS;
    file->private = priv;
    file->size = size;
    file->read = read_function;
    file->write = write_function;
    file->privilege_requirement = privilege_req;
    file->promise_requirement = promise_req;
    file->flags = flags;
    
    return ALL_OK;
}

int kernel_create_data_ramfile(const char *path, size_t filesize, uint8_t privilege_req, uint8_t promise_req, uint8_t flags) {
    struct path_root *root_path = parse_path(path, NULL);
    if (!root_path || root_path->drive_id != 'D') {
        path_parser_free(root_path);
        return -ERROR_INVALID_ARG;
    }

    struct ramfs_file *file = ramfs_create_file(root_path->first);
    
    path_parser_free(root_path);
    
    if (!file) {
        return -ERROR_IO;
    }

    file->type = RAMFILE_TYPE_DATA;
    
    file->size = filesize;
    file->data = kzalloc(filesize);
    if (!file->data) {
        return -ERROR_NO_MEM;
    }

    file->privilege_requirement = privilege_req;
    file->promise_requirement = promise_req;
    file->flags = flags;
    
    return ALL_OK;    
}

void *kernel_get_data_ramfile_data(const char *path, size_t *size_out) {
    struct path_root *root_path = parse_path(path, NULL);
    if (!root_path || root_path->drive_id != 'D') {
        path_parser_free(root_path);
        return NULL;
    }

    struct ramfs_file *file = ramfs_lookup(root_path->first);
    
    path_parser_free(root_path);

    if (file && file->type == RAMFILE_TYPE_DATA && file->data) {
        if (size_out) {
            *size_out = file->size;
        }

        return file->data;
    }

    return NULL;
}

static int ramfs_free_file_descriptor(struct ramfs_file_descriptor *descriptor) {
    kfree(descriptor);
    return 0;
}

void *ramfs_open(struct disk *disk, struct path_part *path, FILE_MODE mode) {
    (void)disk;

    struct ramfs_file *file = ramfs_lookup(path);
    if (!file) {
        return NULL;
    }

    if (file->type != RAMFILE_TYPE_DATA && file->type != RAMFILE_TYPE_FUNCTIONS) {
        return NULL;
    }

    if (mode == FILE_MODE_APPEND) {
        return NULL;
    }

    if (mode == FILE_MODE_READ && !(file->flags & RAMFS_FLAG_R)) {
        return NULL;
    }
    if (mode == FILE_MODE_WRITE && !(file->flags & RAMFS_FLAG_W)) {
        return NULL;
    }

    struct ramfs_file_descriptor *desc = kzalloc(sizeof(struct ramfs_file_descriptor));
    if (!desc) {
        return NULL;
    }

    desc->file = file;
    desc->mode = mode;
    desc->pos = 0;

    return desc;
}

int ramfs_read(struct disk *disk, void *descriptor, uint64_t size, uint64_t nmemb, char *out_ptr) {
    (void)disk;
    struct ramfs_file_descriptor *desc = (struct ramfs_file_descriptor *)descriptor;
    if (desc->mode != FILE_MODE_READ) {
        return -ERROR_IO;
    }
    
    struct ramfs_file *file = desc->file;
    if (!file) {
        return -ERROR_IO;
    }

    if (file->type == RAMFILE_TYPE_DATA) {
        if (!file->data) {
            return -ERROR_IO;
        }

        uint64_t to_read = size * nmemb;
        if (to_read > (file->size - desc->pos)) {
            to_read = (file->size - desc->pos);
        }

        memcpy(out_ptr, file->data + desc->pos, to_read);
        desc->pos += to_read;

        return to_read;
    } else if (file->type == RAMFILE_TYPE_FUNCTIONS) {
        if (!file->read) {
            return -ERROR_IO;
        }

        if (desc->pos >= file->size) {
            return 0;
        }

        uint64_t to_read = size * nmemb;
        if (to_read > (file->size - desc->pos)) {
            to_read = (file->size - desc->pos);
        }

        int res = file->read(file->private, desc->pos, 1, to_read, out_ptr);
        if (res > 0) {
            desc->pos += res;
        }

        return res;
    } else {
        return -ERROR_IO;
    }
}

int ramfs_stat(struct disk *disk, void *descriptor, struct file_stat *stat) {
    (void)disk;
    struct ramfs_file_descriptor *desc = (struct ramfs_file_descriptor *)descriptor;
    
    struct ramfs_file *file = desc->file;
    if (!file) {
        return -ERROR_IO;
    }

    stat->filesize = file->size;
    stat->flags = file->flags;

    return ALL_OK;
}

int ramfs_seek(struct disk *disk, void *descriptor, uint64_t offset, FILE_SEEK_MODE seek_mode) {
    (void)disk;

    struct ramfs_file_descriptor *desc = (struct ramfs_file_descriptor *)descriptor;
    if (!desc->file) {
        return -ERROR_IO;
    }

    if (seek_mode == SEEK_SET) {
        desc->pos = offset;
        return ALL_OK;
    }

    if (seek_mode == SEEK_CUR) {
        desc->pos += offset;
        return ALL_OK;
    }

    if (seek_mode == SEEK_END) {
        desc->pos = (desc->file->size + offset);
        return ALL_OK;
    }

    return -ERROR_INVALID_ARG;
}

int ramfs_write(struct disk *disk, void *descriptor, uint64_t size, uint64_t nmemb, const char *in) {
    (void)disk;
    struct ramfs_file_descriptor *desc = (struct ramfs_file_descriptor *)descriptor;
    if (desc->mode != FILE_MODE_WRITE && desc->mode != FILE_MODE_APPEND) {
        return -ERROR_IO;
    }
    
    struct ramfs_file *file = desc->file;
    if (!file) {
        return -ERROR_IO;
    }

    if (file->type == RAMFILE_TYPE_DATA) {
        if (!file->data) {
            return -ERROR_IO;
        }

        uint64_t to_write = size * nmemb;
        if (to_write > (file->size - desc->pos)) {
            to_write = (file->size - desc->pos);
        }

        memcpy(file->data + desc->pos, in, to_write);
        desc->pos += to_write;

        return to_write;
    } else if (file->type == RAMFILE_TYPE_FUNCTIONS) {
        if (!file->write) {
            return -ERROR_IO;
        }

        if (desc->pos >= file->size) {
            return 0;
        }

        uint64_t to_write = size * nmemb;
        if (to_write > (file->size - desc->pos)) {
            to_write = (file->size - desc->pos);
        }

        int res = file->write(file->private, desc->pos, 1, to_write, in);
        if (res > 0) {
            desc->pos += res;
        }

        return res;
    } else {
        return -ERROR_IO;
    }
}

int ramfs_remove(struct disk *disk, struct path_part *path) {
    (void)disk;

    struct ramfs_file *file = ramfs_lookup(path);
    if (!file) {
        return -ERROR_IO;
    }

    struct ramfs_file *parent = ramfs_lookup_parent(path);
    if (!parent) {
        return -ERROR_IO;
    }
    
    struct ramfs_file *child = parent->first_child;
    if (child == file) {
        parent->first_child = file->sibling;
        ramfs_remove_file(file);
        return ALL_OK;
    }

    while (child) {
        if (child->sibling == file) {
            child->sibling = file->sibling;
        }

        child = child->sibling;
    }

    ramfs_remove_file(file);
    return ALL_OK;
}

int ramfs_close(void *descriptor) {
    ramfs_free_file_descriptor(descriptor);
    return ALL_OK;
}

int ramfs_volume_name(void *private, char *name_out, size_t max) {
    (void)private;
    strncpy(name_out, "RAMFS", max);
    return 0;
}

int ramfs_volume_uuid(void *private, uuid_t *uuid_out) {
    (void)private;
    memset(uuid_out, 0, sizeof(uuid_t));
    return 0;
}