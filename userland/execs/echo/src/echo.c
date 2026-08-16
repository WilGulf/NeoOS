#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv) {
    int i = 1;
    while (argv[i]) {
        printf("%s ", argv[i]);
        i++;
    }
    
    exit();
}