#ifndef OS_H
#define OS_H

#include <stdbool.h>
#include <stddef.h>

struct command_argument {
  char argument[512];
  struct command_argument *next;
};

struct process_arguments {
  int argc;
  char **argv;
};

void print(const char *massage);

void os_exit();
int os_getkey_block();
int os_getkey();
void *os_malloc(size_t size);
void os_free(void *ptr);
void os_putchar(char c);
void os_terminal_readline(char *out, int max, bool output_while_typing);
void os_process_load_start(const char *filename);
struct command_argument *os_parse_command(const char *command, int max);
void os_process_get_arguments(struct process_arguments *arguments);
int os_system(struct command_argument *arguments);
int os_system_run(const char *command);

#endif
