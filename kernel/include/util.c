#include "util.h"
#include "stdint.h"

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }
}

int pow(int x, int power) {
    long int result = 1;

    for (int i = 1; i <= power; i++) {
        result = result * x;
    }
    
    return result;
}