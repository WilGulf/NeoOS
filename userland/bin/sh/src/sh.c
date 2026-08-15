#include "sh.h"

#include "stdlib.h"
#include "stdio.h"
#include "memory.h"

#define KEY_ENTER 0x0D
#define KEY_BACKSPACE 0x08

int main(int argc, char **argv) {
    printf("NeoOS v0.1.0\n");

    char cmd[256] = "";
    char *p;
    p = cmd;

    printf(" sh %% ");

    while(1) {
        char c = getkey();
        if (c != 0) {
            if (c == KEY_ENTER) {
                *p = '\0';
                putchar(c);
                exec(cmd);
                printf("%s", cmd);
                memset(cmd, 0, sizeof(cmd));
                p = cmd;
                
                printf("\n sh %% ");
            } else if (c == KEY_BACKSPACE) {
                *--p = 0;
                putchar(c);
            } else {
                *p = c;
                p++;
                putchar(c);
            }
        }
    }
}