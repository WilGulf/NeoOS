#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_EXEC | PROMISE_FB);
}

int main(int argc, char **argv) {
    printf("[ LAUNCHER ] BOOTING\n");
    drop_privilege(2);
    fork_as("1:/execs/sh", 3);
    while (1) {}
}