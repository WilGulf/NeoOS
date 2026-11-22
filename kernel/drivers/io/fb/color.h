#ifndef COLOR_H
#define COLOR_H

#include "../graphics/graphics.h"

#define RGB_INIT(r, g, b) { .red = (r), .green = (g), .blue = (b), .reserved = 0 }

#define C_BLACK RGB_INIT(0, 0, 0)
#define C_BLUE RGB_INIT(0, 0, 170)
#define C_GREEN RGB_INIT(0, 170, 0)
#define C_CYAN RGB_INIT(0, 170, 170)
#define C_RED RGB_INIT(170, 0, 0)
#define C_MAGENTA RGB_INIT(170, 0, 170)
#define C_BROWN RGB_INIT(170, 85, 0)
#define C_LIGHT_GRAY RGB_INIT(170, 170, 170)
#define C_DARK_GRAY RGB_INIT(85, 85, 85)
#define C_LIGHT_BLUE RGB_INIT(85, 85, 255)
#define C_LIGHT_GREEN RGB_INIT(85, 255, 85)
#define C_LIGHT_CYAN RGB_INIT(85, 255, 255)
#define C_LIGHT_RED RGB_INIT(255, 85, 85)
#define C_LIGHT_MAGENTA RGB_INIT(255, 85, 255)
#define C_YELLOW RGB_INIT(255, 255, 85)
#define C_WHITE RGB_INIT(255, 255, 255)

static const struct fb_pixel color_black = C_BLACK;
static const struct fb_pixel color_blue = C_BLUE;
static const struct fb_pixel color_green = C_GREEN;
static const struct fb_pixel color_cyan = C_CYAN;
static const struct fb_pixel color_red = C_RED;
static const struct fb_pixel color_magenta = C_MAGENTA;
static const struct fb_pixel color_brown = C_BROWN;
static const struct fb_pixel color_ligth_gray = C_LIGHT_GRAY;
static const struct fb_pixel color_dark_gray = C_DARK_GRAY;
static const struct fb_pixel color_light_blue = C_LIGHT_BLUE;
static const struct fb_pixel color_light_green = C_LIGHT_GREEN;
static const struct fb_pixel color_light_cyan = C_LIGHT_CYAN;
static const struct fb_pixel color_light_red = C_LIGHT_RED;
static const struct fb_pixel color_light_magenta = C_LIGHT_MAGENTA;
static const struct fb_pixel color_yellow = C_YELLOW;
static const struct fb_pixel color_white = C_WHITE;

#define DEFAULT_FG color_white
#define DEFAULT_BG color_black

#endif