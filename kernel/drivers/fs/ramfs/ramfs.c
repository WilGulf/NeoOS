#include "ramfs.h"

#include "../file.h"
#include "../disk.h"
#include "../../../include/stdint.h"
#include "../../../include/status.h"
#include "../../../include/util.h"
#include "../../../memory/kheap.h"

struct ramfs_file *first_file;

int ramfs_resolve(struct disk *disk);
void *ramfs_open(struct disk *disk, struct path_part *path, FILE_MODE mode);
int ramfs_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr);
int ramfs_stat(struct disk *disk, void *descriptor, struct file_stat *stat);
int ramfs_seek(void *descriptor, uint32_t offset, FILE_SEEK_MODE seek_mode);
int ramfs_write(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, const char *in);
int ramfs_remove(struct disk *disk, struct path_part *path);
int ramfs_close(void *private);

struct filesystem ramfs = {
    .resolve = ramfs_resolve,
    .open = ramfs_open,
    .seek = ramfs_seek,
    .read = ramfs_read,
    .stat = ramfs_stat,
    .write = ramfs_write,
    .remove = ramfs_remove,
    .close = ramfs_close,
};

struct ramfs_file_descriptor {
    FILE_MODE mode;
    struct ramfs_file *file;
    uint32_t pos;
};

int ramfs_resolve(struct disk *disk) {
    return -ERROR_FS_NOT_US;
}

struct filesystem *ramfs_init() {
    strcpy(ramfs.name, "RAMFS");
    return &ramfs;
}

struct ramfs_file *ramfs_create_fullpath(struct path_part *path) {
    struct ramfs_file *current_dir = first_file;
    struct ramfs_file *parent = 0;
    struct ramfs_file *file = 0;

    while (path->part) {
        file = 0;
        struct ramfs_file *current_file = current_dir;

        while (current_file) {
            if (!strcmp(current_file->name, path->part)) {
                file = current_file;
                break;
            }

            current_file = current_file->next;
        }

        if (!file) {
            file = kzalloc(sizeof(struct ramfs_file));
            if (!file) {
                return 0;
            }

            strncpy(file->name, path->part, sizeof(file->name));
            file->name[sizeof(file->name) - 1] = '\0';

            if (parent) {
                file->parent = parent;
                file->parent->total_children++;

                if (!file->parent->first_child) {
                    file->parent->first_child = file;
                } else {
                    current_file = file->parent->first_child;
                    while (current_file) {
                        if (current_file->next) {
                            current_file = current_file->next;
                        } else {
                            current_file->next = file;
                            file->prev = current_file;
                            break;
                        }
                    }
                }
            } else {
                file->parent = 0;
                if (!first_file) {
                    first_file = file;
                } else {
                    current_file = first_file;
                    while (current_file) {
                        if (current_file->next) {
                            current_file = current_file->next;
                        } else {
                            current_file->next = file;
                            file->prev = current_file;
                            break;
                        }
                    }
                }
            }

            if (path->next) {
                file->is_dir = 1;
                parent = file;
                current_dir = file;
            } else {
                file->is_dir = 0;
                file->total_size = 4096;
                file->memory = kzalloc(file->total_size);

                return file;
            }
        }

        path = path->next;
    }

    return file;
}


static int ramfs_free_file_descriptor(struct ramfs_file_descriptor *descriptor) {
    kfree(descriptor);
    return 0;
}

int ramfs_remove_file(struct ramfs_file *file) {
    if (file->first_child) {
        struct ramfs_file *child = file->first_child;
        for (int i = 0; i < file->total_children; i++) {
            ramfs_remove_file(child);
            if (child->next) {
                child = child->next;
            } else {
                break;
            }
        }
    }

    if (file->prev) {
        file->prev->next = file->next;
    }
    if (file->next) {
        file->next->prev = file->prev;
    }
    if (file->parent) {
        file->parent->total_children--;
    }
    if (file == file->parent->first_child) {
        if (file->next) {
            file->parent->first_child = file->next;
        } else {
            file->parent->first_child = 0;
        }
    }

    file->next = 0;
    file->prev = 0;

    if (file->memory) {
        kfree(file->memory);
    }

    kfree(file);
}

int ramfs_remove_path(struct path_part *path) {
    struct ramfs_file *file = 0;
    struct ramfs_file *current_file = first_file;
    while (path->next) {
        for (int i = 0; i < current_file->total_children; i++) {
            if (!strcmp(current_file->name, path->part)) {
                file = current_file;
                break;
            } else {
                current_file = current_file->next;
            }
        }

        path = path->next;
        current_file = file->first_child;
    }

    for (int i = 0; i < current_file->parent->total_children; i++) {
        if (!strcmp(current_file->name, path->part)) {
            file = current_file;
        } else {
            current_file = current_file->next;
        }
    }

    if (!file) {
        return 0;
    }
    
    ramfs_remove_file(file);
}

void *ramfs_open(struct disk *disk, struct path_part *path, FILE_MODE mode) {
    int err_code;
    struct path_part *original_path = path;
    struct ramfs_file_descriptor *descriptor = 0;
    struct ramfs_file *file = 0;

    struct ramfs_file *first_in_dir = first_file;
    struct ramfs_file *searched_file = first_file;

    while (path->part) {
        if (!strcmp(searched_file->name, path->part)) {
            if (path->next) {
                if (searched_file->is_dir) {
                    first_in_dir = searched_file->first_child;
                    searched_file = searched_file->first_child;
                }

                path = path->next;
            } else {
                file = searched_file;
                break;
            }
        } else {
            if (!searched_file->next) {
                break;
            }
            if (searched_file->next == first_in_dir) {
                break;
            }

            searched_file = searched_file->next;
        }
    }

    if (mode == FILE_MODE_READ) {
        if (!file) {
            goto err_out;
        }
    }

    if (mode == FILE_MODE_WRITE) {
        if (!file) {
            file = ramfs_create_fullpath(original_path);
        }

        if (file) {
            memset(file->memory, 0, file->total_size);
        } else {
            goto err_out;
        }
    }

    descriptor = kzalloc(sizeof(struct ramfs_file_descriptor));
    if (!descriptor) {
        err_code = -ERROR_NO_MEM;
        goto err_out;
    }

    descriptor->file = file;
    descriptor->pos = 0;
    descriptor->mode = mode;
    return descriptor;

err_out:
    if (descriptor) {
        ramfs_free_file_descriptor(descriptor);
    }

    return err_code;
}

int ramfs_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr) {
    int res = 0;
    struct ramfs_file_descriptor *desc = descriptor;
    struct ramfs_file *file = desc->file;
    int offset = desc->pos;

    if (desc->mode != FILE_MODE_READ) {
        return 0;
    }

    uint32_t total_to_read = size * nmemb;
    if (total_to_read > (file->total_size - desc->pos)) {
        total_to_read = file->total_size - desc->pos;
    }
    
    memcpy(out_ptr, file->memory + desc->pos, total_to_read);
    offset += total_to_read;

    desc->pos = offset;
    res = nmemb;

    return res;
}

int ramfs_stat(struct disk *disk, void *descriptor, struct file_stat *stat) {
    struct ramfs_file_descriptor *desc = descriptor;
    struct ramfs_file *file = desc->file;

    stat->filesize = file->total_size;
    stat->flags = 0x00;

    return 0;
}

int ramfs_seek(void *descriptor, uint32_t offset, FILE_SEEK_MODE seek_mode) {
    int res = 0;
    struct ramfs_file_descriptor *desc = descriptor;
    struct ramfs_file *file = desc->file;
    if (file->is_dir) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    if (offset >= file->total_size) {
        res = -ERROR_IO;
        goto out;
    }

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

int ramfs_write(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, const char *in) {
    int res = 0;
    struct ramfs_file_descriptor *desc = private;
    struct ramfs_file *file = desc->file;
    int offset = desc->pos;

    if (desc->mode != FILE_MODE_WRITE) {
        return 0;
    }

    uint32_t total_to_write = size * nmemb;
    if (total_to_write > (file->total_size - desc->pos)) {
        total_to_write = file->total_size - desc->pos;
    }

    if (desc->pos >= file->total_size) {
        return 0;
    }
    
    memcpy(file->memory + offset, in, total_to_write);
    offset += total_to_write;

    desc->pos = offset;
    res = total_to_write / size;

    return res;
}

int ramfs_remove(struct disk *disk, struct path_part *path) {
    ramfs_remove_path(path);
    return 0;
}

int ramfs_close(void *descriptor) {
    ramfs_free_file_descriptor((struct ramfs_file_descriptor *)descriptor);
    return 0;
}