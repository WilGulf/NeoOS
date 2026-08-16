#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

void exit();

void *malloc(size_t size);
void free(void *ptr);

void *kmalloc(size_t size);
void kfree(void *ptr);

struct command_argument {
    char argument[512];
    struct command_argument *next;
};

struct process_arguments {
    int argc;
    char **argv;
};

struct command_argument *parse_command(const char *command, int max);
void process_get_arguments(struct process_arguments *arguments);

void exec(const char *filename);
int system(struct command_argument *arguments);
int system_run(const char *command);

#endif