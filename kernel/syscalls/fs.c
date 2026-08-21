#include "fs.h"
#include "promise.h"
#include "privilege.h"

#include "../include/config.h"
#include "../include/util.h"
#include "../task/task.h"
#include "../task/process.h"
#include "../drivers/fs/file.h"
#include "../kernel.h"

void *isr80h_command16_fopen(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_FS);
    check_allowed_with_privilege(task_current()->process, PRIVILEGE_FS_DATA);

    int fd = 0;
    void *user_space_path_buffer = task_get_stack_item(task_current(), 0);
    void *user_space_mode_buffer = task_get_stack_item(task_current(), 1);
    char path[MAX_PATH];
    char mode[8];
    copy_string_from_task(task_current(), user_space_path_buffer, path, sizeof(path));
    copy_string_from_task(task_current(), user_space_mode_buffer, mode, sizeof(mode));

    if (strncmp(path, "0:/sys", 6) == 0) {
        check_allowed_with_privilege(task_current()->process, PRIVILEGE_FS_SYS);
    }

    fd = fopen(path, mode);

    return (void *)fd;
}

void *isr80h_command17_fclose(struct interrupt_frame *frame) {
    int fd = task_get_stack_item(task_current(), 0);
    fclose(fd);
    return 0;
}

void *isr80h_command18_fread(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_FS);
    check_allowed_with_privilege(task_current()->process, PRIVILEGE_FS_DATA);

    void *out = task_get_stack_item(task_current(), 0);
    size_t size = task_get_stack_item(task_current(), 1);
    size_t count = task_get_stack_item(task_current(), 2);
    int fd = task_get_stack_item(task_current(), 3);
    
    task_page();
    fread(out, size, count, fd);
    kernel_page();
    return 0;
}

void *isr80h_command19_fstat(struct interrupt_frame *frame) {
    check_process_promise(task_current()->process, PROMISE_FS);
    check_allowed_with_privilege(task_current()->process, PRIVILEGE_FS_DATA);

    int fd = task_get_stack_item(task_current(), 0);
    void *out = task_get_stack_item(task_current(), 1);
    task_page();
    fstat(fd, out);
    kernel_page();
    return 0;
}