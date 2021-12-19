#include "os.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char **argv) {
  printf("My age is: %i\n", 21);

  print("Hello how are you?\n");

  putchar('a');

  print(itoa(8762));

  void *ptr = malloc(512);

  if (ptr) {
    free(ptr);
  }

  char buff[1024];
  os_terminal_readline(buff, sizeof(buff), true);

  print(buff);

  while (1) {
  }

  return 0;
}
