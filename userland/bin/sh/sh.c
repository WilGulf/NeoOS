#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv) {
    while(1) {
        char c = getkey();
        if (c != 0) {
            printf("%c", c);
        }
    }
}