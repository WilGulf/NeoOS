#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <va_list.h>

#define MODIFIER_SHIFT 0B00000001
#define MODIFIER_CTRL  0B00000010
#define MODIFIER_ALT   0B00000100

#define MAX_PATH 108

struct key_event {
    char c;
    uint8_t modifiers;
};

typedef unsigned int FILE_STAT_FLAGS;

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

typedef unsigned int FILE_SEEK_MODE;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

char *itoa(int i);
int atoi(const char *str);

int printf(const char *fmt, ...);
int snprintf(char *out, size_t len, char *fmt, ...);
int sprintf(char *out, char *fmt, ...);
int vsprintf(char *out, const char *fmt, va_list arg_ptr);

void putchar(char c);
int getkey();
void clear();
void getkey_event(struct key_event *out);

int fopen(const char *filename, const char *mode);
void fclose(int fd);
int fread(void *buffer, size_t size, size_t count, int fd);
int fstat(int fd, struct file_stat *stat);
size_t fwrite(const void *in, size_t size, size_t nmemb, int fd);
int remove(const char *filename);
int create(const char *filename);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);

char *fgets(char *str, int size, int fd);

void set_fb_cursor(bool enabled);

#endif