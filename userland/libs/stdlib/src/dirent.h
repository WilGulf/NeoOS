#ifndef DIRENT_H
#define DIRENT_H

#include <stdint.h>

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

struct dirent readdir(int fd);

#endif