#include "stdio.h"
#include "stdlib.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    int fd = fopen("0:/data/test.txt", "r");
    printf("fd: %d\n", fd);
    char buffer[1024];
    fread(buffer, sizeof(buffer), 1, fd);
    struct file_stat stat;
    fstat(fd, &stat);
    printf("test.txt size: %d\n", stat.filesize);
    printf("test.txt contains:\n");
    printf("%s\n", buffer);
    fclose(fd);
    exit();
}