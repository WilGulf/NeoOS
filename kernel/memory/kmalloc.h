#ifndef KMALLOC_H
#define KMALLOC_H

#include "../include/stdint.h"

void kmalloc_init(uint32_t initial_heap_size);
void *kmalloc(uint32_t size);

#endif