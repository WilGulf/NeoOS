#ifndef UTIL_H
#define UTIL_H

#include "stdint.h"

#define CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

void *memset(void *ptr, int c, size_t size);
void *memcpy(void *dest, void *src, uint32_t len);
int memcmp(void* s1, void* s2, int count);

char *strcpy(char *dest, const char *src);
int strlen(char *src);
int strnlen(char *src, int max);
int strcmp(char *str1, char *str2);
int strncmp(char *str1, char *str2, int max);
int istrncmp(const char* s1, const char* s2, int n);

kbool char_is_digit(char c);
int char_to_int(char c);

char char_upper_to_lower(char s1);

int pow(int x, int power);

#endif