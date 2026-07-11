#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);

int writer(char *buf);
int fb_clear();
void fb_scroll();

int kprintf(char *buf, ...);

#endif