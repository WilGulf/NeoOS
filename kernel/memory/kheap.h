#ifndef KHEAP_H
#define KHEAP_H

#include "../include/stdint.h"

#define KHEAP_SIZE_BYTES 104857600
#define KHEAP_BLOCK_SIZE 4096
#define KHEAP_ADDRESS 0x01000000
#define KHEAP_TABLE_ADDRESS 0x00007E00

void kheap_init();
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void kfree(void *ptr);

#endif