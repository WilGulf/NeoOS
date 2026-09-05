#include "stdio.h"

#include <stdarg.h>
#include "string.h"

extern void print(const char *msg);

int printf(const char *fmt, ...) {
    va_list args;
    const char *p;
    char *sval;
    int cval, ival;

    char buffer[1024];
    int buffer_i = 0;

    va_start(args, fmt);
    for (p = fmt; *p; p++) {
        if (buffer_i >= 1022) {
            break;
        }

        if (*p != '%') {
            buffer[buffer_i++] = *p;
            continue;
        }

        char *integer;

        switch(*++p) {
            case 'd':
                ival = va_arg(args, int);
                integer = itoa(ival);
                while (*integer && buffer_i < 1022) {
                    buffer[buffer_i++] = *integer++;
                }
                
                break;

            case 's':
                sval = va_arg(args, char *);
                while (*sval && buffer_i < 1022) {
                    buffer[buffer_i++] = *sval++;
                }

                break;

            case 'c':
                cval = va_arg(args, int);
                buffer[buffer_i++] = (char)cval;
                break;

            case 'f':
                break;

            case 'x':
                break;

            default:
                buffer[buffer_i++] = *p;
                break;
        }
    }

    buffer[buffer_i] = '\0';

    print(buffer);

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

int atoi(const char *str) {
    int res = 0;
    int i = 0;

    while (str[i] != '\0') {
        res = res * 10 + (str[i] - '0');
        i++;
    }

    return res;
}

char *fgets(char *str, int size, int fd) {
    if (!str || size <= 1) {
        return 0;
    }

    int i = 0;
    while (i < size - 1) {
        char c;
        
        if (fread(&c, 1, 1, fd) != 1) {
            if (i == 0) {
                return 0;
            }

            break;
        }

        str[i++] = c;

        if (c == '\n') {
            break;
        }
    }

    str[i] = '\0';
    return str;
}