#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif