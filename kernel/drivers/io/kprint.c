#include "io.h"
#include "../../include/util.h"

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

int int_to_str(int x, char str[], int d) {
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0'; 
        x = x / 10;
    }

    while (i < d) {
        str[i++] = '0';
    }
    
    int len = i;
    int j = 0, k = len - 1, temp;
    while (j < k) { 
        temp = str[j]; 
        str[j] = str[k]; 
        str[k] = temp; 
        j++; 
        k--; 
    }
    
    str[i] = '\0';
    return i;
}

int kprintf(char *buf, ...) {
    va_list args;
    va_start(args, buf);

    char output[256];

    int buf_i = 0;
    int output_i = 0;
    while (buf[buf_i] != 0) {
        if (buf[buf_i] == '%') {
            switch (buf[buf_i + 1]) {

            case 's': {
                char * arg = va_arg(args, char *);
                
                int i = 0;
                while (arg[i] != 0) {
                    output[output_i] = arg[i];

                    output_i++;
                    i++;
                }

                break;
            }
            
            case 'c': {
                output[output_i] = va_arg(args, int);
                output_i++;

                break;
            }

            case 'd': {
                char str[32];
                int_to_str(va_arg(args, int), str, 0);

                int i = 0;
                while (str[i] != 0) {
                    output[output_i] = str[i];

                    output_i++;
                    i++;
                }
                

                break;
            }
            
            case 'f': {
                float n = va_arg(args, double);
                char res[32];

                int ipart = (int)n;
                float fpart = n - (float)ipart;

                int i = int_to_str(ipart, res, 0);

                int afterpoint = 5;

                if (afterpoint != 0) {
                    res[i] = '.';
                    fpart = fpart * pow(10, afterpoint) + 0.5;
                    int_to_str((int)fpart, res + i + 1, afterpoint);
                }

                i = 0;
                while (res[i] != 0) {
                    output[output_i] = res[i];
                    output_i++;
                    i++;
                }

                break;
            }
            
            default:
                break;
            }

            buf_i = buf_i + 2;
        } else {
            output[output_i] = buf[buf_i];

            output_i++;
            buf_i++;
        }
    }

    va_end(args);
    
    writer(output);

    return 0;
}