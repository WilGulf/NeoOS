#include "fb.h"
#include "color.h"
#include "../graphics/graphics.h"

#include "../../../include/status.h"
#include "../../../include/stdint.h"
#include "../../../include/util.h"

struct fb_pixel curr_fg;
struct fb_pixel curr_bg;

int cols = 0;
int rows = 0;

int pos_x = 0;
int pos_y = 0;

uint32_t font_h = 0;

int console_init(void) {
    uint32_t res_h = get_resolution_height();
    uint32_t res_w = get_resolution_width();

    font_h = get_font_height();
    uint32_t font_w = get_font_width();

    if (!res_h || !res_w || !font_h || !font_w) {
        return -ERROR_IO;
    }

    cols = res_w / font_w;
    rows = res_h / font_h;

    curr_fg = color_white;
    curr_bg = color_black;

    return 0;
}

void console_clear(kbool refresh) {
    graphics_reset_framebuffer();
    
    if (refresh) {
        graphics_refresh();
    }
}

void console_scroll(kbool refresh) {
    graphics_scroll(font_h);
    
    if (refresh) {
        graphics_refresh();
    }
}

void console_new_line(kbool refresh) {
    pos_x = 0;
    pos_y++;

    if (pos_y >= rows) {
        console_scroll(refresh);
        pos_y = rows - 1;
    }
}

void console_backspace(kbool refresh) {
    pos_x--;
    fb_write_cell(' ', pos_x, pos_y, false, curr_fg, curr_bg);

    if (pos_x < 0) {
        if (pos_y > 0) {
            pos_y--;
        }
        
        pos_x = 0;
    }

    if (refresh) {
        graphics_refresh();
    }
}

void console_putc(char c, kbool refresh) {
    if (c == '\n') {
        console_new_line(refresh);
        return;
    }

    if (c == 0x08) {
        console_backspace(refresh);
        return;
    }

    fb_write_cell(c, pos_x++, pos_y, false, curr_fg, curr_bg);
    if (pos_x >= cols) {
        console_scroll(false);
        pos_y++;
        pos_x = 0;
    }

    if (refresh) {
        graphics_refresh();
    }
}

void process_coloring(int number) {
    switch (number) {
        case 0:
            // Reset
            curr_fg = color_white;
            curr_bg = color_black;
            break;
        case 30:
            // Black
            curr_fg = color_black;
            break;
        case 31:
            // Red
            curr_fg = color_red;
            break;
        case 32:
            // Green
            curr_fg = color_green;
            break;
        case 33:
            // Yellow
            curr_fg = color_yellow;
            break;
        case 34:
            // Blue
            curr_fg = color_blue;
            break;
        case 35:
            // Magenta
            curr_fg = color_magenta;
            break;
        case 36:
            // Cyan
            curr_fg = color_cyan;
            break;
        case 37:
            // White
            curr_fg = color_white;
            break;
        case 39:
            // Reset
            curr_fg = color_white;
            break;

        case 40:
            // Black
            curr_bg = color_black;
            break;
        case 41:
            // Red
            curr_bg = color_red;
            break;
        case 42:
            // Green
            curr_bg = color_green;
            break;
        case 43:
            // Yellow
            curr_bg = color_yellow;
            break;
        case 44:
            // Blue
            curr_bg = color_blue;
            break;
        case 45:
            // Magenta
            curr_bg = color_magenta;
            break;
        case 46:
            // Cyan
            curr_bg = color_cyan;
            break;
        case 47:
            // White
            curr_bg = color_white;
            break;
        case 49:
            // Reset
            curr_bg = color_black;
            break;
        default:
            break;
    }
}

void process_esc(uint8_t *params, int n, char type) {
    switch (type) {
        case 'm': {
            for (int i = 0; i < n; i++) {
                process_coloring(params[i]);
            }
            
            break;
        }

        case 'H': {
            if (!n) {
                //fb_move_cursor(0);
            } else if (n >= 2) {
                //uint16_t y = params[0] * 80;
                //uint16_t x = params[1];
                //fb_move_cursor(y + x);
            }

            break;
        }

        case 'A': {
            //fb_move_cursor(pos - (80 * params[0]));
            break;
        }

        case 'B': {
            //fb_move_cursor(pos + (80 * params[0]));
            break;
        }

        case 'C': {
            //fb_move_cursor(pos + params[0]);
            break;
        }

        case 'D': {
            //fb_move_cursor(pos - params[0]);
            break;
        }

        case 'E': {
            //fb_move_cursor((uint16_t)params[0] * 80);
            break;
        }

        case 'J': {
            if (params[0] == 2) {
                console_clear(false);
            }

            break;
        }

        case 'c': {
            switch (params[0]) {
                case 0:
                    //fb_disable_cursor();
                    break;
                case 1:
                    //fb_enable_cursor();
                    break;
            }
        }
    }
}

int console_writer(char *buf) {
    unsigned char *bytes = (unsigned char *)buf;

    int i = 0;
    while (bytes[i] != 0) {
        if (bytes[i] == '\n') {
            console_putc('\n', false);
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
                        int len = i - last;
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
            console_putc(bytes[i], false);
            i++;
        }
    }

    graphics_refresh();

    return 0;
}