#include "stdlib.h"
#include "stdio.h"
#include "promise.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    int i = 1;
    while (argv[i]) {
        if (create(argv[i]) == 0) {
            printf("Created: %s\n", argv[i]);
        }
        i++;
    }

    exit();
}