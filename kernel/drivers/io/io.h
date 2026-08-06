#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
static inline void insl(int port, void *addr, int cnt);
static inline void outsl(int port, const void *addr, int cnt);

int writer(char *buf);
int fb_clear();
void fb_scroll();

int kprintf(char *buf, ...);

#endif