#include "file.h"
#include "fat/fat16.h"
#include "disk.h"

#include "../io/io.h"
#include "../../include/util.h"
#include "../../include/status.h"
#include "../../memory/kheap.h"


struct filesystem *filesystems[MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[MAX_FILE_DESCRIPTORS];

static struct filesystem **fs_get_free_filesystem() {
    int i = 0;
    for (; i < MAX_FILESYSTEMS; i++) {
        if (filesystems[i] == 0) {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct filesystem *filesystem) {
    struct filesystem **fs;
    fs = fs_get_free_filesystem();
    if (!fs) {
        kprintf("Could not insert filesystem\n");
        while (1) {}
    }

    *fs = filesystem;
}

static void fs_static_load() {
    fs_insert_filesystem(fat16_init());
}

void fs_load() {
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_free_descriptor(struct file_descriptor *desc) {
    file_descriptors[desc->index - 1] = 0;
    kfree(desc);
    return 0;
}

static int file_new_descriptor(struct file_descriptor **desc_out) {
    int res = -ERROR_NO_MEM;
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor *file_get_descriptor(int fd) {
    if (fd <= 0 || fd >= MAX_FILE_DESCRIPTORS) {
        return 0;
    }

    int i = fd - 1;
    return file_descriptors[i];
}

struct filesystem *fs_resolve(struct disk *disk) {
    struct filesystem *fs = 0;
    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char *str) {
    FILE_MODE mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    } else if (strncmp(str, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    } else if (strncmp(str, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}
int fopen(const char *filename, const char *mode_str) {
    int res = 0;
    struct disk *disk = 0;
    FILE_MODE mode = FILE_MODE_INVALID;
    void *descriptor_private_data = 0;
    struct file_descriptor *desc = 0;

    struct path_root *root_path = parse_path(filename, NULL);
    if (!root_path) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    if (!root_path->first) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    disk = disk_get(root_path->drive_no);
    if (!disk) {
        res = -ERROR_IO;
        goto out;
    }

    if (!disk->filesystem) {
        res = -ERROR_IO;
        goto out;
    }

    mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (ISERR(descriptor_private_data)) {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    res = file_new_descriptor(&desc);
    if (res < 0) {
        goto out;
    }

    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    if (res < 0) {
        if (root_path) {
            path_parser_free(root_path);
            root_path = NULL;
        }

        if (descriptor_private_data && disk) {
            disk->filesystem->close(descriptor_private_data);
            descriptor_private_data = NULL;
        }
        
        if (desc) {
            file_free_descriptor(desc);
            desc = NULL;
        }
        
        res = 0;
    }

    return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence) {
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -ERROR_IO;
        goto out;
    }

    res = desc->filesystem->seek(desc->private, offset, whence);
out:
    return res;
}
 
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd) {
    int res = 0;
    if (size == 0 || nmemb == 0 || fd < 1) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -ERROR_INVALID_ARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char *)ptr);
out:
    return res;
}

int fstat(int fd, struct file_stat *stat) {
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -ERROR_IO;
        goto out;
    }

    res = desc->filesystem->stat(desc->disk, desc->private, stat);
out:
    return res;
}

int fclose(int fd) {
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -ERROR_IO;
        goto out;
    }

    res = desc->filesystem->close(desc->private);
    if (res == ALL_OK) {
        file_free_descriptor(desc);
    }
out:
    return res;
}

char *fgets(char *str, int size, int fd) {
    if (!str || size <= 1) {
        return 0;
    }

    int i = 0;
    while (i < size - 1) {
        char c;
        
        if (fread(&c, 1, 1, fd) != 1) {
            if (i == 0) {
                return 0;
            }

            break;
        }

        str[i++] = c;

        if (c == '\n') {
            break;
        }
    }

    str[i] = '\0';
    return str;
}