#ifndef STRING_H
#define STRING_H

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int max);

int strlen(char *src);
int strnlen(char *src, int max);

int strcmp(char *str1, char *str2);
int strncmp(char *str1, char *str2, int max);

char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
char *strstr(const char *str1, const char *str2);

char *strtok(char *str, const char *delimiters);

#endif