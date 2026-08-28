#ifndef STRING_H
#define STRING_H

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int max);

int strlen(char *src);
int strnlen(char *src, int max);

int strcmp(char *str1, char *str2);
int strncmp(char *str1, char *str2, int max);
int istrncmp(const char* str1, const char* str2, int max);

char *strchr(const char *str, int c);
char *strstr(const char *str1, const char *str2);

int char_to_int(char c);
char char_upper_to_lower(char c);
char *strtok(char *str, const char *delimiters);

#endif