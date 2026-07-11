#include "util.h"
#include "stdint.h"

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }
}

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len) {
    const uint8_t *sp = (const uint8_t *)src;
    uint8_t *dp = (uint8_t *)dest;
    for(; len != 0; len--) {
        *dp++ = *sp++;
    }
}

char *strcpy(char *dest, const char *src) {
    do {
        *dest++ = *src++;
    }

    while ((*dest++ = *src++));
}

int strlen(char *src) {
    int i = 0;
    while (*src++) {
        i++;
    }
    return i;
}

int strcmp(char *str1, char *str2) {
    int i = 0;
    int failed = 0;
    while(str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            failed = 1;
            break;
        }

        i++;
    }

    if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0')) {
        failed = 1;
    }

    return failed;
}

int pow(int x, int power) {
    long int result = 1;

    for (int i = 1; i <= power; i++) {
        result = result * x;
    }
    
    return result;
}