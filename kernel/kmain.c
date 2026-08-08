#include "include/stdint.h"

#include "multiboot.h"
#include "drivers/io/io.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "memory/paging.h"
#include "memory/kheap.h"
#include "memory/heap.h"
#include "drivers/fs/disk.h"
#include "drivers/fs/path_parser.h"
#include "include/util.h"
#include "include/va_list.h"
#include "syscall/syscall.h"

extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;

static struct paging_4gb_chunk *kernel_chunk = 0;

int kmain(uint32_t magic, struct multiboot_info* bootInfo) {
    fb_clear();

    kheap_init();
    kprintf("Inititalized kernel heap\n");

    disk_search_and_init();

    gdt_init();
    idt_init();

    void *ptr = kmalloc(50);
    kprintf("ptr: %d\n", ptr);
    void *ptr2 = kmalloc(5000);
    void *ptr3 = kmalloc(5600);
    kfree(ptr);
    void *ptr4 = kmalloc(50);

    kprintf("ptr2: %d\n", ptr2);
    kprintf("ptr3: %d\n", ptr3);
    kprintf("ptr4: %d\n", ptr4);

    kprintf("Kprintf %s%c %d %f\n", "Tes", 't', 1234, 124.121546);

    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    kprintf("Paging new\n");
    
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    kprintf("Paging switch\n");
    kprintf("new");

    enable_paging();
    kprintf("Paging enabled\n");

    enable_interrupts();
    kprintf("Interrupts enabled\n");

    fs_init();
    disk_search_and_init();

    int fd = fopen("0:/hello.txt", "r");
    kprintf("fd: %d\n", fd);
    if (true) {
        kprintf("Opened hello.txt\n");
        char buf[18];
        int res = fread(buf, 17, 1, fd);
        kprintf("data: %s\n", buf);
    }

    //kprintf("%d", 0/0);

    //uint32_t mod1 = *(uint32_t*)(bootInfo->mods_addr + 4);
    //uint32_t physicalAllocStart = (mod1 + 0xFFF) & ~0xFFF;

    //memory_init(bootInfo->mem_upper * 1024, 0x00400000);
    //kmalloc_init(0x1000);

    /*uint32_t user_code_phys = pmm_alloc_page_frame();
    mem_map_page(0x08048000 , user_code_phys, PAGE_FLAG_WRITE | PAGE_FLAG_USER);

    uint32_t user_stack_phys = pmm_alloc_page_frame();
    mem_map_page(0x08050000, user_stack_phys, PAGE_FLAG_WRITE | PAGE_FLAG_USER);

    uint8_t *user_code = (uint8_t*)0x08048000;
    user_code[0] = 0xEB; // jmp $
    user_code[1] = 0xFE;

    switch_to_usermode(0x08048000, 0x08050000 + 0x1000);*/

    while (1) {
        
    }

    return 0;
}