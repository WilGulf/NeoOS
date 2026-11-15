#ifndef MULTIHEAP_H
#define MULTIHEAP_H

#include "heap.h"

#include "../include/stdint.h"

enum {
    MULTIHEAP_HEAP_FLAG_EXTERNALLY_OWNED = 0b00000001,
    MULTIHEAP_HEAP_FLAG_DEFRAGMENT_WITH_PAGING = 0b00000010
};

struct multiheap_single_heap {
    struct heap *heap;
    struct heap *paging_heap;
    int flags;
    struct multiheap_single_heap *next;
};

enum {
    MULTIHEAP_FLAG_IS_READY,
};

struct multiheap {
    struct heap *starting_heap;
    struct multiheap_single_heap *first_multiheap;
    size_t total_heaps;
    void *max_end_data_addr;
    int flags;
};

int multiheap_ready(struct multiheap *multiheap);
struct multiheap *multiheap_new(struct heap *starting_heap);
void multiheap_free_heap(struct multiheap *multiheap);
int multiheap_add_existing_heap(struct multiheap *multiheap, struct heap *heap, int flags);
int multiheap_add(struct multiheap *multiheap, void *saddr, void *eaddr, int flags);

void *multiheap_alloc(struct multiheap *multiheap, size_t size);
void *multiheap_palloc(struct multiheap *multiheap, size_t size);
void multiheap_free(struct multiheap *multiheap, void *ptr);

#endif