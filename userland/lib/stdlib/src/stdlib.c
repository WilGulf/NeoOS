#include "stdlib.h"

void *malloc(size_t size) {
    return kmalloc(size);
}

void free(void *ptr) {
    return kfree(ptr);
}