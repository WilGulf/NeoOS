#ifndef UTIL_H
#define UTIL_H

#include "stdint.h"

#define CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

void memset(void *dest, char val, uint32_t count);
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

char *strcpy(char *dest, const char *src);
int strlen(char *src);
int strcmp(char *str1, char *str2);

int pow(int x, int power);

#endif