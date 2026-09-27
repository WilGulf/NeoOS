#include "stdlib.h"
#include "stdio.h"
#include "dirent.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        int i = 1;
        while (argv[i]) {
            if (mkdir(argv[i]) == 0) {
                printf("Created: %s\n", argv[i]);
            }
            i++;
        }

        putchar('\n');
    }

    exit();
}