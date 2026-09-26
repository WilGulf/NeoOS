#include "stdlib.h"
#include "stdio.h"
#include "promise.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    char cwd[MAX_PATH];
    get_cwd(cwd, sizeof(cwd));
    printf("%s", cwd);
    exit();
}