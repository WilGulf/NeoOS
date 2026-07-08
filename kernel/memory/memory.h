#ifndef MEMORY_H
#define MEMORY_H

#include "../include/stdint.h"
#include "../drivers/io/io.h"
#include "../multiboot.h"

#define KERNEL_START 0xC0000000
#define KERNEL_MALLOC 0xD0000000

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
#define PAGE_FLAG_OWNER (1 << 9)

#define REC_PAGE_DIR ((uint32_t*)0xFFFFF000)
#define REC_PAGE_TABLE(i) ((uint32_t*) (0xFFC00000 + ((i) << 12)))

extern uint32_t initial_page_dir[1024];
extern int mem_num_vpages;

uint32_t* mem_get_current_page_dir();
void mem_change_page_dir(uint32_t* pd);
void mem_sync_page_dirs();
void mem_map_page(uint32_t vaddr, uint32_t addr, uint32_t flags);

void memory_init(uint32_t memHigh, uint32_t physicalAllocStart);
void pmm_init(uint32_t memLow, uint32_t memHigh);
uint32_t pmm_alloc_page_frame();

#endif