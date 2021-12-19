#ifndef OS_H
#define OS_H

#include <stdbool.h>
#include <stddef.h>

void print(const char *massage);

int os_getkey_block();
int os_getkey();
void *os_malloc(size_t size);
void os_free(void *ptr);
void os_putchar(char c);
void os_terminal_readline(char *out, int max, bool output_while_typing);

#endif
