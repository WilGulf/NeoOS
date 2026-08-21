#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_KEY_TRUE 1
#define KEYBOARD_KEY_FALSE 0

typedef int KEYBOARD_KEY_STATE;
typedef int (*KEYBOARD_INIT_FUNCTION)();

struct process;

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];

    KEYBOARD_KEY_STATE lshift_state;
    KEYBOARD_KEY_STATE rshift_state;
    KEYBOARD_KEY_STATE capslock_state;
    struct keyboard *next;
};

void keyboard_init();

void keyboard_backspace(struct process *process);
KEYBOARD_KEY_STATE keyboard_get_capslock(struct keyboard *keyboard);
KEYBOARD_KEY_STATE keyboard_get_shift(struct keyboard *keyboard);

void keyboard_set_capslock(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_lshift(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);
void keyboard_set_rshift(struct keyboard *keyboard, KEYBOARD_KEY_STATE state);

void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard *keyboard);

#endif