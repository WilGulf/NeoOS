#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "../../include/stdint.h"

void outb(uint16_t port, uint8_t data);
void outw(uint16_t port, uint16_t data);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

int writer(char *buf);
void fb_putc(char c);
void fb_clear();
void fb_scroll();

void fb_enable_cursor();
void fb_disable_cursor();

int kprintf(const char *fmt, ...);

#endif