#include "format.h"

#include "string.h"
#include "stdio.h"

#include <stdarg.h>

char *format(const char *fmt, va_list args) {
    const char *p;
    char *sval;
    int cval, ival;

    static char buffer[1024];
    int buffer_i = 0;

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

    return buffer;
}