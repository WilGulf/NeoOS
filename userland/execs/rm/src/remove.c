#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_FS);
}

int main(int argc, char **argv) {
    if (argv[1]) {
        printf("Removed: ");
        int i = 1;
        while (argv[i]) {
            remove(argv[i]);
            printf("%s ", argv[i]);
            i++;
        }
    }

    exit();
}