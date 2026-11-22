#ifndef FB_H
#define FB_H

#include "../../../include/stdint.h"
#include "../graphics/graphics.h"

uint32_t get_font_width(void);
uint32_t get_font_height(void);

int fb_init(uint8_t *font);
int fb_write_char(char c, uint32_t x, uint32_t y, kbool refresh, struct fb_pixel fg, struct fb_pixel bg);
int fb_write_cell(char c, uint32_t x, uint32_t y, kbool refresh, struct fb_pixel fg, struct fb_pixel bg);

#endif