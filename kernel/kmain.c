#include "drivers/io/io.h"
#include "gdt/gdt.h"

int kmain() {
    gdt_init();

    fb_clear();
    writer("GDT Initialized", 15);

    writer("hello", 5);

    while (1) {
        
    }

    return 0;
}