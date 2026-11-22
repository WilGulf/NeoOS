#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "../../include/stdint.h"
#include "../../include/va_list.h"

void outb(uint16_t port, uint8_t data);
void outw(uint16_t port, uint16_t data);
void outl(uint16_t port, uint32_t data);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint16_t inl(uint16_t port);

void putc(char c);

int klogf(const char *fmt, ...);
int kprintf(const char *fmt, ...);
int snprintf(char *out, size_t len, char *fmt, ...);
int sprintf(char *out, char *fmt, ...);
int vsprintf(char *out, const char *fmt, va_list arg_ptr);

#endif