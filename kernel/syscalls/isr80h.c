#include "isr80h.h"
#include "../idt/idt.h"

#include "io.h"
#include "heap.h"
#include "process.h"
#include "promise.h"
#include "privilege.h"
#include "kernel.h"
#include "fs.h"
#include "timer.h"

void isr80h_register_commands() {
    isr80h_register_command(SYSTEM_COMMAND0_EXIT, isr80h_command0_exit);
    isr80h_register_command(SYSTEM_COMMAND1_PROMISE, isr80h_command1_promise);
    isr80h_register_command(SYSTEM_COMMAND2_DROP_PRIVILEGE, isr80h_command2_drop_privilege);
    isr80h_register_command(SYSTEM_COMMAND3_GET_KERNEL_INFO, isr80h_command3_get_kernel_info);
    isr80h_register_command(SYSTEM_COMMAND4_PRINT, isr80h_command4_print);
    isr80h_register_command(SYSTEM_COMMAND5_PUTCHAR, isr80h_command5_putchar);
    isr80h_register_command(SYSTEM_COMMAND6_FB_CLEAR, isr80h_command6_fb_clear);
    isr80h_register_command(SYSTEM_COMMAND7_GET_KEY, isr80h_command7_getkey);
    isr80h_register_command(SYSTEM_COMMAND8_GET_KEY_EVENT, isr80h_command8_get_key_event);
    isr80h_register_command(SYSTEM_COMMAND9_MALLOC, isr80h_command9_malloc);
    isr80h_register_command(SYSTEM_COMMAND10_FREE, isr80h_command10_free);
    isr80h_register_command(SYSTEM_COMMAND11_GET_PROCESS_ARGUMENT, isr80h_command11_get_process_arguments);
    isr80h_register_command(SYSTEM_COMMAND12_SYSTEM, isr80h_command12_system);
    isr80h_register_command(SYSTEM_COMMAND13_SYSTEM_AS, isr80h_command13_system_run_as);
    isr80h_register_command(SYSTEM_COMMAND14_FORK, isr80h_command14_fork);
    isr80h_register_command(SYSTEM_COMMAND15_FORK_AS, isr80h_command15_fork_as);
    isr80h_register_command(SYSTEM_COMMAND16_GET_PROCESSES, isr80h_command16_get_processes);
    isr80h_register_command(SYSTEM_COMMAND17_TERMINATE_PROCESS, isr80h_command17_terminate_process);
    isr80h_register_command(SYSTEM_COMMAND18_FOPEN, isr80h_command18_fopen);
    isr80h_register_command(SYSTEM_COMMAND19_FCLOSE, isr80h_command19_fclose);
    isr80h_register_command(SYSTEM_COMMAND20_FREAD, isr80h_command20_fread);
    isr80h_register_command(SYSTEM_COMMAND21_FSTAT, isr80h_command21_fstat);
    //isr80h_register_command(SYSTEM_COMMAND22_FSEEK, isr80h_command18_fopen);
    isr80h_register_command(SYSTEM_COMMAND23_GET_TICKS_MS, isr80h_command23_get_ticks_ms);
    isr80h_register_command(SYSTEM_COMMAND24_DISABLE_CURSOR, isr80h_command24_disable_cursor);
    isr80h_register_command(SYSTEM_COMMAND25_ENABLE_CURSOR, isr80h_command25_enable_cursor);
    isr80h_register_command(SYSTEM_COMMAND26_REMOVE, isr80h_command26_remove);
    isr80h_register_command(SYSTEM_COMMAND27_FWRITE, isr80h_command27_fwrite);
}