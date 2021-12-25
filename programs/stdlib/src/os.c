#include "os.h"
#include "string.h"

struct command_argument *os_parse_command(const char *command, int max) {
  struct command_argument *root_command = 0;
  char scommand[512];

  if (max >= (int)sizeof(scommand)) {
    return 0;
  }

  strncpy(scommand, command, sizeof(scommand));

  char *token = strtok(scommand, " ");

  if (!token) {
    goto out;
  }

  root_command = os_malloc(sizeof(struct command_argument));

  if (!root_command) {
    goto out;
  }

  strncpy(root_command->argument, token, sizeof(root_command->argument));
  root_command->next = 0;

  struct command_argument *current = root_command;
  token = strtok(NULL, " ");

  while (token != 0) {
    struct command_argument *new_command =
        os_malloc(sizeof(struct command_argument));

    if (!new_command) {
      break;
    }

    strncpy(new_command->argument, token, sizeof(new_command->argument));
    new_command->next = 0;
    current->next = new_command;
    current = new_command;
    token = strtok(NULL, " ");
  }

out:
  return root_command;
}

int os_getkey_block() {
  int val = os_getkey();

  while (val == 0) {
    val = os_getkey();
  }

  return val;
}

void os_terminal_readline(char *out, int max, bool output_while_typing) {
  int index = 0;

  for (index = 0; index < max - 1; index++) {
    char key = os_getkey_block();

    // carriage return means we have read the line
    if (key == 13) {
      break;
    }

    if (output_while_typing) {
      os_putchar(key);
    }

    // backspace
    if (key == 0x08 && index >= 1) {
      out[index - 1] = 0x00;
      index -= 2;
      continue;
    }

    out[index] = key;
  }

  out[index] = 0x00;
}
