#include "graphics.h"

#include "../io.h"
#include "../../../panic.h"
#include "../../../kernel.h"
#include "../../../memory/kheap.h"
#include "../../../memory/paging.h"
#include "../../../include/util.h"
#include "../../fs/ramfs/ramfs.h"
#include "../../../syscalls/promise.h"


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

void graphics_scroll(uint32_t vertical_pixels) {
    if (!loaded_graphics_info) {
        return;
    }
    
    if (!vertical_pixels) {
        return;
    }

    if (vertical_pixels > framebuffer_height) {
        graphics_reset_framebuffer();
        return;
    }

    size_t row_size = framebuffer_pixels_per_scanline * sizeof(struct fb_pixel);

    memmove(
        loaded_graphics_info->pixels,
        (struct fb_pixel *)loaded_graphics_info->pixels + vertical_pixels * framebuffer_pixels_per_scanline,
        (framebuffer_height - vertical_pixels) * row_size
    );

    memset(
        (struct fb_pixel *)loaded_graphics_info->pixels + (framebuffer_height - vertical_pixels) * framebuffer_pixels_per_scanline,
        0,
        vertical_pixels * row_size
    );
}

int fb0_on_read(void *private, uint64_t offset, uint64_t size, uint64_t nmemb, char *out) {
    return 0;
}

int fb0_on_write(void *private, uint64_t offset, uint64_t size, uint64_t nmemb, const char *in) {
    (void)private;
    if (!loaded_graphics_info || offset >= framebuffer_size) {
        return 0;
    }

    size_t len = size * nmemb;
    if (len > framebuffer_size - offset) len = framebuffer_size - offset;

    memcpy((uint8_t *)loaded_graphics_info->pixels + offset, in, len);
    return len;
}

int fb0ctl_on_write(void *private, uint64_t offset, uint64_t size, uint64_t nmemb, const char *in) {
    (void)private;
    (void)offset;
    if (!loaded_graphics_info) {
        return 0;
    }

    char command[1024] = "";
    
    int to_read = 0;
    if ((size * nmemb) >= sizeof(command)) {
        to_read = sizeof(command) - 1;
    } else {
        to_read =(size * nmemb);
    }

    memcpy(command, in, to_read);

    int res = 0;

    kernel_page();

    if (strcmp(command, "FLUSH") == 0) {
        graphics_refresh();
        res = to_read;
        return res;
    }

    task_page();

out:
    return res;
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

    //SETUP FB0
    kernel_create_function_ramfile(
        "D:/func/fb0",
        NULL,
        framebuffer_pixels_per_scanline * framebuffer_height * sizeof(struct fb_pixel),
        fb0_on_read,
        fb0_on_write,
        6,
        PROMISE_FB,
        RAMFS_FLAG_W | RAMFS_FLAG_R
    );

    kernel_create_function_ramfile(
        "D:/func/fb0ctl",
        NULL,
        1024,
        NULL,
        fb0ctl_on_write,
        6,
        PROMISE_FB,
        RAMFS_FLAG_W
    );

    kernel_create_data_ramfile(
        "D:/data/fb0",
        1024,
        6,
        PROMISE_FB,
        RAMFS_FLAG_R
    );

    void *data = kernel_get_data_ramfile_data("D:/data/fb0", NULL);
    memcpy(data, loaded_graphics_info, sizeof(struct graphics_info));
}