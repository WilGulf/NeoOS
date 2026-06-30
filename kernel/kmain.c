#include "drivers/io/io.h"
#include "gdt/gdt.h"
#include "idt/idt.h"

int kmain() {
    fb_clear();
    
    gdt_init();
    idt_init();

    kprintf("Kprint %s%c %d %f\n", "Tes", 't', 1234, 124.121546);
    //kprintf("Divide %d", 1/0);

    while (1) {
        
    }

    return 0;
}