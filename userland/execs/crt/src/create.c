#include "stdlib.h"
#include "stdio.h"
#include "promise.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        printf("Created: ");
        int i = 1;
        while (argv[i]) {
            int fd = fopen(argv[i], "w");
            fclose(fd);
            printf("%s ", argv[i]);
            i++;
        }

        putchar('\n');
    }

    exit();
}