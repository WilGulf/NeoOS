#include "memory.h"
#include "../include/util.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t total_alloc;

int mem_num_vpages;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

uint8_t physical_memory_bitmap[NUM_PAGE_FRAMES / 8];

static uint32_t page_dirs[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t page_dir_used[NUM_PAGES_DIRS];

void pmm_init(uint32_t memLow, uint32_t memHigh) {
    page_frame_min = CEIL_DIV(memLow, 0x1000);
    page_frame_max = memHigh / 0x1000;
    total_alloc = 0;

    memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}

uint32_t pmm_alloc_page_frame() {
    uint32_t start = page_frame_min / 8 + ((page_frame_min & 7) != 0 ? 1 : 0);
    uint32_t end = page_frame_max / 8 - ((page_frame_max & 7) != 0 ? 1 : 0);

    for (uint32_t b = start; b < end; b++) {
        uint8_t byte = physical_memory_bitmap[b];
        if (byte == 0xFF) {
            continue;
        }

        for (uint32_t i = 0; i < 8; i++) {
            kbool used = byte >> i & 1;

            if (!used) {
                byte ^= (-1 ^ byte) & (1 << i);
                physical_memory_bitmap[b] = byte;
                total_alloc++;

                uint32_t addr = (b * 8 + i) * 0x1000;
                return addr;
            }
        }
    }

    return 0;
}

void invalidate_page(uint32_t vaddr) {
    asm volatile("invlpg %0" :: "m"(vaddr));
}

uint32_t* mem_get_current_page_dir() {
    uint32_t pd;
    asm volatile ("mov %%cr3, %0": "=r"(pd));
    pd += KERNEL_START;

    return (uint32_t*) pd;
}

void mem_change_page_dir(uint32_t* pd) {
    pd = (uint32_t*) (((uint32_t)pd) - KERNEL_START);
    asm volatile ("mov %0, %%eax \n mov %%eax, %%cr3 \n" :: "m"(pd));
}

void mem_sync_page_dirs() {
    for (int i = 0; i < NUM_PAGES_DIRS; i++) {
        if (page_dir_used[i]) {
            uint32_t* page_dir = page_dirs[i];

            for (int i = 768; i < 1023; i++) {
                page_dir[i] = initial_page_dir[i] & ~PAGE_FLAG_OWNER;
            }
        }
    }
}

void mem_map_page(uint32_t vaddr, uint32_t addr, uint32_t flags) {
    uint32_t *prev_page_dir = 0;

    if (vaddr >= KERNEL_START) {
        prev_page_dir = mem_get_current_page_dir();

        if (prev_page_dir != initial_page_dir) {
            mem_change_page_dir(initial_page_dir);
        }
    }

    uint32_t pd_index = vaddr >> 22;
    uint32_t pt_index = vaddr >> 12 & 0x3FF;

    uint32_t* page_dir = REC_PAGE_DIR;
    uint32_t* pt = REC_PAGE_TABLE(pd_index);

    if (!(page_dir[pd_index] & PAGE_FLAG_PRESENT)) {
        uint32_t ptp_addr = pmm_alloc_page_frame();
        page_dir[pd_index] = ptp_addr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_OWNER | flags;
        invalidate_page(vaddr);
        invalidate_page((uint32_t)pt);

        for (uint32_t i = 0; i < 1024; i++) {
            pt[i] = 0;
        }
    }

    pt[pt_index] = addr | PAGE_FLAG_PRESENT | flags;
    mem_num_vpages++;
    invalidate_page(vaddr);

    if (prev_page_dir != 0) {
        mem_sync_page_dirs();
        if (prev_page_dir != initial_page_dir) {
            mem_change_page_dir(prev_page_dir);
        }
    }
}

void memory_init(uint32_t memHigh, uint32_t physicalAllocStart) {
    mem_num_vpages = 0;
    initial_page_dir[0] = 0;
    invalidate_page(0);
    initial_page_dir[1023] = ((uint32_t) initial_page_dir - KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate_page(0xFFFFF000);

    pmm_init(physicalAllocStart, memHigh);

    memset(page_dirs, 0, 0x1000 * NUM_PAGES_DIRS);
    memset(page_dir_used, 0, NUM_PAGES_DIRS);
}