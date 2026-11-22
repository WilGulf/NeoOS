#ifndef FS_H
#define FS_H

#include <efi.h>
#include <stdint.h>
#include <stddef.h>

struct path_part;

typedef unsigned int FILE_SEEK_MODE;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

typedef unsigned int FILE_TYPE;
enum {
    FILE_TYPE_FILE,
    FILE_TYPE_DIR,
};

typedef unsigned int DIRENT_TYPE;
enum {
    DIRENT_TYPE_FILE,
    DIRENT_TYPE_DIR,
    DIRENT_TYPE_EXEC,
};

struct dirent {
    DIRENT_TYPE type;
    char name[8];
};

typedef unsigned int FILE_STAT_FLAGS;

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

EFI_STATUS neofs_resolve(void);
int neofs_seek(void *private, uint64_t offset, FILE_SEEK_MODE seek_mode);
void *neofs_open(struct path_part *path, FILE_MODE mode);
int neofs_read(void *descriptor, uint64_t size, uint64_t nmemb, char *out_ptr);
int neofs_stat(void *private, struct file_stat *stat);
int neofs_write(void *private, uint64_t size, uint64_t nmemb, const char *in);
int neofs_remove(struct path_part *path);
int neofs_close(void *private);
struct dirent neofs_readdir(void *private);
int neofs_create(struct path_part *path);
int neofs_mkdir(struct path_part *path);
EFI_STATUS neofs_volume_uuid(uint8_t uuid_out[16]);

#endif