#include "stdlib.h"
#include "stdio.h"
#include "string.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_INPUT | PROMISE_FS);
}

int main(int argc, char **argv) {
    int fd = fopen("D:/test.txt", "w");
    if (fd > 0) {
        char *message = "HELLO FROM RAMFS!";
        fwrite(message, strlen(message), 1, fd);
        fclose(fd);
    } else {
        printf("Failed to create file");
        exit();
    }
    

    fd = fopen("D:/test.txt", "r");
    if (fd > 0) {
        char line[256];
        fread(line, sizeof(line), 1, fd);
        printf("%s", line);
        fclose(fd);
    } else {
        printf("Failed to open file");
        exit();
    }

    //remove("D:/test.txt");

    putchar('\n');
    exit();
}