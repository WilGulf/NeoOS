#include "stdio.h"
#include "stdlib.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        int fd = fopen(argv[1], "r");
        if (fd) {
            struct file_stat stat;
            fstat(fd, &stat);
            char buffer[stat.filesize];
            fread(buffer, sizeof(buffer), 1, fd);
            printf("%s\n", buffer);
            fclose(fd);
        }
    }
    
    exit();
}