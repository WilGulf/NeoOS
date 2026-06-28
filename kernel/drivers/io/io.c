#include "io.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

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

int fb_clear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        fb_write_cell(i, ' ', 0x0F, 0x00);
    }

    fb_move_cursor(0);

    return 0;
}

uint16_t fb_get_cursor_position(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;

    int x = pos % 80;
    int y = pos / 80;

    pos = x + y;

    return pos;
}

int writer(char *buf, unsigned int len) {

    uint16_t pos = fb_get_cursor_position();

    unsigned char *bytes = (unsigned char *)buf;

    for (unsigned int i = 0; i < len; i++) {
        fb_write_cell(pos * 2, bytes[i], 0x0F, 0x00);
        pos++;
    }

    fb_move_cursor(pos);

    return 0;
}