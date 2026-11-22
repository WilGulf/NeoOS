#include "graphics.h"

#include "../../../panic.h"
#include "../../../kernel.h"
#include "../../../memory/kheap.h"
#include "../../../memory/paging.h"
#include "../../../include/util.h"


struct graphics_info *loaded_graphics_info = NULL;

void *framebuffer = NULL;
void *framebuffer_end = NULL;
size_t framebuffer_width = 0;
size_t framebuffer_height = 0;
size_t framebuffer_pixels_per_scanline = 0;
size_t framebuffer_size = 0;

size_t get_resolution_width(void) {
    return framebuffer_width;
}

size_t get_resolution_height(void) {
    return framebuffer_height;
}

void graphics_refresh(void) {
    memcpy(loaded_graphics_info->framebuffer, loaded_graphics_info->pixels, framebuffer_size);
}

void graphics_draw_pixel(uint32_t x, uint32_t y, struct fb_pixel pixel) {
    if (x < loaded_graphics_info->horizontal_resolution && y < loaded_graphics_info->vertical_resolution) {
        loaded_graphics_info->pixels[y * loaded_graphics_info->pixels_per_scanline + x] = pixel;
    }
}

void graphics_reset_framebuffer(void) {
    memset(loaded_graphics_info->pixels, 0, framebuffer_size);
}

void graphics_init(struct graphics_info *graphics_info) {
    if (loaded_graphics_info) {
        panic("", 1);
    }

    framebuffer = graphics_info->framebuffer;
    framebuffer_width = graphics_info->horizontal_resolution;
    framebuffer_height = graphics_info->vertical_resolution;
    framebuffer_pixels_per_scanline = graphics_info->pixels_per_scanline;
    framebuffer_size = framebuffer_height * framebuffer_pixels_per_scanline * sizeof(struct fb_pixel);
    framebuffer_end = (void *)(uintptr_t)framebuffer + framebuffer_size;

    void *new_framebuffer_memory = kzalloc(framebuffer_size);
    graphics_info->framebuffer = new_framebuffer_memory;
    graphics_info->pixels = kzalloc(framebuffer_size);

    paging_map_to(kernel_descriptor(), new_framebuffer_memory, framebuffer, framebuffer_end, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT);
    loaded_graphics_info = graphics_info;
    graphics_refresh();
}