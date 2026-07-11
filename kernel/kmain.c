#include "include/stdint.h"

#include "multiboot.h"
#include "drivers/io/io.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "memory/kmalloc.h"
#include "include/util.h"

extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;


int kmain(uint32_t magic, struct multiboot_info* bootInfo) {
    fb_clear();
    
    gdt_init();
    idt_init();

    kprintf("kernel_virtual_start: %d\n", kernel_virtual_start);

    kprintf("Kprintf %s%c %d %f\n", "Tes", 't', 1234, 124.121546);

    uint32_t mod1 = *(uint32_t*)(bootInfo->mods_addr + 4);
    uint32_t physicalAllocStart = (mod1 + 0xFFF) & ~0xFFF;

    memory_init(bootInfo->mem_upper * 1024, 0x00400000);
    kmalloc_init(0x1000);
    kprintf("Memory Allocation Done\n");

    while (1) {
        
    }

    return 0;
}