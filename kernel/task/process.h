#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"

#include "../include/stdint.h"
#include "../include/config.h"
#include "../drivers/fs/path_parser.h"

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12

struct process {
    uint16_t id;
    char filename[MAX_PATH];
    struct task *task;
    void *allocations[MAX_PROGRAM_ALLOCATIONS];
    void *ptr;
    void *stack;
    uint32_t size;
};

int process_load(const char *filename, struct process **process);

#endif