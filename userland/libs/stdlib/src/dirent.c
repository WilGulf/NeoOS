#include "dirent.h"

#include "memory.h"

extern int sys_readdir(int fd, void *out);
struct dirent readdir(int fd) {
    struct dirent dirent;
    memset(&dirent, 0, sizeof(dirent));
    sys_readdir(fd, &dirent);
    return dirent;
}