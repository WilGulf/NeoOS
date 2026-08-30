#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

void outb(unsigned short port, unsigned char data);
void outw(unsigned short port, unsigned short data);

unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);

int writer(char *buf);
void fb_putc(char c);
void fb_clear();
void fb_scroll();

void fb_enable_cursor();
void fb_disable_cursor();

int kprintf(char *buf, ...);

#endif