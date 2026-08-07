#include "util.h"
#include "stdint.h"

char char_upper_to_lower(char s1) {
    if (s1 >= 65 && s1 <= 90) {
        s1 += 32;
    }

    return s1;
}

void *memset(void *ptr, int c, size_t size) {
    char *c_ptr = (char*) ptr;
    for (int i = 0; i < size; i++){
        c_ptr[i] = (char) c;
    }

    return ptr;
}

void *memcpy(void *dest, void *src, uint32_t len) {
    char *d = dest;
    char *s = src;
    while (len--) {
        *d++ = *s++;
    }

    return dest;
}

int memcmp(void* s1, void* s2, int count) {
    char* c1 = s1;
    char* c2 = s2;
    while(count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
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


int istrncmp(const char* s1, const char* s2, int n) {
    unsigned char u1, u2;
    while(n-- > 0) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && char_upper_to_lower(u1) != char_upper_to_lower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
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