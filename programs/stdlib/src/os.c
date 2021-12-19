#include "os.h"

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