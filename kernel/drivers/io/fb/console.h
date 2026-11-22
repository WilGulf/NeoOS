#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../../include/stdint.h"

int console_init(void);

void console_clear(kbool refresh);
void console_new_line(kbool refresh);
void console_scroll(kbool refresh);
void console_backspace(kbool refresh);
void console_putc(char c, kbool refresh);
int console_writer(char *buf);

#endif