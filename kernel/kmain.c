#include "drivers/io/io.h"
#include "gdt/gdt.h"

int kmain() {
    gdt_init();

    fb_clear();
    writer("GDT Initialized\n");

    while (1) {
        
    }

    return 0;
}