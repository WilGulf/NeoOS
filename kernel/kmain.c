#include "drivers/io/io.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "multiboot.h"
#include "memory/memory.h"
#include "include/stdint.h"

int kmain(uint32_t magic, struct multiboot_info* bootInfo) {
    fb_clear();
    
    gdt_init();
    idt_init();

    kprintf("Kprint %s%c %d %f\n", "Tes", 't', 1234, 124.121546);
    //kprintf("Divide %d", 1/0);

    uint32_t mod1 = *(uint32_t*)(bootInfo->mods_addr + 4);
    uint32_t physicalAllocStart = (mod1 + 0xFFF) & ~0xFFF;

    memory_init(bootInfo->mem_upper * 1024, physicalAllocStart);

    kprintf("Memory Allocation Done");

    while (1) {
        
    }

    return 0;
}