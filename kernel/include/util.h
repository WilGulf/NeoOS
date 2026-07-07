#ifndef UTIL_H
#define UTIL_H

#include "stdint.h"

#define CEIL_DIV(a, b) (((a) + (b) - 1) / (b))

void memset(void *dest, char val, uint32_t count);
int pow(int x, int power);

#endif