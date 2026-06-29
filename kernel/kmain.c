#include "drivers/io/io.h"
#include "gdt/gdt.h"

int kmain() {
    gdt_init();

    fb_clear();
    kprintf("GDT Initialized\n");

    kprintf("Kprint %s%c %d %f\n", "Tes", 't', 12345, 124.121546);

    while (1) {
        
    }

    return 0;
}