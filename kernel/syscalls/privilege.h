#ifndef ISR80H_PRIVILEGE_H
#define ISR80H_PRIVILEGE_H

#include "../include/stdint.h"
#include "../task/process.h"

#define PRIVILEGE_PRINT 6
#define PRIVILEGE_CLEAR 6
#define PRIVILEGE_KEYBOARD 6
#define PRIVILEGE_EXEC_EXECS 4
#define PRIVILEGE_EXEC_SYSRO 0 

kbool check_allowed_with_privilege(struct process *process, uint8_t privilege);

struct interrupt_frame;

void *isr80h_command11_system_run_as(struct interrupt_frame *frame);
void *isr80h_command13_system_fork_as(struct interrupt_frame *frame);
void *isr80h_command14_drop_privilege(struct interrupt_frame *frame);

#endif