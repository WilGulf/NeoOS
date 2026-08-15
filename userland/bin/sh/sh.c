#include "stdlib.h"
#include "stdio.h"

int main(int arc, char **argv) {
    print("Hello from C in userland\n");

    void *ptr = malloc(512);
    if (ptr) {
        
    }

    while (1) {
        int c = getkey();
        if (c != 0) {
            print((char *)c);
        }
    }

    return 0;
}