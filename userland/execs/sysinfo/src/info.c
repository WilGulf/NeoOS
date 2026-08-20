#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "kernel.h"

#include <stdbool.h>

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {    
    bool kernel = false;
    bool os = false;
    bool machine = false;

    if (argv[1]) {
        if (!strcmp(argv[1], "-a")) {
            kernel = true;
            os = true;
            machine = true;
        } else if (!strcmp(argv[1], "-g")) {
            kernel = true;
            os = true;
        } else if (!strcmp(argv[1], "-k")) {
            kernel = true;
        } else if (!strcmp(argv[1], "-o")) {
            os = true;
        } else if (!strcmp(argv[1], "-m")) {
            machine = true;
        }

        if (kernel) {
            struct kernel_info info;
            get_kernel(&info);
            printf(
                "%s-%d-%d-%d", 
                info.version.name, 
                info.version.version_major, 
                info.version.version_minor, 
                info.version.version_patch
            );
            if (info.version.extension[0] != 0) {
                printf(".%s", info.version.extension);
            }

            putchar(' ');
        }
        if (os) {
            printf("NeoOS");
            putchar(' ');
        }
        if (machine) {
            printf("i386");
            putchar(' ');
        }
    }

    exit();
}