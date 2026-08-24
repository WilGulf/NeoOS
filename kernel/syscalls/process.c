#include "process.h"
#include "promise.h"
#include "privilege.h"

#include "../include/config.h"
#include "../include/status.h"
#include "../include/util.h"
#include "../task/task.h"
#include "../task/process.h"
#include "../kernel.h"

#include "../drivers/io/io.h"

void *isr80h_command0_exit(struct interrupt_frame *frame) {
    struct process *process = task_current()->process;
    process_terminate(process);
    task_next();
}

void *isr80h_command11_get_process_arguments(struct interrupt_frame *frame) {
    struct process *process = task_current()->process;
    struct process_arguments *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));

    process_get_arguments(process, &arguments->argc, &arguments->argv);
    return 0;
}

void *isr80h_command12_system(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_EXEC);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_EXECS);
    if (res != true) {
        return 0;
    }

    struct task *calling_task = task_current();
    struct command_argument *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || !strlen(arguments[0].argument)) {
        return ERROR(-ERROR_INVALID_ARG);
    }

    struct command_argument *root_command_argument = &arguments[0];
    const char *program_name = root_command_argument->argument;

    if (strncmp(program_name, "0:/sysro", 8) == 0) {
        check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_SYSRO);
    }

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

    process->privilege = calling_task->process->privilege;

    task_list_remove(calling_task);
    process->task->parent = calling_task;
    task_switch(process->task);
    task_return(&process->task->registers);

    return 0;
}

void *isr80h_command14_fork(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_EXEC);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_EXECS);
    if (res != true) {
        return 0;
    }

    struct command_argument *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || !strlen(arguments[0].argument)) {
        return ERROR(-ERROR_INVALID_ARG);
    }

    struct command_argument *root_command_argument = &arguments[0];
    const char *program_name = root_command_argument->argument;

    if (strncmp(program_name, "0:/sysro", 8) == 0) {
        check_allowed_with_privilege(task_current()->process, PRIVILEGE_EXEC_SYSRO);
    }

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

    process->privilege = task_current()->process->privilege;
    
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

void *isr80h_command16_get_processes(struct interrupt_frame *frame) {
    void *out = task_get_stack_item(task_current(), 0);
    uint16_t max = task_get_stack_item(task_current(), 1);

    struct process_public processes[max];
    memset(&processes, 0, sizeof(processes));
    int processes_found = 0;
    for (int i = 0; i < MAX_PROCESSES && processes_found < max; i++) {
        struct process *process = process_get(i);
        if (process != NULL) {
            strcpy(processes[processes_found].filename, process->filename);
            processes[processes_found].id = process->id;
            processes[processes_found].privilege = process->privilege;

            processes_found++;
        }
    }

    task_page();
    memcpy(out, processes, sizeof(processes));
    kernel_page();

    return 0;
}

void *isr80h_command17_terminate_process(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_KILL);
    check_allowed_with_privilege(task_current()->process, PRIVILEGE_KILL);

    uint16_t id = task_get_stack_item(task_current(), 0);
    struct process *process = process_get(id);
    if (process != NULL) {
        if (process->privilege < task_current()->process->privilege) {
            kprintf("Process trying to killing higher privileged process\n");
            kprintf("Process terminated\n");
            process_terminate(task_current()->process);
            task_next();
            return 0;
        }

        process_terminate(process);
        task_next();
        return 0;
    }

    return 0;
}