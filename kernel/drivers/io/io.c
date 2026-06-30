#include "io.h"
#include "../../include/stdint.h"

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

    return pos;
}

void fb_new_line() {
    uint16_t pos = fb_get_cursor_position();

    int y = pos / 80;
    pos = y * 80 + 80;
    
    fb_move_cursor(pos);
}

int writer(char *buf) {

    uint16_t pos = fb_get_cursor_position();

    int x = pos % 80;
    int y = pos / 80;

    pos = x + y * 80;

    unsigned char *bytes = (unsigned char *)buf;

    int i = 0;
    while (bytes[i] != 0) {
        if (bytes[i] == 0x0A) {

            fb_move_cursor(pos);
            fb_new_line();

            pos = fb_get_cursor_position();

            int x = pos % 80;
            int y = pos / 80;

            pos = x + y * 80;
            
            break;
        } else {
            fb_write_cell(pos * 2, bytes[i], 0x0F, 0x00);
            pos++;
            i++;
        }
    }

    fb_move_cursor(pos);

    return 0;
}