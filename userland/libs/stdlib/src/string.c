#include "string.h"
#include "stddef.h"
#include "memory.h"

char char_upper_to_lower(char c) {
    if (c >= 65 && c <= 90) {
        c += 32;
    }

    return c;
}

char *strcpy(char *dest, const char *src) {
    char *res = dest;
    while (*src != 0) {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

char *strncpy(char *dest, const char *src, int max) {
    int i = 0;
    for (; i < max - 1; i++) {
        if (src[i] == 0x00) {
            break;
        }

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
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

int istrncmp(const char* str1, const char* str2, int max) {
    unsigned char u1, u2;
    while(max-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2 && char_upper_to_lower(u1) != char_upper_to_lower(u2))
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

char *strstr(const char *str1, const char *str2) {
    size_t n = strlen(str2);
    while (*str1) {
        if (!memcmp(str1++, str2, n)) {
            return (char *)(str1 - 1);
        }
    }

    return 0;
}

int char_to_int(char c) {
    return c - '0';
}

char *sp = 0;
char *strtok(char *str, const char *delimiters) {
    int i = 0;
    int len = strlen(delimiters);
    if (!str && !sp) {
        return 0;
    }
    
    if (str && sp) {
        sp = str;
    }

    char *p_start = sp;
    while(1) {
        for (i = 0; i < len; i++) {
            if (*p_start == delimiters[i]) {
                p_start++;
                break;
            }
        }

        if (i == len) {
            sp = p_start;
            break;
        }
    }

    if (*sp == '\0') {
        sp = 0;
        return sp;
    }

    while(*sp != '\0') {
        for (i = 0; i < len; i++) {
            if (*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }

        sp++;
        if (i < len) {
            break;
        }
    }

    return p_start;
}