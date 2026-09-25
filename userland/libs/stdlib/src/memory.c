#include "memory.h"

typedef unsigned int size_t;

#define NULL ((void *)0)

void *memset(void *ptr, int c, size_t size) {
    unsigned char *b = ptr;
    for (int i = 0; i < size; i++) {
        *b = c;
        b++;
    }
    return ptr;
}

void *memcpy(void *dest, void *src, int len) {
    char *d = dest;
    char *s = src;
    
    int i = 0;
    while (i < len) {
        *d++ = *s++;
        i++;
    }

    return dest;
}

int memcmp(void* ptr1, void* ptr2, int count) {
    char *char1 = ptr1;
    char *char2 = ptr2;

    while (count-- && *char1 == *char2) {
        char1++;
        char2++;
    }

    return 0;
}