#include "stdlib.h"
#include "stdio.h"
#include "kernel.h"

void declarations(void) {
    promise(PROMISE_FB);
}

int main(int argc, char **argv) {
    struct kernel_info info;
    get_kernel(&info);

    uint32_t ms = get_ticks_ms();
    uint32_t total_seconds = ms / 1000;

    uint32_t hours   = total_seconds / 3600;
    uint32_t minutes = (total_seconds / 60) % 60;
    uint32_t seconds = total_seconds % 60;

    printf("\033[35mA    M              dMMb  dMMM\033[0m      OS: NeoOS v0.1.0\n");
    printf("\033[35mMA   M              M  M  M         ----------------\n");
    printf("\033[35mMAl  M              M  M  M   \033[0m      Kernel: %s-%d-%d-%d.%s\n", info.version.name, info.version.version_major, info.version.version_minor, info.version.version_patch, info.version.extension);
    printf("\033[35mM A  M  dMMb  dMMb  M  M  M   \033[0m      Machine: i386\n");
    printf("\033[35mM ll M  M  M  M  M  M  M  TMMb\033[0m      Uptime: ");

    if (hours)
        printf("Hours: %d ", hours);
    else if (minutes)
        printf("Minutes: %d ", minutes);
    else
        printf("Seconds: %d\n", seconds);

    printf("\033[35mM  A M  MMMP  M  M  M  M     M\033[0m      \n");
    printf("\033[35mM  lLM  M     M  M  M  M     M\033[0m      \n");
    printf("\033[35mM.  VM  ?MMP  ?MMP  ?MMP  ?MMP\033[0m      \n");

    exit();
}