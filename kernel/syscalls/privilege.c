#include "privilege.h"

#include "promise.h"
#include "../include/util.h"
#include "../task/task.h"
#include "../task/process.h"
#include "../include/status.h"
#include "../drivers/io/io.h"

kbool check_allowed_with_privilege(struct process *process, uint8_t privilege) {
    if (process->privilege > privilege) {
        kprintf("Process privilege not sufficient\n");
        kprintf("Process Terminated\n");
        process_terminate(process);
        task_next();
        return 0;
    }
    
    return 1;
}

void *isr80h_command2_drop_privilege(struct interrupt_frame *frame) {
    uint8_t privilege = (uint8_t)task_get_stack_item(task_current(), 0);
    if (privilege < task_current()->process->privilege) {
        process_terminate(task_current()->process);
        task_next();
        return 0;
    }
    
    task_current()->process->privilege = privilege;
}

void *isr80h_command13_system_run_as(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_EXEC);
    check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_EXECS);

    struct task *calling_task = task_current();
    uint8_t privilege = (uint8_t)task_get_stack_item(task_current(), 1);
    struct command_argument *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || !strlen(arguments[0].argument)) {
        return ERROR(-ERROR_INVALID_ARG);
    }
    
    struct command_argument *root_command_argument = &arguments[0];
    const char *program_name = root_command_argument->argument;

    char path[MAX_PATH];
    strncpy(path, program_name, sizeof(path));

    struct process *process = 0;
    int res = process_load_switch(path, &process);
    if (res < 0) {
        return ERROR(res);
    }

    res = process_inject_arguments(process, root_command_argument);
    if (res < 0) {
        return ERROR(res);
    }

    if (privilege < task_current()->process->privilege) {
        process_terminate(process);
        process_terminate(task_current()->process);
        task_next();
        return 0;
    }

    process->privilege = privilege;

    task_list_remove(calling_task);
    process->task->parent = calling_task;
    task_switch(process->task);
    task_return(&process->task->registers);

    return 0;
}

void *isr80h_command15_fork_as(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_EXEC);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_EXECS);
    if (res != true) {
        return 0;
    }

    uint8_t privilege = (uint8_t)task_get_stack_item(task_current(), 1);
    struct command_argument *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || !strlen(arguments[0].argument)) {
        return ERROR(-ERROR_INVALID_ARG);
    }

    struct command_argument *root_command_argument = &arguments[0];
    const char *program_name = root_command_argument->argument;

    char path[MAX_PATH];
    strncpy(path, program_name, sizeof(path));

    struct process *process = 0;
    res = process_load_switch(path, &process);
    if (res < 0) {
        return ERROR(res);
    }

    res = process_inject_arguments(process, root_command_argument);
    if (res < 0) {
        return ERROR(res);
    }

    if (privilege < task_current()->process->privilege) {
        process_terminate(process);
        process_terminate(task_current()->process);
        task_next();
        return 0;
    }

    process->privilege = privilege;
    
    if (!task_current()->process->first_child) {
        task_current()->process->first_child = process;
    } else {
        struct process *child = task_current()->process->first_child;
        while (child->sibling_child) {
            child = child->sibling_child;
        }
        
        child->sibling_child = process;
    }

    task_switch(process->task);
    task_return(&process->task->registers);

    return 0;
}