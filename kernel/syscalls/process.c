#include "process.h"

#include "../include/config.h"
#include "../include/status.h"
#include "../include/util.h"
#include "../task/task.h"
#include "../task/process.h"

void *isr80h_command6_process_load_start(struct interrupt_frame *frame) {
    void *filename_user_ptr = task_get_stack_item(task_current(), 0);
    char filename[MAX_PATH];
    int res = copy_string_from_task(task_current(), filename_user_ptr, filename, sizeof(filename));
    if (res < 0) {
        goto out;
    }

    struct process *process = 0;
    res = process_load_switch(filename, &process);
    if (res < 0) {
        goto out;
    }

    task_switch(process->task);
    task_return(&process->task->registers);
out:
    return 0;
}

void *isr80h_command7_system(struct interrupt_frame *frame) {
    struct task *calling_task = task_current();
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

    task_list_remove(calling_task);
    process->task->parent = calling_task;
    task_switch(process->task);
    task_return(&process->task->registers);

    return 0;
}

void *isr80h_command8_get_process_arguments(struct interrupt_frame *frame) {
    struct process *process = task_current()->process;
    struct process_arguments *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));

    process_get_arguments(process, &arguments->argc, &arguments->argv);
    return 0;
}

void *isr80h_command0_exit(struct interrupt_frame *frame) {
    struct process *process = task_current()->process;
    process_terminate(process);
    task_next();
}