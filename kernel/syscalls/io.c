#include "io.h"
#include "promise.h"
#include "privilege.h"

#include "../task/task.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/io/io.h"
#include "../kernel.h"

void *isr80h_command1_print(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_FB);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_PRINT);
    if (res != true) {
        return 0;
    }

    void *user_space_msg_buffer = task_get_stack_item(task_current(), 0);
    char buf[1024];
    copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));
    writer(buf);
    return 0;
}

void *isr80h_command2_getkey(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_INPUT);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_KEYBOARD);
    if (res != true) {
        return 0;
    }
    
    if (get_input_process() != task_current()->process) {
        input_dest_process_switch(task_current()->process);
    }
    char c = keyboard_pop();
    return (void *)(int)c;
}

void *isr80h_command3_putchar(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_FB);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_PRINT);
    if (res != true) {
        return 0;
    }
    
    char c = (char)(int)task_get_stack_item(task_current(), 0);
    fb_putc(c);
    return 0;
}

void *isr80h_command9_fb_clear(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_FB);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_CLEAR);
    if (res != true) {
        return 0;
    }

    fb_clear();
    return 0;
}

void *isr80h_command23_get_key_event(struct interrupt_frame *frame) {
    int res = check_process_promise(task_current()->process, PROMISE_INPUT);
    res = check_allowed_with_privilege(task_current()->process, PRIVILEGE_KEYBOARD);
    if (res != true) {
        return 0;
    }
    
    if (get_input_process() != task_current()->process) {
        input_dest_process_switch(task_current()->process);
    }
    struct key_event event = keyboard_pop_event();

    struct key_event *out = (struct key_event *)task_get_stack_item(task_current(), 0);

    task_page();
    *out = event;
    kernel_page();

    return 0;
}