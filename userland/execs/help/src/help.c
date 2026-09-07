#include "stdio.h"
#include "stdlib.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    printf("\nShell\n");
    printf("- Clear: Clears the terminal\n");
    printf("- Exit: Exits the shell\n");
    putchar('\n');
    printf("System information\n");
    printf("- sysinfo: Privdes info close to what uname would provide on a UNIX-like system\n");
    printf("- fetch: Fetches and displays system info with the os logo\n");
    printf("- uptime: Prints the time since boot\n");
    putchar('\n');
    printf("Text & file utilities\n");
    printf("- echo: Echos the priveded arguments back\n");
    printf("- read: Prints the contents of a file\n");
    printf("- rm: Removes provided file(s)\n");
    printf("- crt: Creates listed file(s) with size 0\n");
    putchar('\n');
    printf("Process management\n");
    printf("- running: Lists running processes and their associated information\n");
    printf("- kill: Kills the provided process\n");
    exit();
}