#ifndef KEYHOARD_H
#define KEYHOARD_H

#include "../task/process.h"

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard
{
  KEYBOARD_INIT_FUNCTION init;
  char name[20];
  struct keyboard* next;
};

void keyboard_init();
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();

#endif 
