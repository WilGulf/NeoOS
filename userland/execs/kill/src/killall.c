#include "stdlib.h"
#include "stdio.h"
#include "promise.h"
#include "stdbool.h"
#include "string.h"

void declarations(void) {
    promise(PROMISE_KILL | PROMISE_FB);
}

int main(int argc, char **argv) {
    bool killed = false;

    if (argv[1]) {
        struct process processes[12];
        get_processes(&processes, 12);

        for (int i = 0; i < 12; i++) {
            if (processes[i].filename[0] == 0)
                break;

            
        }

        if (killed) {
            exit();
        }
    }

out:
    printf("No process to kill\n");
    exit();
}