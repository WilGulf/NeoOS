#include "isr80h.h"
#include "../idt/idt.h"

#include "io.h"
#include "heap.h"
#include "process.h"
#include "promise.h"
#include "privilege.h"
#include "kernel.h"
#include "fs.h"

void isr80h_register_commands() {
    isr80h_register_command(SYSTEM_COMMAND0_EXIT, isr80h_command0_exit);
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    isr80h_register_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    isr80h_register_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    isr80h_register_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start);
    isr80h_register_command(SYSTEM_COMMAND7_SYSTEM, isr80h_command7_system);
    isr80h_register_command(SYSTEM_COMMAND8_GET_PROCESS_ARGUMENT, isr80h_command8_get_process_arguments);
    isr80h_register_command(SYSTEM_COMMAND9_FB_CLEAR, isr80h_command9_fb_clear);
    isr80h_register_command(SYSTEM_COMMAND10_PROMISE, isr80h_command10_promise);
    isr80h_register_command(SYSTEM_COMMAND11_SYSTEM_AS, isr80h_command11_system_run_as);
    isr80h_register_command(SYSTEM_COMMAND12_FORK, isr80h_command12_fork);
    isr80h_register_command(SYSTEM_COMMAND13_FORK_AS, isr80h_command13_fork_as);
    isr80h_register_command(SYSTEM_COMMAND14_DROP_PRIVILEGE, isr80h_command14_drop_privilege);
    isr80h_register_command(SYSTEM_COMMAND15_GET_KERNEL_INFO, isr80h_command15_get_kernel_info);
    isr80h_register_command(SYSTEM_COMMAND16_FOPEN, isr80h_command16_fopen);
    isr80h_register_command(SYSTEM_COMMAND17_FCLOSE, isr80h_command17_fclose);
    isr80h_register_command(SYSTEM_COMMAND18_FREAD, isr80h_command18_fread);
    isr80h_register_command(SYSTEM_COMMAND19_FSTAT, isr80h_command19_fstat);
}