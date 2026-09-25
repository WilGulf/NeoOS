#include "stdio.h"

#include <stdarg.h>
#include <string.h>
#include <memory.h>

extern void print(const char *msg);

void prntnum(unsigned long n, int base, char sign, char *outbuf);

int vsprintf(char *out, const char *fmt, va_list arg_ptr) {
    const char *p;
    char *sval;
    char *integer;
    char *hexa;
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

            case 'x': {
                break;
            }

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
    char message[1024];

    va_list args;
    va_start(args, fmt);
    vsprintf(message, fmt, args);
    va_end(args);

    memcpy(out, message, len);

    return 0;
}

int printf(const char *fmt, ...) {
    char message[1024];

    va_list args;
    va_start(args, fmt);
    vsprintf(message, fmt, args);
    va_end(args);

    print(message);

    return 0;
}

char *itoa(int i) {
    int i2 = i;
    static char text[12];
    int textpos = 0;

    if (i == 0) {
        text[textpos++] = '0';
    }

    if (i < 0) {
        text[textpos++] = '-';
        i2 = -i;
    }

    while (i2) {
        text[textpos++] = '0' + (i2 % 10);
        i2 /= 10;
    }

    text[textpos] = '\0';

    int a = (text[0] == '-') ? 1 : 0;
    for (int b = textpos - 1; a < b; a++, b--) {
        char tmp = text[a];
        text[a] = text[b];
        text[b] = tmp;
    }

    return text;
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

void prntnum(unsigned long num, int base, char sign, char *outbuf)
{

    int i = 12;
    int j = 0;

    do{
        outbuf[i] = "0123456789ABCDEF"[num % base];
        i--;
        num = num/base;
    }while( num > 0);

    if(sign != ' '){
        outbuf[0] = sign;
        ++j;
    }

    while( ++i < 13){
       outbuf[j++] = outbuf[i];
    }

    outbuf[j] = 0;

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