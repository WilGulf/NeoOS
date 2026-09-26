#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "stdbool.h"

#define VERSION 1
#define VERSION_PATCH 0

#define KEY_ENTER 0x0D
#define KEY_BACKSPACE 0x08

void declarations(void) {
    promise(PROMISE_FB | PROMISE_INPUT | PROMISE_EXEC);
}

int run(const char *cmd) {
    if (strchr(cmd, '&')) {
        fork_as(cmd, 4);
        fork("");
    } else {
        system_as(cmd, 4);
        system("");
    }

    return 0;
}

extern struct command_argument *command_to_arguments(const char *command);

void print_prompt(void) {
    char cwd[MAX_PATH];
    get_cwd(cwd, sizeof(cwd));
    char *path = strrchr(cwd, '/');
    printf(" sh %s %% ", path);
}

int main(int argc, char **argv) {
    // VARIABLES
    char path1[256 + 9] = "0:/execs/";

    char cmd[256] = "";
    char *p;
    p = cmd;

    printf("\nNSH v%d.%d\n", VERSION, VERSION_PATCH);
    print_prompt();

    // MAIN LOOP
    while(1) {
        struct key_event event = {0, 0};
        getkey_event(&event);

        if (event.c != 0) {
            if (event.c == 'l') {
                if (event.modifiers == MODIFIER_CTRL) {
                    clear();
                    putchar('\n');

                    print_prompt();
                    printf("%s", cmd);

                    continue;
                }
            }

            if (event.c == KEY_ENTER) {
                *p = '\0';
                putchar('\n');
                
                if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "CLEAR") == 0) {
                    clear();
                } else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "EXIT") == 0) {
                    exit();
                } else if (strncmp(cmd, "cd", 2) == 0 || strncmp(cmd, "CD", 2) == 0) {
                    struct command_argument *arg = command_to_arguments(cmd);
                    if (strncmp(arg->next->argument, "0:/sysro", 8) == 0 || strncmp(arg->next->argument, "sysro", 5) == 0) {
                        printf("Cancelling directory change. Privilege not met.\n");
                    } else {
                        if (chwd(arg->next->argument) != 0) {
                            printf("No such directory: %s\n", arg->next->argument);
                        }
                    }
                } else {
                    strncpy(path1 + 9, cmd, sizeof(path1));
                    run(path1);
                }

                printf("\033[1c");

                if (cmd[0] != 0)
                    putchar('\n');

                memset(cmd, 0, sizeof(cmd));
                p = cmd;
                
                print_prompt();
            } else if (event.c == KEY_BACKSPACE) {
                if (cmd[0] != 0) {
                    *--p = 0;
                    putchar(event.c);
                }
            } else {
                *p = event.c;
                p++;
                putchar(event.c);
            }
        }
    }

    exit();
}