#ifndef STRING_H
#define STRING_H

#include <stddef.h>

static inline size_t strlen(const char *s) {
    size_t n = 0;
    
    while (s[n]) {
        n++;
    }

    return n;
}

static inline char *strchr(const char *s, int c) {
    for (; *s; s++) {
        if (*s == (char)c) {
            return (char *)s;
        }
    }

    return NULL;
}

static inline char *strcpy(char *d, const char *s) {
    char *r = d;
    while ((*d++ = *s++)) {}
    return r;
}
static inline char *strncpy(char *d, const char *s, size_t n) {
    size_t i = 0;
    for (; i < n && s[i]; i++) d[i] = s[i];
    for (; i < n; i++) d[i] = 0;
    return d;
}
static inline int strncmp(const char *a, const char *b, size_t n) {
    for (; n; n--, a++, b++) {
        if (*a != *b) return (unsigned char)*a - (unsigned char)*b;
        if (!*a) break;
    }
    return 0;
}

static inline void *memcpy(void *d, const void *s, size_t n) {
    volatile unsigned char *dp = d;
    const volatile unsigned char *sp = s;
    while (n--) *dp++ = *sp++;
    return d;
}

#endif