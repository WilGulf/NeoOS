#include "keyboard.h"
#include "ps2.h"

#include "../../kernel.h"
#include "../../idt/idt.h"
#include "../../task/task.h"
#include "../../include/stdint.h"
#include "../../include/util.h"
#include "../io/io.h"

#define PS2_KEYBOARD_CAPSLOCK 0x3A

#define PS2_KEYBOARD_LSHIFT_DOWN 0x2A
#define PS2_KEYBOARD_LSHIFT_UP 0xAA

#define PS2_KEYBOARD_LCTRL_DOWN 0x1D
#define PS2_KEYBOARD_LCTRL_UP 0x9D

#define PS2_KEYBOARD_LALT_DOWN 0x38
#define PS2_KEYBOARD_LALT_UP 0xB8

static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '!', '"', '#', '$', '%',
    '&', '/', '(', ')', '=', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ';', ':', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

int ps2_keyboard_init();
void ps2_keyboard_handle_interrupt();

struct keyboard ps2_keyboard = {
    .name = {"PS2"},
    .init = ps2_keyboard_init
};

int ps2_keyboard_init() {
    idt_register_interrupt_callback(ISR_KEYBOARD_INTERRUPT, ps2_keyboard_handle_interrupt);
    keyboard_set_capslock(&ps2_keyboard, KEYBOARD_KEY_FALSE);
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

char keyboard_upper_to_lower(char c) {
    if (c >= 'A' && c <= 'Z') {
        c = char_upper_to_lower(c);
        return c;
    }

    if (c == ':')
        return '.';
    if (c == ';')
        return '.';

    if (c == '!')
        return '1';
    if (c == '"')  
        return '2';
    if (c == '#')
        return '3';
    if (c == '$')
        return '4';
    if (c == '%')
        return '5';
    if (c == '&')
        return '6';
    if (c == '/')
        return '7';
    if (c == '(')
        return '8';
    if (c == ')')
        return '9';
    if (c == '=')
        return '0';
}

uint8_t ps2_keyboard_scancode_to_char(uint8_t scancode) {
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if (scancode >= size_of_keyboard_set_one) {
        return 0;
    }

    char c = keyboard_scan_set_one[scancode];
    if (keyboard_get_capslock(&ps2_keyboard) == KEYBOARD_KEY_FALSE) {
        if (!keyboard_get_shift(&ps2_keyboard)) {
            c = keyboard_upper_to_lower(c);
        }
    } else if (keyboard_get_shift(&ps2_keyboard)) {
        c = keyboard_upper_to_lower(c);
    }

    return c;
}

void ps2_keyboard_handle_interrupt() {
    kernel_page();
    uint8_t scancode = 0;
    scancode = inb(KEYBOARD_INPUT_PORT);
    inb(KEYBOARD_INPUT_PORT);

    if (scancode & PS2_KEYBOARD_KEY_RELEASED) {
        switch (scancode) {
            case PS2_KEYBOARD_LSHIFT_UP:
                keyboard_set_lshift(&ps2_keyboard, KEYBOARD_KEY_FALSE);
                break;

            case PS2_KEYBOARD_LCTRL_UP:
                keyboard_set_lctrl(&ps2_keyboard, KEYBOARD_KEY_FALSE);
                break;

            case PS2_KEYBOARD_LALT_UP:
               keyboard_set_lalt(&ps2_keyboard, KEYBOARD_KEY_FALSE); 
               break;
        }

        return;
    }

    uint8_t c = ps2_keyboard_scancode_to_char(scancode);

    if (scancode == PS2_KEYBOARD_CAPSLOCK) {
        KEYBOARD_KEY_STATE old_state = keyboard_get_capslock(&ps2_keyboard);
        keyboard_set_capslock(&ps2_keyboard, old_state == KEYBOARD_KEY_TRUE ? KEYBOARD_KEY_FALSE : KEYBOARD_KEY_TRUE);
        c = 0x00;
    }

    switch (scancode) {
        case PS2_KEYBOARD_LSHIFT_DOWN:
            keyboard_set_lshift(&ps2_keyboard, KEYBOARD_KEY_TRUE);
            c = 0x00;
            break;

        case PS2_KEYBOARD_LCTRL_DOWN:
            keyboard_set_lctrl(&ps2_keyboard, KEYBOARD_KEY_TRUE);
            c = 0x00;
            break;

        case PS2_KEYBOARD_LALT_DOWN:
            keyboard_set_lalt(&ps2_keyboard, KEYBOARD_KEY_TRUE);
            c = 0x00;
            break;
    }

    uint8_t modifiers = 0;
    if (keyboard_get_shift(&ps2_keyboard)) {
        modifiers |= MODIFIER_SHIFT;
    }
    if (keyboard_get_ctrl(&ps2_keyboard)) {
        modifiers |= MODIFIER_CTRL;
    }
    if (keyboard_get_alt(&ps2_keyboard)) {
        modifiers |= MODIFIER_ALT;
    }

    if (c != 0) {
        keyboard_push(c, modifiers);
    }

    task_page();
}

struct keyboard *ps2_init() {
    return &ps2_keyboard;
}