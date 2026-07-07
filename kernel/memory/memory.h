#ifndef MEMORY_H
#define MEMORY_H

#include "../include/stdint.h"
#include "../drivers/io/io.h"
#include "../multiboot.h"

#define KERNEL_START 0xC0000000

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)

extern uint32_t initial_page_dir[1024];
void memory_init(uint32_t memHigh, uint32_t physicalAllocStart);

#endif