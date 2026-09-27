#include "stdlib.h"
#include "stdio.h"
#include "promise.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        int i = 1;
        while (argv[i]) {
            if (create(argv[1]) == 0) {
                printf("Created: %s\n", argv[1]);
            }
            i++;
        }

        putchar('\n');
    }

    exit();
}