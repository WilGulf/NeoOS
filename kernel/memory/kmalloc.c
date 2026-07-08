#include "kmalloc.h"
#include "memory.h"
#include "../include/util.h"

static uint32_t heap_start;
static uint32_t heap_size;
static uint32_t threshold;
static kbool kmalloc_initialized = false;

void change_heap_size(int new_size) {
    int old_page_top = CEIL_DIV(heap_size, 0x1000);
    int new_page_top = CEIL_DIV(new_size, 0x1000);
    
    if (new_page_top > old_page_top) {
        int diff = new_page_top - old_page_top;

        for (int i = 0; i < diff; i++) {//loop through all new pages needed
            uint32_t phys = pmm_alloc_page_frame();//allocate a new physical page
            kprintf("addr: %d\n", phys);
            mem_map_page(KERNEL_MALLOC + old_page_top * 0x1000 + i * 0x1000, phys, PAGE_FLAG_WRITE);//update the map to include all the new pages
        }
    }

    heap_size = new_size;
}

void kmalloc_init(uint32_t initial_heap_size) {
    heap_start = KERNEL_MALLOC; //set heap_start to a address not directly where kernel beginns but a bit after
    heap_size = 0; //set up heap_size with starting value 0
    threshold = 0;//set up threshold with starting value 0
    kmalloc_initialized = true;//tell the rest that kmalloc is initialized

    change_heap_size(initial_heap_size);//change the heap size to what is specified as initial size
    *((uint32_t*)heap_start) = 0;
}