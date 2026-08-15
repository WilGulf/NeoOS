#include "stdio.h"

#include <stdarg.h>

void print(const char *msg);

int printf(const char *fmt, ...) {
    va_list args;
    const char *p;
    char *sval, *cval;
    int ival;

    va_start(args, fmt);
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch(*++p) {
            case 'd':
                ival = va_arg(args, int);
                print(itoa(ival));
                break;

            case 's':
                sval = va_arg(args, char *);
                print(sval);
                break;

            case 'c':
                cval = va_arg(args, char *);
                print(cval);
                break;

            case 'f':
                break;

            case 'x':
                break;

            default:
                putchar(*p);
                break;
        }
    }

    va_end(args);

    return 0;
}

char *itoa(int i) {
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = -1;
    if (i >= 0) {
        neg = 0;
        i = -i;
    }

    while (i) {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11) {
        text[--loc] = '0';
    }

    if (neg) {
        text[--loc] = '-';
    }

    return &text[loc];
}