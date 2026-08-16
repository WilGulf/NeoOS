#include "stdlib.h"

#include "string.h"

void *malloc(size_t size) {
    return kmalloc(size);
}

void free(void *ptr) {
    return kfree(ptr);
}

struct command_argument *parse_command(const char *command, int max) {
    struct command_argument *root_command = 0;
    char scommand[1025];
    if (max >= (int)sizeof(scommand)) {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));
    char *token = strtok(scommand, " ");
    if (!token) {
        goto out;
    }

    root_command = malloc(sizeof(struct command_argument));
    if (!root_command) {
        goto out;
    }

    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct command_argument *current = root_command;
    token = strtok(NULL, " ");
    while (token != 0) {
        struct command_argument *new_command = malloc(sizeof(struct command_argument));
        if (!new_command) {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }
out:
    return root_command;
}

int system_run(const char *command) {
    char buffer[1024];
    strncpy(buffer, command, sizeof(buffer));
    struct command_argument *root_command_argument = parse_command(buffer, sizeof(buffer));
    if (!root_command_argument) {
        return -1;
    }
    
    return system(root_command_argument);
}