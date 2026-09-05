#include "io.h"
#include "../../include/stdint.h"
#include "../../include/util.h"

char *fb = (char *) 0x000B8000;

unsigned char current_fg = 0x0F;
unsigned char current_bg = 0x00;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = (bg << 4) | (fg & 0x0F);
}

void fb_move_cursor(unsigned short pos) {
    outb(0x3D4, 14);
    outb(0x3D5, ((pos >> 8) & 0x00FF));
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0x00FF);
}

uint16_t fb_get_cursor_position(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;

    return pos;
}

void fb_enable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x0F);

    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x0F);
}


void fb_disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void fb_clear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        fb_write_cell(i, ' ', 0x0F, 0x00);
    }

    fb_move_cursor(0);
}

void fb_backspace() {
    uint16_t pos = fb_get_cursor_position();

    fb_write_cell((pos - 1) * 2, ' ', 0x0F, 0x00);
    fb_move_cursor(pos - 1);
}

void fb_new_line() {
    uint16_t pos = fb_get_cursor_position();

    int y = pos / 80;

    int i = y;
    while (i < 24) {
        fb_scroll();
        i++;
    }
    
    pos = 23 * 80;
    
    fb_move_cursor(pos);
}

void fb_scroll() {
    uint16_t pos = fb_get_cursor_position();
    int y = pos / 80;

    int i = 0 * 80;
    for (; i < 24 * 80 * 2; i++) {
        fb[i] = fb[i + 160];
    }

    for (i = 24 * 80 * 2; i < 25 * 80; i += 2) {
        fb[i] = ' ';
        fb[i + 1] = 0x0F;
    }
}

void process_coloring(int number) {
    switch (number) {
        case 0:
            current_fg = 0x0F;
            current_bg = 0x00;
            break;
        case 30:
            // Black
            current_fg = 0x00;
            break;
        case 31:
            // Red
            current_fg = 0x04;
            break;
        case 32:
            // Green
            current_fg = 0x02;
            break;
        case 33:
            // Yellow
            current_fg = 0x0E;
            break;
        case 34:
            // Blue
            current_fg = 0x01;
            break;
        case 35:
            // Purple
            current_fg = 0x05;
            break;
        case 36:
            // Cyan
            current_fg = 0x03;
            break;
        case 37:
            // White
            current_fg = 0x0F;
            break;
        case 39:
            current_fg = 0x0F;
            break;

        case 40:
            // Black
            current_bg = 0x00;
            break;
        case 41:
            // Red
            current_bg = 0x04;
            break;
        case 42:
            // Green
            current_bg = 0x02;
            break;
        case 43:
            // Yellow
            current_bg = 0x0E;
            break;
        case 44:
            // Blue
            current_bg = 0x01;
            break;
        case 45:
            // Purple
            current_bg = 0x05;
            break;
        case 46:
            // Cyan
            current_bg = 0x03;
            break;
        case 47:
            // White
            current_bg = 0x0F;
            break;
        case 49:
            current_bg = 0x00;
            break;
        default:
            break;
    }
}

int process_esc(uint8_t *params, int n, char type) {
    switch (type) {
        case 'm': {
            for (int i = 0; i < n; i++) {
                process_coloring(params[i]);
            }
            
            break;
        }

        case 'H': {
            if (!n) {
                fb_move_cursor(0);
            } else if (n >= 2) {
                uint16_t y = params[0] * 80;
                uint16_t x = params[1];
                fb_move_cursor(y + x);
            }

            break;
        }

        case 'A': {
            uint16_t pos = fb_get_cursor_position();
            fb_move_cursor(pos - (80 * params[0]));
            break;
        }

        case 'B': {
            uint16_t pos = fb_get_cursor_position();
            fb_move_cursor(pos + (80 * params[0]));
            break;
        }

        case 'C': {
            uint16_t pos = fb_get_cursor_position();
            fb_move_cursor(pos + params[0]);
            break;
        }

        case 'D': {
            uint16_t pos = fb_get_cursor_position();
            fb_move_cursor(pos - params[0]);
            break;
        }

        case 'E': {
            fb_move_cursor((uint16_t)params[0] * 80);
            break;
        }

        case 'J': {
            if (params[0] == 2) {
                fb_clear();
            }

            break;
        }

        case 'c': {
            switch (params[0]) {
                case 0:
                    fb_disable_cursor();
                    break;
                case 1:
                    fb_enable_cursor();
                    break;
            }
        }
    }
}

void fb_putc(char c) {
    if (c == '\n' || c == 0x0d) {
        fb_new_line();
    } else if (c == 0x08) {
        fb_backspace();
    } else {
        uint16_t pos = fb_get_cursor_position();
        fb_write_cell((pos * 2), c, current_fg, current_bg);
        fb_move_cursor(pos + 1);
    }
}

int writer(char *buf) {

    uint16_t pos = fb_get_cursor_position();

    int x = pos % 80;
    int y = pos / 80;

    pos = x + y * 80;

    unsigned char *bytes = (unsigned char *)buf;

    int i = 0;
    while (bytes[i] != 0) {
        if (bytes[i] == '\n') {

            fb_move_cursor(pos);
            fb_new_line();

            pos = fb_get_cursor_position();

            int x = pos % 80;
            int y = pos / 80;

            pos = x + y * 80;
            
            i++;
        } else if (bytes[i] == '\0') {
            break;
        } else if (bytes[i] == '\033') {
            if (bytes[i + 1] == '[') {
                uint8_t params[8];
                int total_params = 0;
                char param_type = 0x00;

                i += 2;
                int last = i;
                while (bytes[i]) {
                    if (bytes[i] == ';' || !char_is_digit(bytes[i])) {
                        int len = i - last + 1;
                        char formatting[8];
                        strncpy(formatting, bytes + last, len < 7 ? len : 7);
                        formatting[len < 7 ? len : 7] = 0x00;

                        params[total_params] = atoi(formatting);
                        total_params++;

                        last = i + 1;

                        if (bytes[i] != ';' || total_params >= 7) {
                            if (total_params) {
                                param_type = bytes[i];
                                process_esc(params, total_params, param_type);
                            }

                            i++;
                            break;
                        }
                    }

                    i++;
                }
            }
        } else {
            fb_write_cell(pos * 2, bytes[i], current_fg, current_bg);
            pos++;
            i++;
        }
    }

    fb_move_cursor(pos);

    return 0;
}