#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    int i = 1;
    while (argv[i]) {
        printf("%s ", argv[i]);
        i++;
    }
    
    exit();
}