#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include <stdbool.h>

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
    }

    if (kernel)
        printf("");
    if (os)
        printf("NeoOS");
    if (machine)
        printf("i386");
    
    putchar('\n');
    exit();
}