/*
 *
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
  * This file is drerived from the PeachOS Kernel (github.com/nibblebits/PeachOS).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * 
 * See NOTICE for the full list of GPL-derived files in this project.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"
#include "../drivers/keyboard/keyboard.h"

#include "../include/stdint.h"
#include "../include/config.h"
#include "../drivers/fs/path_parser.h"

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_TYPE;

struct process_allocation {
    void *ptr;
    size_t size;
};

struct command_argument {
    char argument[512];
    struct command_argument *next;
};

struct process_arguments {
    int argc;
    char **argv;
};

struct process {
    uint16_t id;
    struct process *fork_parent;
    struct process *first_child;
    struct process *sibling_child;

    char filename[MAX_PATH];
    char cwd[MAX_PATH];
    struct task *task;

    struct process_allocation allocations[MAX_PROGRAM_ALLOCATIONS];

    PROCESS_TYPE filetype;
    union {
        void *ptr;
        struct elf_file *elf_file;
    };

    void *stack;
    uint32_t size;

    struct keyboard_buffer {
        struct key_event buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;

    struct process_arguments arguments;

    uint32_t promises;
    kbool declared;

    uint8_t privilege;
};

int process_load_switch(const char *filename, struct process **process);
int process_load(const char *filename, struct process **process);
int process_terminate(struct process *process);

int process_switch(struct process *process);
void *process_malloc(struct process *process, size_t size);
void process_free(struct process *process, void *ptr);

struct process *process_current();
struct process *process_get(int id);

int input_dest_process_switch(struct process *process);
struct process *get_input_process();

void process_get_arguments(struct process *process, int *argc, char ***argv);
int process_inject_arguments(struct process *process, struct command_argument *root_argument);

#endif