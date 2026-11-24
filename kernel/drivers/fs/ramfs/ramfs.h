#ifndef RAMFS_H
#define RAMFS_H

#define RAMFS_MAX_NAME 32

#include "../file.h"
#include "../../../include/stdint.h"

struct disk;
struct filesystem *ramfs_init();

typedef int RAMFILE_TYPE;
enum {
    RAMFILE_TYPE_INVALID,
    RAMFILE_TYPE_DIR,
    RAMFILE_TYPE_DATA,
    RAMFILE_TYPE_FUNCTIONS,
    RAMFILE_TYPE_STREAMFUNCTIONS,
};

typedef int FUNCTION_TYPE;
enum {
    FUNCTION_TYPE_FILE,
    FUNCTION_TYPE_STREAM,
};

#define RAMFS_FLAG_R 0b001
#define RAMFS_FLAG_W 0b010
typedef int (*FUNCTION_RAMFILE_OPEN)(void *private);
typedef int (*FUNCTION_RAMFILE_CLOSE)(void *private);
typedef int (*FUNCTION_RAMFILE_READ)(void *private, uint64_t offset, uint64_t size, uint64_t nmemb, char *out);
typedef int (*FUNCTION_RAMFILE_WRITE)(void *private, uint64_t offset, uint64_t size, uint64_t nmemb, const char *in);

struct ramfs_file {
    char name[RAMFS_MAX_NAME];
    RAMFILE_TYPE type;

    uint8_t flags; //RWX
    uint8_t privilege_requirement; //LEVELS 0..8
    uint8_t promise_requirement; //EG PROMISE_FB

    struct ramfs_file *first_child;
    struct ramfs_file *sibling;
    
    //RAMFILE_TYPE_DATA
    void *data;
    size_t size;

    //RAMFILE_TYPE_FUNCTION
    void *private;
    FUNCTION_TYPE function_type;
    FUNCTION_RAMFILE_OPEN open;
    FUNCTION_RAMFILE_CLOSE close;
    FUNCTION_RAMFILE_READ read;
    FUNCTION_RAMFILE_WRITE write;
};

//ONLY USED BY KERNEL TO INTERRACT WITH THE FILESYSTEM
int kernel_create_function_ramfile(const char *path, void *priv, size_t size, FUNCTION_TYPE type, FUNCTION_RAMFILE_OPEN open_function, FUNCTION_RAMFILE_CLOSE close_function, FUNCTION_RAMFILE_READ read_function, FUNCTION_RAMFILE_WRITE write_function, uint8_t privilege_req, uint8_t promise_req, uint8_t flags);
int kernel_create_data_ramfile(const char *path, size_t filesize, uint8_t privilege_req, uint8_t promise_req, uint8_t flags);
void  *kernel_get_data_ramfile_data(const char *path, size_t *size_out);

#endif