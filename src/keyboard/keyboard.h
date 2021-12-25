#ifndef KEYHOARD_H
#define KEYHOARD_H

#include "../task/process.h"

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

typedef int KEYBOARD_CAPS_LOCK_STATE;
typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard {
  KEYBOARD_INIT_FUNCTION init;
  char name[20];
  KEYBOARD_CAPS_LOCK_STATE capslock_state;
  struct keyboard *next;
};

void keyboard_init();
void keyboard_backspace(struct process *process);
void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard *keyboard);
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard *keyboard);
void keyboard_set_capslock(struct keyboard *keyboard,
                           KEYBOARD_CAPS_LOCK_STATE state);

#endif
