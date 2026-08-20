#include "stdlib.h"
#include "stdio.h"
#include "kernel.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    struct kernel_info info;
    get_kernel(&info);

    printf("A    M              dMMb  dMMM      OS: NeoOS v0.1.0\n");
    printf("MA   M              M  M  M         ----------------\n");
    printf("MAl  M              M  M  M         Kernel: %s-%d-%d-%d.%s\n", info.version.name, info.version.version_major, info.version.version_minor, info.version.version_patch, info.version.extension);
    printf("M A  M  dMMb  dMMb  M  M  M         Machine: i386\n");
    printf("M ll M  M  M  M  M  M  M  TMMb      \n");
    printf("M  A M  ?MMM  M  M  M  M     M      \n");
    printf("M  lLM     M  M  M  M  M     M      \n");
    printf("M.  VM  ?MMP  ?MMP  ?MMP  ?MMP      \n");

    exit();
}