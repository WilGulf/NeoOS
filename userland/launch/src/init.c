#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_EXEC);
}

int main(int argc, char **argv) {
    drop_privilege(2);
    fork_as("0:/execs/sh.elf", 4);
    while (1) {}
}