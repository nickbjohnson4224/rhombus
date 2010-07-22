#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* keymap */
extern const char keymap[];

/* line buffering (thread-safe) */
void push_char(char c);
void push_line(char *l);
char *pop_line(void);
char pop_char (void);

#endif
