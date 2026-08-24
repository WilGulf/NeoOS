#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../../include/stdint.h"

#define KEYBOARD_KEY_TRUE 1
#define KEYBOARD_KEY_FALSE 0

#define MODIFIER_SHIFT 0B00000001
#define MODIFIER_CTRL  0B00000010
#define MODIFIER_ALT   0B00000100

typedef int KEYBOARD_KEY_STATE;
typedef int (*KEYBOARD_INIT_FUNCTION)();

struct process;

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];

    KEYBOARD_KEY_STATE lshift_state;
    KEYBOARD_KEY_STATE rshift_state;

    KEYBOARD_KEY_STATE lctrl_state;

    KEYBOARD_KEY_STATE lalt_state;

    KEYBOARD_KEY_STATE capslock_state;
    struct keyboard *next;
};

struct key_event {
    char key;
    uint8_t modifiers;
};

void keyboard_init();

void keyboard_backspace(struct process *process);
KEYBOARD_KEY_STATE keyboard_get_capslock(struct keyboard *keyboard);
KEYBOARD_KEY_STATE keyboard_get_shift(struct keyboard *keyboard);
KEYBOARD_KEY_STATE keyboard_get_ctrl(struct keyboard *keyboard);
KEYBOARD_KEY_STATE keyboard_get_alt(struct keyboard *keyboard);

void keyboard_set_capslock(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_lshift(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_rshift(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_lctrl(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_lalt(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);

void keyboard_push(char c, uint8_t modifiers);
struct key_event keyboard_pop_event();
char keyboard_pop();
int keyboard_insert(struct keyboard *keyboard);

#endif