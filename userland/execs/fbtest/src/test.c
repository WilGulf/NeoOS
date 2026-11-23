#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

#include <stdbool.h>

void declarations(void) {
    promise(PROMISE_FB | PROMISE_INPUT | PROMISE_FS);
}

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

int main(int argc, char **argv) {
    int data = fopen("D:/data/fb0", "r");
    if (data < 0) {
        printf("Failed to open framebuffer data\n");
        exit();
    }

    struct graphics_info info;
    int got = fread(&info, 1, sizeof(info), data);
    fclose(data);

    if (got != (int)sizeof(info) || !info.pixels_per_scanline || !info.vertical_resolution) {
        printf("Bad framebuffer data (read %d)\n", got);
        exit();
    }

    int fb = fopen("D:/func/fb0", "w");
    if (fb <= 0) {
        printf("Failed to open framebuffer\n");
        exit();
    }

    int ctl = fopen("D:/func/fb0ctl", "w");
    if (ctl <= 0) {
        printf("Failed to open framebuffer ctl\n");
        exit();
    }

    uint32_t row_bytes = info.pixels_per_scanline * sizeof(struct fb_pixel);
    struct fb_pixel *row = malloc(row_bytes);
    if (!row) {
        printf("Out of memory\n");
        fclose(fb);
        exit();
    }

    struct fb_pixel red = {0, 0, 255, 0};
    for (uint32_t x = 0; x < info.pixels_per_scanline; x++) {
        row[x] = red;
    }

    for (uint32_t y = 0; y < info.vertical_resolution; y++) {
        fseek(fb, (uint64_t)y * row_bytes, SEEK_SET);
        int wrote = fwrite(row, 1, row_bytes, fb);
        if (wrote != (int)row_bytes) {
            printf("Short write at row %d (%d of %d)\n", (int)y, wrote, (int)row_bytes);
            break;
        }
    }

    if (fwrite("FLUSH", 5, 1, ctl) != 5) {
        printf("Command failed\n");
        exit();
    }

    struct key_event event = {0, 0};
    while (1) {
        getkey_event(&event);
        if (event.c != 0) {
            break;
        }
    }

    fclose(fb);
    free(row);
    exit();
}