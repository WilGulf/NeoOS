#include "string.h"
#include "stddef.h"
#include "memory.h"

char *strcpy(char *dest, const char *src) {
    char *res = dest;
    
    while (*src != 0) {
        *dest++ = *src++;
    }

    *dest = 0x00;

    return res;
}

char *strncpy(char *dest, const char *src, int max) {
    char *res = dest;

    int i = 0;
    while (*src != 0 && i < max) {
        *dest++ = *src++;
        i++;
    }

    *dest = 0x00;
    return res;
}

int strlen(char *src) {
    int i = 0;
    while (*src++) {
        i++;
    }
    return i;
}

int strnlen(char *src, int max) {
    int i = 0;
    for (; i < max; i++) {
        if (src[i] == 0) {
            break;
        }
    }

    return i;
}

int strcmp(char *str1, char *str2) {
    int i = 0;
    int failed = 0;
    while(str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            failed = 1;
            break;
        }

        i++;
    }

    if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0')) {
        failed = 1;
    }

    return failed;
}

int strncmp(char *str1, char *str2, int max) {
    unsigned char u1, u2;

    while (max-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

char *strchr(const char *str, int c) {
    while (*str != (char) c) {
        if (!*str++) {
            return 0;
        }
    }
    return (char *)str;
}

char *strrchr(const char *str, int c) {
    const char *p = 0;
    
    for (;;) {
        if (*str == (char)c) {
            p = str;
        }
        if (*str++ == '\0') {
            return (char *)p;
        }
    }
}

char *strstr(const char *str1, const char *str2) {
    size_t n = strlen(str2);
    while (*str1) {
        if (!memcmp(str1++, str2, n)) {
            return (char *)(str1 - 1);
        }
    }

    return 0;
}

char *olds = 0;
char *strtok(char *str, const char *delimiters) {
    static char *input = NULL;

    if (str != NULL) {
        input = str;
    }

    if (input == NULL) {
        return NULL;
    }

    while (*input) {
        bool delimiter = 0;
        for (int i = 0; delimiters[i]; i++) {
            if (*input == delimiters[i]) {
                delimiter = true;
                break;
            }
        }

        if (!delimiter) {
            break;
        }

        input++;
    }

    if (!input) {
        input = NULL;
        return NULL;
    }

    char *result = input;

    while (*input) {
        bool delimiter = 0;
        for (int i = 0; delimiters[i]; i++) {
            if (*input == delimiters[i]) {
                delimiter = true;
                break;
            }
        }

        if (delimiter) {
            *input = '\0';
            input++;
            return result;
        }

        input++;
    }

    input = NULL;
    return result;
}