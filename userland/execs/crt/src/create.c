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
            int fd = fopen(argv[i], "w");
            if (fd) {
                for (int j = 0; j < 10; j++) {
                    fwrite(0, 1, 1, fd);
                }
                fclose(fd);

                printf("Created: ");
                printf("%s\n", argv[i]);
            }
            i++;
        }

        putchar('\n');
    }

    exit();
}