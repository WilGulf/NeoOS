#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"

#include "../include/stdint.h"
#include "../include/config.h"
#include "../drivers/fs/path_parser.h"

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_TYPE;

struct process {
    uint16_t id;
    char filename[MAX_PATH];
    struct task *task;
    void *allocations[MAX_PROGRAM_ALLOCATIONS];

    PROCESS_TYPE filetype;
    union {
        void *ptr;
        struct elf_file *elf_file;
    };

    void *stack;
    uint32_t size;

    struct keyboard_buffer {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_load_switch(const char *filename, struct process **process);
int process_load(const char *filename, struct process **process);

int process_switch(struct process *process);
void *process_malloc(struct process *process, size_t size);
void process_free(struct process *process, void *ptr);

struct process *process_current();
struct process *process_get(int id);

#endif