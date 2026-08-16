#include "io.h"
#include "../task/task.h"
#include "../drivers/keyboard/keyboard.h"

#include "../drivers/io/io.h"

void *isr80h_command1_print(struct interrupt_frame *frame) {
    void *user_space_msg_buffer = task_get_stack_item(task_current(), 0);
    char buf[1024];
    copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));
    writer(buf);
    return 0;
}

void *isr80h_command2_getkey(struct interrupt_frame *frame) {
    if (get_input_process() != task_current()->process) {
        input_dest_process_switch(task_current()->process);
    }
    char c = keyboard_pop();
    return (void *)(int)c;
}

void *isr80h_command3_putchar(struct interrupt_frame *frame) {
    char c = (char)(int)task_get_stack_item(task_current(), 0);
    fb_putc(c);
    return 0;
}

void *isr80h_command9_fb_clear(struct interrupt_frame *frame) {
    fb_clear();
    return 0;
}