#ifndef PSF_H
#define PSF_H

#include "../../../../include/stdint.h"

extern char _binary_spleen_12x24_psfu_start[];
extern char _binary_spleen_16x32_psfu_start[];
extern char _binary_spleen_32x64_psfu_start[];

static const uint8_t *spleen_12x24 = (uint8_t *)&_binary_spleen_12x24_psfu_start;
static const uint8_t *spleen_16x32 = (uint8_t *)&_binary_spleen_16x32_psfu_start;
static const uint8_t *spleen_32x64 = (uint8_t *)&_binary_spleen_32x64_psfu_start;

#define PSF_MAGIC 0x864AB572U

struct psf_font_header {
    uint32_t magic;

    uint32_t version;
    uint32_t header_size;
    
    uint32_t flags;

    uint32_t num_glyphs;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} __attribute__((packed));

struct psf_font {
    int bytes_per_row;
    uint8_t *font;
    struct psf_font_header *header;
};

#endif