#include "fb.h"

#include "../io.h"
#include "psf/psf.h"
#include "../graphics/graphics.h"
#include "../../../include/stdint.h"
#include "../../../include/status.h"
#include "../../../memory/kheap.h"

struct psf_font *loaded_font = NULL;

uint32_t get_font_width(void) {
    if (!loaded_font) {
        return 0;
    }

    return loaded_font->header->width;
}

uint32_t get_font_height(void) {
    if (!loaded_font) {
        return 0;
    }

    return loaded_font->header->height;
}

static void cell_to_pos(uint32_t inx, uint32_t iny, uint32_t *outx, uint32_t *outy) {
    if (!loaded_font) {
        return;
    }

    *outx = inx * loaded_font->header->width;
    *outy = iny * loaded_font->header->height;
}

int fb_write_char(char c, uint32_t x, uint32_t y, kbool refresh, struct fb_pixel fg, struct fb_pixel bg) {
    if (!loaded_font) {
        return -ERROR_IO;
    }

    uint8_t uc = (uint8_t)c;

    if (uc >= loaded_font->header->num_glyphs) {
        uc = '?';
    }

    uint8_t *glyph = loaded_font->font + (loaded_font->header->bytes_per_glyph * uc);
    for (uint32_t row = 0; row < loaded_font->header->height; row++) {
        uint8_t *row_p = glyph + (row * loaded_font->bytes_per_row);
        for (uint32_t col = 0; col < loaded_font->header->width; col++) {
            uint8_t byte = row_p[col / 8];
            int bit = 7 - (col % 8);
            if ((byte >> bit) & 1) {
                graphics_draw_pixel(x + col, y + row, fg);
            } else {
                graphics_draw_pixel(x + col, y + row, bg);
            }
        }
    }

    if (refresh) {
        graphics_refresh();
    }

    return 0;
}

int fb_write_cell(char c, uint32_t x, uint32_t y, kbool refresh, struct fb_pixel fg, struct fb_pixel bg) {
    uint32_t posx, posy;
    cell_to_pos(x, y, &posx, &posy);
    return fb_write_char(c, posx, posy, refresh, fg, bg);
}

int fb_init(uint8_t *font) {
    int res = 0;

    if (!font) {
        return -ERROR_INVALID_ARG;
    }

    if (loaded_font) {
        kfree(loaded_font);
        loaded_font = NULL;
    }

    loaded_font = kzalloc(sizeof(struct psf_font));
    if (!loaded_font) {
        res = -ERROR_NO_MEM;
        goto out;
    }

    loaded_font->header = (struct psf_font_header *)font;
    
    if (loaded_font->header->magic != PSF_MAGIC) {
        res = -ERROR_INVALID_FORMAT;
        goto out;
    }

    loaded_font->font = font + loaded_font->header->header_size;

    if (!loaded_font->header->num_glyphs) {
        res = -ERROR_INVALID_FORMAT;
        goto out;
    }

    loaded_font->bytes_per_row = (loaded_font->header->width + 7) / 8;

out:
    if (res < 0) {
        if (loaded_font) {
            kfree(loaded_font);
            loaded_font = NULL;
        }
    }
    return res;
}