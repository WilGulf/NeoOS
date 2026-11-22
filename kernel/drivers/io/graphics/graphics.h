#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../../../include/stdint.h"

struct fb_pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

struct graphics_info {
    struct fb_pixel *framebuffer;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t pixels_per_scanline;

    struct fb_pixel *pixels;
};

void graphics_init(struct graphics_info *graphics_info);

size_t get_resolution_width(void);
size_t get_resolution_height(void);

void graphics_refresh(void);

void graphics_reset_framebuffer(void);
void graphics_scroll(uint32_t vertical_pixels);
void graphics_draw_pixel(uint32_t x, uint32_t y, struct fb_pixel pixel);

#endif