#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"

#define KEY_ENTER 0x0D
#define KEY_BACKSPACE 0x08

void declarations(void) {
    promise(PROMISE_FB | PROMISE_INPUT | PROMISE_EXEC);
}

int main(int argc, char **argv) {
    printf("\nNeoOS v0.1.0, %s\n", argv[0]);

    char path1[256 + 9] = "0:/execs/";

    char cmd[256] = "";
    char *p;
    p = cmd;

    printf(" sh %% ");

    while(1) {
        char c = getkey();
        if (c != 0) {
            if (c == KEY_ENTER) {
                *p = '\0';
                putchar('\n');
                
                if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "CLEAR") == 0) {
                    clear();
                } else {
                    strncpy(path1 + 9, cmd, sizeof(path1));
                    system(path1);
                    system("");
                }

                if (cmd[0] != 0)
                    putchar('\n');

                memset(cmd, 0, sizeof(cmd));
                p = cmd;
                
                printf(" sh %% ");
            } else if (c == KEY_BACKSPACE) {
                if (cmd[0] != 0) {
                    *--p = 0;
                    putchar(c);
                }
            } else {
                *p = c;
                p++;
                putchar(c);
            }
        }
    }
}