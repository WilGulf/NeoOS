#include "io.h"

#include "../../include/util.h"
#include "../../include/va_list.h"
#include "../../timer/timer.h"

int vsprintf(char *out, const char *fmt, va_list arg_ptr) {
    const char *p;
    char *sval;
    int cval, ival;

    char buffer[1024];
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
                ival = va_arg(arg_ptr, int);
                integer = itoa(ival);
                while (*integer && buffer_i < 1022) {
                    buffer[buffer_i++] = *integer++;
                }
                
                break;

            case 's':
                sval = va_arg(arg_ptr, char *);
                while (*sval && buffer_i < 1022) {
                    buffer[buffer_i++] = *sval++;
                }

                break;

            case 'c':
                cval = va_arg(arg_ptr, int);
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

    memcpy(out, buffer, buffer_i + 1);

    return 0;
}

int sprintf(char *out, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vsprintf(out, fmt, args);

    va_end(args);
    return 0;
}

int snprintf(char *out, size_t len, char *fmt, ...) {

}

int kprintf(const char *fmt, ...) {
    va_list args;

    char message[1024];
    char buffer[1152];

    uint32_t ms = timer_get_ms();
    int sec = ms / 1000;
    int sec_ms = ms % 1000;

    va_start(args, fmt);

    vsprintf(message, fmt, args);

    va_end(args);

    sprintf(
        buffer,
        "[ KERNEL ] [%d.%d] %s",
        sec,
        sec_ms,
        message
    );

    writer(buffer);

    return 0;
}