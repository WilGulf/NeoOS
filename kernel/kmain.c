#include "drivers/io/io.h"

int kmain() {

    fb_clear();
    writer("hello", 5);

    while (1) {
        
    }

    return 0;
}