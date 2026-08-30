#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>

#define MODIFIER_SHIFT 0B00000001
#define MODIFIER_CTRL  0B00000010
#define MODIFIER_ALT   0B00000100

struct key_event {
    char c;
    uint8_t modifiers;
};

typedef unsigned int FILE_STAT_FLAGS;

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

char *itoa(int i);
int atoi(const char *str);

int printf(const char *fmt, ...);
void putchar(char c);
int getkey();
void clear();
void getkey_event(struct key_event *out);

int fopen(const char *filename, const char *mode);
void fclose(int fd);
int fread(void *buffer, size_t size, size_t count, int fd);
int fstat(int fd, struct file_stat *stat);

char *fgets(char *str, int size, int fd);

void set_fb_cursor(bool enabled);

#endif