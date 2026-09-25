#include "util.h"
#include "stdint.h"

char char_upper_to_lower(char s1) {
    if (s1 >= 65 && s1 <= 90) {
        s1 += 32;
    }

    return s1;
}

void *memset(void *ptr, int c, size_t size) {
    unsigned char *b = ptr;
    for (int i = 0; i < size; i++) {
        *b = c;
        b++;
    }
    return ptr;
}

void *memcpy(void *dest, void *src, int len) {
    char *d = dest;
    char *s = src;
    
    int i = 0;
    while (i < len) {
        *d++ = *s++;
        i++;
    }

    return dest;
}

int memcmp(void* ptr1, void* ptr2, int count) {
    char *char1 = ptr1;
    char *char2 = ptr2;

    while (count-- && *char1 == *char2) {
        char1++;
        char2++;
    }

    return 0;
}

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

char *strstr(const char *str1, const char *str2) {
    size_t n = strlen(str2);
    while (*str1) {
        if (!memcmp(str1++, str2, n)) {
            return (char *)(str1 - 1);
        }
    }

    return 0;
}

kbool char_is_digit(char c) {
    return c >= 48 && c <= 57;
}

int char_to_int(char c) {
    return c - '0';
}

int pow(int x, int power) {
    long int result = 1;

    for (int i = 1; i <= power; i++) {
        result = result * x;
    }
    
    return result;
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