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

int main(int argc, char **argv) {
    // VARIABLES
    char path1[256 + 9] = "0:/execs/";

    char cmd[256] = "";
    char *p;
    p = cmd;

    printf("\nNSH v%d.%d\n", VERSION, VERSION_PATCH);
    printf(" sh %% ");

    // MAIN LOOP
    while(1) {
        struct key_event event = {0, 0};
        getkey_event(&event);

        if (event.c != 0) {
            if (event.c == 'l') {
                if (event.modifiers == MODIFIER_CTRL) {
                    clear();
                    memset(cmd, 0, sizeof(cmd));
                    p = cmd;

                    putchar('\n');

                    printf(" sh %% ");

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
                } else {
                    strncpy(path1 + 9, cmd, sizeof(path1));

                    if (strchr(cmd, '&')) {
                        fork(cmd);
                        fork("");
                    } else {
                        system(path1);
                        system("");
                    }
                }

                if (cmd[0] != 0)
                    putchar('\n');

                memset(cmd, 0, sizeof(cmd));
                p = cmd;
                
                printf(" sh %% ");
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