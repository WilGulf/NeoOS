#include "memory.h"
#include "../include/util.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t totalAlloc;

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

uint8_t physicalMemoryBitmap[NUM_PAGE_FRAMES / 8];

void pmm_init(uint32_t memLow, uint32_t memHigh) {
    page_frame_min = CEIL_DIV(memLow, 0x1000);
    page_frame_max = memHigh / 0x1000;
    totalAlloc = 0;

    memset(physicalMemoryBitmap, 0, sizeof(physicalMemoryBitmap));
}

static uint32_t pageDirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t pageDirsUsed[NUM_PAGE_DIRS];

void invalidate_page(uint32_t vaddr) {
    asm volatile("invlpg %0" :: "m"(vaddr));
}

void memory_init(uint32_t memHigh, uint32_t physicalAllocStart) {
    initial_page_dir[0] = 0;
    invalidate_page(0);
    initial_page_dir[1023] = ((uint32_t) initial_page_dir - KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate_page(0xFFFFF000);

    pmm_init(physicalAllocStart, memHigh);

    memset(pageDirs, 0, 0x1000 * NUM_PAGE_DIRS);
    memset(pageDirs, 0, NUM_PAGE_DIRS);
}