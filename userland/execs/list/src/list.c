#include "stdlib.h"
#include "stdio.h"
#include "dirent.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        int fd = fopen(argv[1], "r");
        if (fd) {
            struct dirent dirent = readdir(fd);
            while (dirent.name[0]) {
                dirent = readdir(fd);

                if (dirent.type == DIRENT_TYPE_FILE) {
                    printf("\033[0m");
                }
                if (dirent.type == DIRENT_TYPE_DIR) {
                    printf("\033[34m");
                }
                if (dirent.type == DIRENT_TYPE_EXEC) {
                    printf("\033[32m");
                }

                printf("%s ", dirent.name);
            }

            printf("\033[0m\n");
            putchar('\n');
        }
    }
    
    exit();
}