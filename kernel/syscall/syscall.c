#include "syscall.h"

#include "../drivers/io/io.h"
#include "../idt/idt.h"

static void *syscalls[1] = {
    &kprintf,
};
uint32_t num_syscalls = 1;

void syscall_handler(struct interrupt_registers *regs) {
    if (regs->eax >= num_syscalls) {
        return;
    }

    void *location = syscalls[regs->eax];

    int ret;
    asm volatile (" \
        push %1; \
        push %2; \
        push %3; \
        push %4; \
        push %5; \
        call *%6; \
        pop %%ebx; \
        pop %%ebx; \
        pop %%ebx; \
        pop %%ebx; \
        pop %%ebx; \
        " : "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));
    regs->eax = ret;
}

DEFN_SYSCALL1(kprintf, 0, const char*)