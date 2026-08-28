#include "stdlib.h"
#include "stdio.h"

void declarations(void) {
    promise(PROMISE_FB | PROMISE_KILL);
}

int main(int argc, char **argv) {    
    if (argv[1]) {
        int id = atoi(argv[1]);
        kill(id);
    } else {
        printf("No process to kill\n");
    }

    exit();
}