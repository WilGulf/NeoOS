#ifndef STDLIB_H
#define STDLIB_H

#include "promise.h"
#include "stdint.h"

#include <stddef.h>

void exit();

void *malloc(size_t size);
void free(void *ptr);

void *kmalloc(size_t size);
void kfree(void *ptr);

int promise(int promise);

struct kernel_info;

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

//void exec(const char *filename);

int system(const char *command);
int system_as(const char *command, uint8_t privilege);

int fork(const char *command);
int fork_as(const char *command, uint8_t privilege);

int drop_privilege(uint8_t privilege);

void get_kernel(void *out);

#endif