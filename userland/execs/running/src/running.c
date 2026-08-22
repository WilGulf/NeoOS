#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    int max = 24;
    if (argv[1]) {
        
    }

    struct process processes[max];
    get_processes(&processes, max);

    printf("FILENAME  ID  PRIVILEGE\n");

    for (int i = 0; i < max; i++) {
        if (processes[i].filename[0] == 0)
            break;

        printf("%s  %d  %d\n", processes[i].filename, processes[i].id, processes[i].privilege);
    }

    exit();
}