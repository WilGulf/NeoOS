#include "drivers/io/io.h"

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    char *fb = (char *) 0x000B8000;
    fb[i] = c;
    fb[i + 1] = (bg << 4) | (fg & 0x0F);
}

void fb_move_cursor(unsigned short pos) {
    outb(0x3D4, 14);
    outb(0x3D5, ((pos >> 8) & 0x00FF));
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0x00FF);
}

int kmain() {

    while (1) {
        
    }

    return 0;
}