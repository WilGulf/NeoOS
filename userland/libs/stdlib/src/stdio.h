#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>

#define MODIFIER_SHIFT 0B00000001
#define MODIFIER_CTRL  0B00000010
#define MODIFIER_ALT   0B00000100

struct key_event {
    char c;
    uint8_t modifiers;
};

int printf(const char *fmt, ...);
void putchar(char c);
int getkey();
void clear();
void getkey_event(struct key_event *out);

char *itoa(int i);
int atoi(const char *str);

#endif