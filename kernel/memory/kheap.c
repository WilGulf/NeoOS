#include "kheap.h"
#include "heap.h"

#include "../drivers/io/io.h"
#include "../include/stdint.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init() {
    int total_table_entries = KHEAP_SIZE_BYTES / KHEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY *)(KHEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void *end = (void *)(KHEAP_ADDRESS + KHEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *)(KHEAP_ADDRESS), end, &kernel_heap_table);
    if (res < 0) {
        kprintf("Failed to create kheap\n");
    }
}

void *kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void kfree(void *ptr) {
    heap_free(&kernel_heap, ptr);
}