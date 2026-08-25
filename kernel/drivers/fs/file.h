#ifndef FILE_H
#define FILE_H

#include "path_parser.h"
#include "../../include/stdint.h"

#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512

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

enum {
    FILE_STAT_RO = 0B00000001
};

typedef unsigned int FILE_STAT_FLAGS;

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

struct disk;

typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, char *out);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);
typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk *disk, void *private, struct file_stat *stat);
typedef int (*FS_CLOSE_FUNCTION)(void *private);

struct filesystem {
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_SEEK_FUNCTION seek;
    FS_READ_FUNCTION read;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;

    char name[20];
};

struct file_descriptor {
    int index;
    struct filesystem *filesystem;

    void *private;

    struct disk *disk;
};

void fs_init();
int fopen(const char *filename, const char *mode_str);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd);
int fstat(int fd, struct file_stat *stat);
int fclose(int fd);
char *fgets(char *str, int size, int fd);

void fs_insert_filesystem(struct filesystem *filesystem);
struct filesystem *fs_resolve(struct disk *disk);

#endif